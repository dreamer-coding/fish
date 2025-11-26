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

#define MAX_LINES 200000
#define MAX_LINE_LEN 4096

/* ---------------- tokenization helpers ---------------- */

static void tokenize_field(const char *in, char *out, size_t outsz) {
    size_t w = 0;
    for (size_t i = 0; in[i] && w + 1 < outsz; i++) {
        if (isalnum((unsigned char)in[i])) {
            out[w++] = (char)tolower(in[i]);
        } else {
            if (w > 0 && out[w-1] != ' ')
                out[w++] = ' ';
        }
    }
    if (w > 0 && out[w-1] == ' ')
        w--;
    out[w] = '\0';
}

/* ---------------- categorical encoding ---------------- */

static unsigned long hash_str(const char *s) {
    unsigned long h = 5381;
    int c;
    while ((c = *s++))
        h = ((h << 5) + h) + c;
    return h;
}

static int encode_category(const char *s) {
    /* simple consistent hash -> 0..999999 */
    return (int)(hash_str(s) % 1000000);
}

/* ---------------- numeric utilities ---------------- */

static int is_numeric(const char *s) {
    char *end;
    strtod(s, &end);
    return end != s;
}

/* ---------------- main preprocess function ---------------- */

int fish_dataset_preprocess(int tokenize, int scale, int encode)
{
    const char *path = "datasets/current.dataset";

    FILE *fp = fopen(path, "r");
    if (!fp) {
        printf("fish_dataset_preprocess: no active dataset.\n");
        return -1;
    }

    char **rows = malloc(sizeof(char*) * MAX_LINES);
    if (!rows) { fclose(fp); return -1; }

    size_t count = 0;
    char buf[MAX_LINE_LEN];

    /* ---------- LOAD ---------- */
    while (fgets(buf, sizeof(buf), fp)) {
        if (count >= MAX_LINES) break;
        rows[count++] = strdup(buf);
    }
    fclose(fp);

    if (count == 0) {
        printf("fish_dataset_preprocess: dataset empty.\n");
        free(rows);
        return 0;
    }

    /* count columns */
    size_t cols = 1;
    for (char *p = rows[0]; *p; p++)
        if (*p == ',') cols++;

    /* storage for scaling */
    double *minv = NULL;
    double *maxv = NULL;

    if (scale) {
        minv = malloc(sizeof(double) * cols);
        maxv = malloc(sizeof(double) * cols);
        for (size_t c = 0; c < cols; c++) {
            minv[c] = 1e300;
            maxv[c] = -1e300;
        }

        /* scan numeric min/max */
        for (size_t i = 0; i < count; i++) {
            char *row = strdup(rows[i]);
            char *tok = strtok(row, ",");
            size_t c = 0;

            while (tok && c < cols) {
                if (is_numeric(tok)) {
                    double v = atof(tok);
                    if (v < minv[c]) minv[c] = v;
                    if (v > maxv[c]) maxv[c] = v;
                }
                tok = strtok(NULL, ",");
                c++;
            }
            free(row);
        }

        printf("fish_dataset_preprocess: numeric scaling enabled.\n");
    }

    /* ---------- PROCESS ---------- */
    for (size_t i = 0; i < count; i++) {
        char out[MAX_LINE_LEN];
        out[0] = '\0';

        char *row = strdup(rows[i]);
        char *tok = strtok(row, ",");
        size_t c = 0;

        while (tok && c < cols) {
            char field[MAX_LINE_LEN];

            strcpy(field, tok);

            /* --- TOKENIZE TEXT --- */
            if (tokenize && !is_numeric(field)) {
                char tokbuf[MAX_LINE_LEN];
                tokenize_field(field, tokbuf, sizeof(tokbuf));
                strcpy(field, tokbuf);
            }

            /* --- ENCODE CATEGORICAL --- */
            if (encode && !is_numeric(field)) {
                int code = encode_category(field);
                char tmp[64];
                snprintf(tmp, sizeof(tmp), "%d", code);
                strcpy(field, tmp);
            }

            /* --- SCALE NUMERIC --- */
            if (scale && is_numeric(field)) {
                double v = atof(field);
                if (maxv[c] > minv[c]) {
                    double scaled = (v - minv[c]) / (maxv[c] - minv[c]);
                    char tmp[64];
                    snprintf(tmp, sizeof(tmp), "%.6f", scaled);
                    strcpy(field, tmp);
                }
            }

            /* write field */
            strcat(out, field);
            if (c + 1 < cols)
                strcat(out, ",");

            tok = strtok(NULL, ",");
            c++;
        }

        free(rows[i]);
        rows[i] = strdup(out);
        free(row);
    }

    if (scale) {
        free(minv);
        free(maxv);
    }

    /* ---------- SAVE ---------- */
    fp = fopen(path, "w");
    if (!fp) {
        printf("fish_dataset_preprocess: failed to write dataset.\n");
        return -1;
    }

    for (size_t i = 0; i < count; i++) {
        fputs(rows[i], fp);
        free(rows[i]);
    }

    free(rows);
    fclose(fp);

    printf("fish_dataset_preprocess: completed successfully.\n");
    return 0;
}
