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
    fossil_io_file_t file_stream;

    if (fossil_io_file_open(&file_stream, path, "r") != 0 || !file_stream.file) {
        fossil_io_printf("{red,bold}fish_dataset_clean: no active dataset found.{normal}\n");
        return -1;
    }

    /* storage */
    cstring *rows = (cstring *)fossil_sys_memory_alloc(sizeof(cstring) * MAX_LINES);
    if (!rows) {
        fossil_io_file_close(&file_stream);
        fossil_io_printf("{red,bold}fish_dataset_clean: memory allocation failed.{normal}\n");
        return -1;
    }
    size_t count = 0;

    /* ---------- Load rows ---------- */
    char buf[MAX_LINE_LEN];
    while (fgets(buf, sizeof(buf), file_stream.file)) {
        if (count >= MAX_LINES) break;
        rows[count] = fossil_io_cstring_create(buf);
        count++;
    }
    fossil_io_file_close(&file_stream);

    /* ---------- Drop null rows ---------- */
    if (drop_null) {
        size_t w = 0;
        for (size_t i = 0; i < count; i++) {
            fossil_io_cstring_trim(rows[i]);
            if (fossil_io_cstring_length(rows[i]) > 0)
                rows[w++] = rows[i];
            else
                fossil_io_cstring_free(rows[i]);
        }
        count = w;
        fossil_io_printf("{green}fish_dataset_clean: dropped null rows.{normal}\n");
    }

    /* ---------- Deduplicate ---------- */
    if (dedup) {
        // Use jellyfish hash for deduplication
        uint8_t *seen_hashes = (uint8_t *)fossil_sys_memory_calloc(count, FOSSIL_JELLYFISH_HASH_SIZE);
        size_t seen_count = 0;

        size_t w = 0;
        for (size_t i = 0; i < count; i++) {
            uint8_t hash[FOSSIL_JELLYFISH_HASH_SIZE];
            fossil_ai_jellyfish_hash(rows[i], NULL, hash);

            int exists = 0;
            for (size_t s = 0; s < seen_count; s++) {
                if (fossil_sys_memory_compare(seen_hashes + s * FOSSIL_JELLYFISH_HASH_SIZE, hash, FOSSIL_JELLYFISH_HASH_SIZE) == 0) {
                    exists = 1;
                    break;
                }
            }
            if (!exists) {
                fossil_sys_memory_copy(seen_hashes + seen_count * FOSSIL_JELLYFISH_HASH_SIZE, hash, FOSSIL_JELLYFISH_HASH_SIZE);
                seen_count++;
                rows[w++] = rows[i];
            } else {
                fossil_io_cstring_free(rows[i]);
            }
        }
        count = w;
        fossil_sys_memory_free(seen_hashes);

        fossil_io_printf("{yellow}fish_dataset_clean: removed duplicates.{normal}\n");
    }

    /* ---------- Normalize numeric columns ---------- */
    if (normalize && count > 0) {
        /* determine number of columns */
        size_t cols = 1;
        for (char *p = rows[0]; *p; p++)
            if (*p == ',') cols++;

        double *minv = (double *)fossil_sys_memory_alloc(sizeof(double) * cols);
        double *maxv = (double *)fossil_sys_memory_alloc(sizeof(double) * cols);
        if (!minv || !maxv) {
            fossil_io_printf("{red,bold}fish_dataset_clean: normalize failed (alloc).{normal}\n");
            fossil_sys_memory_free(minv); fossil_sys_memory_free(maxv);
            return -1;
        }

        /* init */
        for (size_t c = 0; c < cols; c++) {
            minv[c] = 1e300;
            maxv[c] = -1e300;
        }

        /* scan numeric ranges */
        for (size_t i = 0; i < count; i++) {
            cstring row = fossil_io_cstring_copy(rows[i]);
            cstring saveptr = NULL;
            cstring tok = fossil_io_cstring_token(row, ",", &saveptr);
            size_t c = 0;
            while (tok && c < cols) {
                char *end;
                double v = strtod(tok, &end);
                if (end != tok) { /* numeric */
                    if (v < minv[c]) minv[c] = v;
                    if (v > maxv[c]) maxv[c] = v;
                }
                tok = fossil_io_cstring_token(NULL, ",", &saveptr);
                c++;
            }
            fossil_io_cstring_free(row);
        }

        /* apply normalization */
        for (size_t i = 0; i < count; i++) {
            cstring out = fossil_io_cstring_create("");
            cstring row = fossil_io_cstring_copy(rows[i]);
            cstring saveptr = NULL;
            cstring tok = fossil_io_cstring_token(row, ",", &saveptr);
            size_t c = 0;

            while (tok && c < cols) {
                char *end;
                double v = strtod(tok, &end);
                if (end != tok && maxv[c] > minv[c]) {
                    /* numeric -> normalize */
                    double scaled = (v - minv[c]) / (maxv[c] - minv[c]);
                    cstring num = fossil_io_cstring_format("%.6f", scaled);
                    fossil_io_cstring_append(&out, num);
                    fossil_io_cstring_free(num);
                } else {
                    fossil_io_cstring_append(&out, tok);
                }
                if (c + 1 < cols)
                    fossil_io_cstring_append(&out, ",");
                tok = fossil_io_cstring_token(NULL, ",", &saveptr);
                c++;
            }

            fossil_io_cstring_free(rows[i]);
            rows[i] = out;
            fossil_io_cstring_free(row);
        }

        fossil_sys_memory_free(minv);
        fossil_sys_memory_free(maxv);

        fossil_io_printf("{cyan}fish_dataset_clean: normalized numeric values.{normal}\n");
    }

    /* ---------- Write output back ---------- */
    if (fossil_io_file_open(&file_stream, path, "w") != 0 || !file_stream.file) {
        fossil_io_printf("{red,bold}fish_dataset_clean: failed to rewrite dataset.{normal}\n");
        return -1;
    }

    for (size_t i = 0; i < count; i++) {
        fossil_io_fputs(file_stream.file, rows[i]);
        fossil_io_cstring_free(rows[i]);
    }
    fossil_io_file_close(&file_stream);

    fossil_sys_memory_free(rows);

    fossil_io_printf("{green,bold}fish_dataset_clean: completed successfully.{normal}\n");
    return 0;
}
