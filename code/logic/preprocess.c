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
    cstring tmp = fossil_io_cstring_create(in);
    cstring lower = fossil_io_cstring_to_lower(tmp);
    size_t w = 0;
    for (size_t i = 0; lower[i] && w + 1 < outsz; i++) {
        if (isalnum((unsigned char)lower[i])) {
            out[w++] = lower[i];
        } else {
            if (w > 0 && out[w-1] != ' ')
                out[w++] = ' ';
        }
    }
    if (w > 0 && out[w-1] == ' ')
        w--;
    out[w] = '\0';
    fossil_io_cstring_free(lower);
    fossil_io_cstring_free(tmp);
}

/* ---------------- categorical encoding ---------------- */

#define CATEGORY_HASH_SIZE FOSSIL_JELLYFISH_HASH_SIZE

static int encode_category(const char *s) {
    uint8_t hash[CATEGORY_HASH_SIZE];
    /* Use input as both input and output for deterministic hash */
    fossil_ai_jellyfish_hash(s, s, hash);
    /* Convert first 4 bytes of hash to int, mod 1000000 for range */
    unsigned int code = ((unsigned int)hash[0] << 24) |
                        ((unsigned int)hash[1] << 16) |
                        ((unsigned int)hash[2] << 8)  |
                        ((unsigned int)hash[3]);
    return (int)(code % 1000000);
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
    fossil_io_file_t file_stream;

    if (fossil_io_file_open(&file_stream, path, "r") != 0 || !fossil_io_file_is_open(&file_stream)) {
        fossil_io_printf("{red,bold}fish_dataset_preprocess: no active dataset.{normal}\n");
        return -1;
    }

    cstring *rows = (cstring *)fossil_sys_memory_alloc(sizeof(cstring) * MAX_LINES);
    if (!rows) { fossil_io_file_close(&file_stream); return -1; }

    size_t count = 0;
    char buf[MAX_LINE_LEN];

    /* ---------- LOAD ---------- */
    while (fgets(buf, sizeof(buf), file_stream.file)) {
        if (count >= MAX_LINES) break;
        rows[count++] = fossil_io_cstring_create(buf);
    }
    fossil_io_file_close(&file_stream);

    if (count == 0) {
        fossil_io_printf("{yellow,bold}fish_dataset_preprocess: dataset empty.{normal}\n");
        fossil_sys_memory_free(rows);
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
        minv = (double *)fossil_sys_memory_alloc(sizeof(double) * cols);
        maxv = (double *)fossil_sys_memory_alloc(sizeof(double) * cols);
        for (size_t c = 0; c < cols; c++) {
            minv[c] = 1e300;
            maxv[c] = -1e300;
        }

        /* scan numeric min/max */
        for (size_t i = 0; i < count; i++) {
            cstring row = fossil_io_cstring_create(rows[i]);
            cstring saveptr = NULL;
            cstring tok = fossil_io_cstring_token(row, ",", &saveptr);
            size_t c = 0;

            while (tok && c < cols) {
                if (is_numeric(tok)) {
                    double v = atof(tok);
                    if (v < minv[c]) minv[c] = v;
                    if (v > maxv[c]) maxv[c] = v;
                }
                tok = fossil_io_cstring_token(NULL, ",", &saveptr);
                c++;
            }
            fossil_io_cstring_free(row);
        }

        fossil_io_printf("{green,bold}fish_dataset_preprocess: numeric scaling enabled.{normal}\n");
    }

    /* ---------- PROCESS ---------- */
    for (size_t i = 0; i < count; i++) {
        char out[MAX_LINE_LEN];
        out[0] = '\0';

        cstring row = fossil_io_cstring_create(rows[i]);
        cstring saveptr = NULL;
        cstring tok = fossil_io_cstring_token(row, ",", &saveptr);
        size_t c = 0;

        while (tok && c < cols) {
            char field[MAX_LINE_LEN];
            strncpy(field, tok, MAX_LINE_LEN - 1);
            field[MAX_LINE_LEN - 1] = '\0';

            /* --- TOKENIZE TEXT --- */
            if (tokenize && !is_numeric(field)) {
                char tokbuf[MAX_LINE_LEN];
                tokenize_field(field, tokbuf, sizeof(tokbuf));
                strncpy(field, tokbuf, MAX_LINE_LEN - 1);
                field[MAX_LINE_LEN - 1] = '\0';
            }

            /* --- ENCODE CATEGORICAL --- */
            if (encode && !is_numeric(field)) {
                int code = encode_category(field);
                snprintf(field, sizeof(field), "%d", code);
            }

            /* --- SCALE NUMERIC --- */
            if (scale && is_numeric(field)) {
                double v = atof(field);
                if (maxv[c] > minv[c]) {
                    double scaled = (v - minv[c]) / (maxv[c] - minv[c]);
                    snprintf(field, sizeof(field), "%.6f", scaled);
                }
            }

            /* write field */
            strncat(out, field, MAX_LINE_LEN - strlen(out) - 1);
            if (c + 1 < cols)
                strncat(out, ",", MAX_LINE_LEN - strlen(out) - 1);

            tok = fossil_io_cstring_token(NULL, ",", &saveptr);
            c++;
        }

        fossil_io_cstring_free(rows[i]);
        rows[i] = fossil_io_cstring_create(out);
        fossil_io_cstring_free(row);
    }

    if (scale) {
        fossil_sys_memory_free(minv);
        fossil_sys_memory_free(maxv);
    }

    /* ---------- SAVE ---------- */
    if (fossil_io_file_open(&file_stream, path, "w") != 0 || !fossil_io_file_is_open(&file_stream)) {
        fossil_io_printf("{red,bold}fish_dataset_preprocess: failed to write dataset.{normal}\n");
        return -1;
    }

    for (size_t i = 0; i < count; i++) {
        fputs(rows[i], file_stream.file);
        fossil_io_cstring_free(rows[i]);
    }

    fossil_sys_memory_free(rows);
    fossil_io_file_close(&file_stream);

    fossil_io_printf("{cyan,bold}fish_dataset_preprocess: completed successfully.{normal}\n");
    return 0;
}
