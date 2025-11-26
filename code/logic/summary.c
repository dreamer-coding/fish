/*
 * -----------------------------------------------------------------------------
 * Project: Fossil Logic
 *
 * This file is part of the Fossil Logic project, which aims to develop high-
 * performance, cross-platform applications and libraries. The code contained
 * herein is subject to the terms and conditions defined in the project license.
 *
 * Author: Michael Gene Brockus (Dreamer)
 *
 * Copyright (C) 2024 Fossil Logic. All rights reserved.
 * -----------------------------------------------------------------------------
 */
#include "fossil/code/commands.h"

#define MAX_INPUT_SIZE   (256 * 1024)   // 256 KB buffer for input file
#define MAX_SENTENCES    2048
#define MAX_WORDS_SENT   2048
#define MAX_WORD_LEN     64
#define MAX_VOCAB        65536

typedef struct {
    char *word;
    int df;        // number of sentences containing the word
    long tf_total; // total occurrences across all sentences (not strictly required)
} VocabEntry;

static void strtolower_inplace(char *s) {
    while (*s) { *s = (char)tolower((unsigned char)*s); s++; }
}

/* Trim leading/trailing whitespace */
static char *trim(char *s) {
    while (*s && isspace((unsigned char)*s)) s++;
    if (*s == 0) return s;
    char *end = s + strlen(s) - 1;
    while (end > s && isspace((unsigned char)*end)) { *end = '\0'; end--; }
    return s;
}

/* Split content into sentences. Returns number of sentences. */
static int split_sentences(char *content, char *sentences[], int max_sentences) {
    int count = 0;
    char *p = content;
    char *start = p;
    while (*p && count < max_sentences) {
        if (*p == '.' || *p == '?' || *p == '!' || *p == '\n') {
            // include character, then terminate sentence
            char save = p[1];
            p[1] = '\0';
            char *s = trim(start);
            if (*s) {
                // duplicate so we own memory
                sentences[count++] = strdup(s);
            }
            p[1] = save;
            // skip separator
            p++;
            while (*p && isspace((unsigned char)*p)) p++;
            start = p;
            continue;
        }
        p++;
    }
    // last chunk if any
    if (*start && count < max_sentences) {
        char *s = trim(start);
        if (*s) sentences[count++] = strdup(s);
    }
    return count;
}

/* Tokenize a sentence into words (alphanumeric sequences). Returns word count. */
static int tokenize_sentence(const char *sent, char words[][MAX_WORD_LEN], int max_words) {
    int w = 0;
    size_t len = strlen(sent);
    char buf[MAX_WORD_LEN];
    int bi = 0;
    for (size_t i = 0; i <= len && w < max_words; ++i) {
        char c = sent[i];
        if (isalnum((unsigned char)c)) {
            if (bi < MAX_WORD_LEN - 1) buf[bi++] = (char)tolower((unsigned char)c);
        } else {
            if (bi > 0) {
                buf[bi] = '\0';
                strncpy(words[w++], buf, MAX_WORD_LEN);
                bi = 0;
            }
        }
    }
    return w;
}

/* Vocabulary helpers using simple linear search (ok for moderate text sizes) */
static int vocab_find(VocabEntry *vocab, int vcount, const char *word) {
    for (int i = 0; i < vcount; ++i)
        if (strcmp(vocab[i].word, word) == 0) return i;
    return -1;
}

static int vocab_add(VocabEntry *vocab, int *vcount, const char *word) {
    int idx = vocab_find(vocab, *vcount, word);
    if (idx >= 0) return idx;
    if (*vcount >= MAX_VOCAB) return -1;
    vocab[*vcount].word = strdup(word);
    vocab[*vcount].df = 0;
    vocab[*vcount].tf_total = 0;
    return (*vcount)++;
}

/**
 * fish_summary: real extractive summarizer
 * - file_path: path to file to summarize
 * - depth: 1 -> 1 sentence, 2 -> 3, 3 -> 5, >=4 -> min(10, N)
 * - time_flag: print timing if 1
 */
int fish_summary(const char *file_path, int depth, int time_flag)
{
    if (!file_path) {
        fprintf(stderr, "fish_summary: null file path\n");
        return -1;
    }

    FILE *fp = fopen(file_path, "rb");
    if (!fp) {
        fprintf(stderr, "fish_summary: cannot open '%s'\n", file_path);
        return -1;
    }

    char *content = malloc(MAX_INPUT_SIZE);
    if (!content) { fclose(fp); fprintf(stderr, "fish_summary: OOM\n"); return -1; }

    size_t r = fread(content, 1, MAX_INPUT_SIZE - 1, fp);
    content[r] = '\0';
    fclose(fp);

    clock_t t0 = 0, t1 = 0;
    if (time_flag) t0 = clock();

    /* 1) Split into sentences */
    char *sentences[MAX_SENTENCES];
    int nsent = split_sentences(content, sentences, MAX_SENTENCES);
    if (nsent == 0) {
        printf("[Summary] (empty or no sentences)\n");
        free(content);
        return 0;
    }

    /* 2) Build vocab and per-sentence term lists */
    VocabEntry *vocab = malloc(sizeof(VocabEntry) * MAX_VOCAB);
    if (!vocab) { fprintf(stderr, "fish_summary: OOM vocab\n"); free(content); return -1; }
    int vcount = 0;

    // per-sentence: array of word indices (unique presence) and counts
    int *sent_word_counts[MAX_SENTENCES]; // parallel arrays sized to vcount after building
    int sent_word_counts_len[MAX_SENTENCES];
    for (int i = 0; i < nsent; ++i) { sent_word_counts[i] = NULL; sent_word_counts_len[i] = 0; }

    // temporary storage of tokens per sentence
    char tokens[MAX_WORDS_SENT][MAX_WORD_LEN];

    for (int i = 0; i < nsent; ++i) {
        int tw = tokenize_sentence(sentences[i], tokens, MAX_WORDS_SENT);
        // track unique words in sentence to compute DF
        int unique_indices[MAX_WORDS_SENT];
        int unique_count = 0;

        // dynamic map from word idx -> count in sentence via linear arrays
        int idx_list[MAX_WORDS_SENT];
        int count_list[MAX_WORDS_SENT];
        int idx_list_len = 0;

        for (int t = 0; t < tw; ++t) {
            if (tokens[t][0] == '\0') continue;
            int vidx = vocab_find(vocab, vcount, tokens[t]);
            if (vidx < 0) {
                vidx = vocab_add(vocab, &vcount, tokens[t]);
                if (vidx < 0) break;
            }
            vocab[vidx].tf_total += 1;

            // find in idx_list
            int found = -1;
            for (int z = 0; z < idx_list_len; ++z) {
                if (idx_list[z] == vidx) { found = z; break; }
            }
            if (found >= 0) {
                count_list[found] += 1;
            } else {
                idx_list[idx_list_len] = vidx;
                count_list[idx_list_len] = 1;
                idx_list_len++;
                // mark unique for DF increment later
                unique_indices[unique_count++] = vidx;
            }
        }

        // increment DF for unique words in this sentence
        for (int u = 0; u < unique_count; ++u) {
            int vidx = unique_indices[u];
            vocab[vidx].df += 1;
        }

        // allocate per-sentence pairs (vidx,count) as flat array: [vidx,count,vidx,count,...]
        if (idx_list_len > 0) {
            sent_word_counts_len[i] = idx_list_len;
            sent_word_counts[i] = malloc(sizeof(int) * idx_list_len * 2);
            for (int z = 0; z < idx_list_len; ++z) {
                sent_word_counts[i][2*z] = idx_list[z];
                sent_word_counts[i][2*z+1] = count_list[z];
            }
        } else {
            sent_word_counts_len[i] = 0;
            sent_word_counts[i] = NULL;
        }
    }

    /* 3) Compute IDF and sentence scores (TF * IDF sum) */
    double *idf = malloc(sizeof(double) * vcount);
    if (!idf) { fprintf(stderr, "fish_summary: OOM idf\n"); goto cleanup; }
    for (int v = 0; v < vcount; ++v) {
        // smooth idf
        idf[v] = log((double)nsent / (1.0 + (double)vocab[v].df));
    }

    double sent_score[MAX_SENTENCES];
    for (int i = 0; i < nsent; ++i) {
        double score = 0.0;
        int len = sent_word_counts_len[i];
        for (int z = 0; z < len; ++z) {
            int vidx = sent_word_counts[i][2*z];
            int tf = sent_word_counts[i][2*z+1];
            score += (double)tf * idf[vidx];
        }
        sent_score[i] = score;
    }

    /* 4) Choose K sentences based on depth */
    int K;
    if (depth <= 1) K = 1;
    else if (depth == 2) K = 3;
    else if (depth == 3) K = 5;
    else K = 10;
    if (K > nsent) K = nsent;

    /* select top-K by score, then output in original order */
    int selected_idx[MAX_SENTENCES] = {0};
    for (int k = 0; k < K; ++k) {
        int best = -1;
        double best_score = -1e300;
        for (int i = 0; i < nsent; ++i) {
            if (selected_idx[i]) continue;
            if (sent_score[i] > best_score) {
                best_score = sent_score[i];
                best = i;
            }
        }
        if (best >= 0) selected_idx[best] = 1;
    }

    /* collect selected indices in order */
    int out_order[MAX_SENTENCES];
    int out_count = 0;
    for (int i = 0; i < nsent; ++i) if (selected_idx[i]) out_order[out_count++] = i;

    /* print summary */
    printf("=== Extractive Summary (depth=%d) ===\n\n", depth);
    for (int i = 0; i < out_count; ++i) {
        int si = out_order[i];
        printf("%s\n\n", sentences[si]);
    }

    if (time_flag) {
        t1 = clock();
        double elapsed = (double)(t1 - t0) / (double)CLOCKS_PER_SEC;
        printf("[Timing] summary generated in %.4f seconds\n", elapsed);
    }

cleanup:
    /* free memory */
    for (int i = 0; i < nsent; ++i) {
        free(sentences[i]);
        if (sent_word_counts[i]) free(sent_word_counts[i]);
    }
    for (int v = 0; v < vcount; ++v) free(vocab[v].word);
    free(vocab);
    free(idf);
    free(content);
    return 0;
}
