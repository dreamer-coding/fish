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

/* simple numeric checker */
static int is_numeric(ccstring s) {
    char *end;
    strtod(s, &end);
    return end != s;
}

/* random small noise between -0.05 and +0.05 */
static double rand_noise(void) {
    return ((rand() % 10000) / 10000.0 - 0.5) * 0.1;
}

static void augment_row_hash(const cstring *fields, size_t fc, uint8_t *hash_out) {
    // Concatenate all fields into a single string for hashing
    char buf[4096] = {0};
    size_t pos = 0;
    for (size_t i = 0; i < fc; i++) {
        size_t len = strlen(fields[i]);
        if (pos + len + 1 < sizeof(buf)) {
            memcpy(buf + pos, fields[i], len);
            pos += len;
            if (i < fc - 1) buf[pos++] = ',';
        }
    }
    buf[pos] = '\0';
    fossil_ai_jellyfish_hash(buf, "", hash_out);
}

int fish_dataset_augment(ccstring type, int factor)
{
    if (!type || factor <= 0) {
        fossil_io_printf("{red,bold}fish_dataset_augment: invalid parameters.{normal}\n");
        return -1;
    }

    ccstring path = "datasets/current.dataset";
    fossil_io_file_t dataset_file;
    if (fossil_io_file_open(&dataset_file, path, "r") != 0 || !fossil_io_file_is_open(&dataset_file)) {
        fossil_io_printf("{red,bold}fish_dataset_augment: no active dataset.{normal}\n");
        return -1;
    }

    cstring *rows = (cstring *)fossil_sys_memory_alloc(sizeof(cstring) * 200000);
    if (!rows) {
        fossil_io_file_close(&dataset_file);
        return -1;
    }

    size_t count = 0;
    char buf[4096];

    /* LOAD */
    while (fgets(buf, sizeof(buf), dataset_file.file)) {
        fossil_io_cstring_trim(buf);
        rows[count++] = fossil_io_cstring_dup(buf);
    }
    fossil_io_file_close(&dataset_file);

    if (count == 0) {
        fossil_io_printf("{yellow,bold}fish_dataset_augment: dataset empty.{normal}\n");
        fossil_sys_memory_free(rows);
        return 0;
    }

    /* determine number of columns */
    size_t cols = 1;
    for (char *p = rows[0]; *p; p++)
        if (*p == ',') cols++;

    fossil_io_printf("{green,bold}fish_dataset_augment: type=%s factor=%d columns=%zu{normal}\n",
           type, factor, cols);

    /* prepare output rows list with room for augmented rows */
    cstring *out = (cstring *)fossil_sys_memory_alloc(sizeof(cstring) * (count * (factor + 1)));
    if (!out) {
        fossil_sys_memory_free(rows);
        return -1;
    }

    size_t out_count = 0;

    /* copy originals */
    for (size_t i = 0; i < count; i++) {
        out[out_count++] = fossil_io_cstring_dup(rows[i]);
    }

    /* AUGMENT */
    for (size_t i = 0; i < count; i++) {
        for (int k = 0; k < factor; k++) {
            cstring temp = fossil_io_cstring_dup(rows[i]);
            cstring saveptr = NULL;
            cstring fields[512] = {0};
            size_t fc = 0;
            cstring tok = fossil_io_cstring_token(temp, ",", &saveptr);

            while (tok && fc < cols) {
                fields[fc++] = tok;
                tok = fossil_io_cstring_token(NULL, ",", &saveptr);
            }

            /* noise augmentation */
            if (fossil_io_cstring_iequals(type, "noise")) {
                for (size_t c = 0; c < fc; c++) {
                    if (is_numeric(fields[c])) {
                        double v = atof(fields[c]);
                        v += rand_noise();
                        char tmp[64];
                        snprintf(tmp, sizeof(tmp), "%.6f", v);
                        fossil_io_cstring_free(fields[c]);
                        fields[c] = fossil_io_cstring_dup(tmp);
                    }
                }
            }

            /* flip augmentation */
            else if (fossil_io_cstring_iequals(type, "flip")) {
                for (size_t c = 0; c < fc / 2; c++) {
                    cstring tmp = fields[c];
                    fields[c] = fields[fc - 1 - c];
                    fields[fc - 1 - c] = tmp;
                }
            }

            /* shift augmentation */
            else if (fossil_io_cstring_iequals(type, "shift")) {
                cstring last = fields[fc - 1];
                for (size_t c = fc - 1; c > 0; c--) {
                    fields[c] = fields[c - 1];
                }
                fields[0] = last;
            }

            /* generate hash for augmented row */
            uint8_t hash_out[FOSSIL_JELLYFISH_HASH_SIZE] = {0};
            augment_row_hash(fields, fc, hash_out);

            /* reassemble row */
            cstring line = fossil_io_cstring_join(fields, fc, ',');
            cstring newline = fossil_io_cstring_concat(line, "\n");
            fossil_io_cstring_free(line);

            out[out_count++] = newline;

            for (size_t c = 0; c < fc; c++) {
                if (fields[c]) fossil_io_cstring_free(fields[c]);
            }
            fossil_io_cstring_free(temp);
        }
    }

    /* SAVE */
    if (fossil_io_file_open(&dataset_file, path, "w") != 0 || !fossil_io_file_is_open(&dataset_file)) {
        fossil_io_printf("{red,bold}fish_dataset_augment: write error.{normal}\n");
        fossil_sys_memory_free(out);
        fossil_sys_memory_free(rows);
        return -1;
    }

    for (size_t i = 0; i < out_count; i++) {
        fputs(out[i], dataset_file.file);
        fossil_io_cstring_free(out[i]);
    }

    fossil_io_file_close(&dataset_file);
    fossil_sys_memory_free(out);
    for (size_t i = 0; i < count; i++) fossil_io_cstring_free(rows[i]);
    fossil_sys_memory_free(rows);

    fossil_io_printf("{cyan,bold}fish_dataset_augment: wrote %zu rows.{normal}\n", out_count);
    return 0;
}
