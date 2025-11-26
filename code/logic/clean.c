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

/* ----------- simple hash for deduplication ----------- */
static unsigned long str_hash(const char *s) {
    unsigned long h = 5381;
    int c;
    while ((c = *s++))
        h = ((h << 5) + h) + c;
    return h;
}

#define MAX_LINES 200000
#define MAX_LINE_LEN 4096

/**
 * @brief Clean the dataset (drop nulls, deduplicate, normalize).
 *
 * The dataset is expected to be located at:
 *     datasets/current.dataset
 *
 * Null rows = rows that are empty or whitespace.
 * Deduplication = remove exact duplicate rows.
 * Normalize = scale numeric values in each column to 0–1 range.
 *
 * @return int Status code.
 */
int fish_dataset_clean(int drop_null, int dedup, int normalize)
{
    const char *path = "datasets/current.dataset";

    FILE *fp = fopen(path, "r");
    if (!fp) {
        printf("fish_dataset_clean: no active dataset found.\n");
        return -1;
    }

    /* storage */
    char **rows = malloc(sizeof(char*) * MAX_LINES);
    if (!rows) {
        fclose(fp);
        return -1;
    }
    size_t count = 0;

    /* ---------- Load rows ---------- */
    char buf[MAX_LINE_LEN];
    while (fgets(buf, sizeof(buf), fp)) {
        if (count >= MAX_LINES) break;
        rows[count] = strdup(buf);
        count++;
    }
    fclose(fp);

    /* ---------- Drop null rows ---------- */
    if (drop_null) {
        size_t w = 0;
        for (size_t i = 0; i < count; i++) {
            int allspace = 1;
            for (char *p = rows[i]; *p; p++) {
                if (!isspace((unsigned char)*p)) {
                    allspace = 0;
                    break;
                }
            }
            if (!allspace)
                rows[w++] = rows[i];
            else
                free(rows[i]);
        }
        count = w;
        printf("fish_dataset_clean: dropped null rows.\n");
    }

    /* ---------- Deduplicate ---------- */
    if (dedup) {
        unsigned long *seen = calloc(count, sizeof(unsigned long));
        size_t seen_count = 0;

        size_t w = 0;
        for (size_t i = 0; i < count; i++) {
            unsigned long h = str_hash(rows[i]);
            int exists = 0;
            for (size_t s = 0; s < seen_count; s++) {
                if (seen[s] == h) {
                    exists = 1;
                    break;
                }
            }
            if (!exists) {
                seen[seen_count++] = h;
                rows[w++] = rows[i];
            } else {
                free(rows[i]);
            }
        }
        count = w;
        free(seen);

        printf("fish_dataset_clean: removed duplicates.\n");
    }

    /* ---------- Normalize numeric columns ---------- */
    if (normalize && count > 0) {
        /* determine number of columns */
        size_t cols = 1;
        for (char *p = rows[0]; *p; p++)
            if (*p == ',') cols++;

        double *minv = malloc(sizeof(double) * cols);
        double *maxv = malloc(sizeof(double) * cols);
        if (!minv || !maxv) {
            printf("fish_dataset_clean: normalize failed (alloc).\n");
            free(minv); free(maxv);
            return -1;
        }

        /* init */
        for (size_t c = 0; c < cols; c++) {
            minv[c] = 1e300;
            maxv[c] = -1e300;
        }

        /* scan numeric ranges */
        for (size_t i = 0; i < count; i++) {
            char *row = strdup(rows[i]);
            char *tok = strtok(row, ",");
            size_t c = 0;
            while (tok && c < cols) {
                char *end;
                double v = strtod(tok, &end);
                if (end != tok) { /* numeric */
                    if (v < minv[c]) minv[c] = v;
                    if (v > maxv[c]) maxv[c] = v;
                }
                tok = strtok(NULL, ",");
                c++;
            }
            free(row);
        }

        /* apply normalization */
        for (size_t i = 0; i < count; i++) {
            char out[MAX_LINE_LEN];
            out[0] = '\0';

            char *row = strdup(rows[i]);
            char *tok = strtok(row, ",");
            size_t c = 0;

            while (tok && c < cols) {
                char *end;
                double v = strtod(tok, &end);
                if (end != tok && maxv[c] > minv[c]) {
                    /* numeric -> normalize */
                    double scaled = (v - minv[c]) / (maxv[c] - minv[c]);
                    char num[64];
                    snprintf(num, sizeof(num), "%.6f", scaled);
                    strcat(out, num);
                } else {
                    /* non-numeric */
                    strcat(out, tok);
                }
                if (c + 1 < cols) strcat(out, ",");
                tok = strtok(NULL, ",");
                c++;
            }

            free(rows[i]);
            rows[i] = strdup(out);
            free(row);
        }

        free(minv);
        free(maxv);

        printf("fish_dataset_clean: normalized numeric values.\n");
    }

    /* ---------- Write output back ---------- */
    fp = fopen(path, "w");
    if (!fp) {
        printf("fish_dataset_clean: failed to rewrite dataset.\n");
        return -1;
    }

    for (size_t i = 0; i < count; i++) {
        fputs(rows[i], fp);
        free(rows[i]);
    }
    fclose(fp);

    free(rows);

    printf("fish_dataset_clean: completed successfully.\n");
    return 0;
}
