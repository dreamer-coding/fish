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

/**
 * @brief Split the dataset into train, validation, and test sets.
 * 
 * @param train_frac Fraction for training set.
 * @param val_frac Fraction for validation set.
 * @param test_frac Fraction for test set.
 * @return int Status code.
 */
int fish_dataset_split(float train_frac, float val_frac, float test_frac)
{
    if (train_frac < 0 || val_frac < 0 || test_frac < 0 ||
        train_frac + val_frac + test_frac != 1.0f) {
        fossil_io_printf("{red,bold}fish_dataset_split: Fractions must sum to 1.0{normal}\n");
        return -1;
    }

    ccstring dataset_path = "datasets/current.dataset";
    fossil_io_file_t fin, ftrain, fval, ftest;

    if (fossil_io_file_open(&fin, dataset_path, "r") != 0) {
        fossil_io_printf("{red,bold}fish_dataset_split: No active dataset found.{normal}\n");
        return -1;
    }

    if (fossil_io_file_open(&ftrain, "datasets/train.dataset", "w") != 0 ||
        fossil_io_file_open(&fval,   "datasets/val.dataset",   "w") != 0 ||
        fossil_io_file_open(&ftest,  "datasets/test.dataset",  "w") != 0) {
        fossil_io_file_close(&fin);
        if (fossil_io_file_is_open(&ftrain)) fossil_io_file_close(&ftrain);
        if (fossil_io_file_is_open(&fval))   fossil_io_file_close(&fval);
        if (fossil_io_file_is_open(&ftest))  fossil_io_file_close(&ftest);
        fossil_io_printf("{red,bold}fish_dataset_split: Failed to create output files.{normal}\n");
        return -1;
    }

    // Read header
    char header[4096];
    if (!fgets(header, sizeof(header), fin.file)) {
        fossil_io_file_close(&fin); fossil_io_file_close(&ftrain);
        fossil_io_file_close(&fval); fossil_io_file_close(&ftest);
        fossil_io_printf("{red,bold}fish_dataset_split: Dataset empty.{normal}\n");
        return -1;
    }
    fossil_io_fputs(&ftrain, header);
    fossil_io_fputs(&fval, header);
    fossil_io_fputs(&ftest, header);

    // Count lines (rows)
    int row_count = 0;
    char line[4096];
    while (fgets(line, sizeof(line), fin.file)) row_count++;

    if (row_count == 0) {
        fossil_io_file_close(&fin); fossil_io_file_close(&ftrain);
        fossil_io_file_close(&fval); fossil_io_file_close(&ftest);
        fossil_io_printf("{red,bold}fish_dataset_split: Dataset has no rows.{normal}\n");
        return -1;
    }

    // Rewind to data lines
    fossil_io_file_rewind(&fin);
    fgets(header, sizeof(header), fin.file); // skip header

    // Allocate array to shuffle rows using fossil_sys_memory_alloc
    cstring *rows = (cstring *)fossil_sys_memory_alloc(sizeof(cstring) * row_count);
    if (!rows) {
        fossil_io_file_close(&fin); fossil_io_file_close(&ftrain);
        fossil_io_file_close(&fval); fossil_io_file_close(&ftest);
        fossil_io_printf("{red,bold}fish_dataset_split: Memory allocation failed.{normal}\n");
        return -1;
    }

    int idx = 0;
    while (fgets(line, sizeof(line), fin.file)) {
        line[strcspn(line, "\r\n")] = 0;
        rows[idx] = fossil_io_cstring_create(line);
        if (!rows[idx]) {
            fossil_io_printf("{red,bold}fish_dataset_split: Memory allocation failed for row.{normal}\n");
            for (int j=0; j<idx; ++j) fossil_io_cstring_free(rows[j]);
            fossil_sys_memory_free(rows);
            fossil_io_file_close(&fin); fossil_io_file_close(&ftrain);
            fossil_io_file_close(&fval); fossil_io_file_close(&ftest);
            return -1;
        }
        idx++;
    }
    fossil_io_file_close(&fin);

    // Shuffle rows
    srand((unsigned int)time(NULL));
    for (int i = row_count - 1; i > 0; --i) {
        int j = rand() % (i + 1);
        fossil_sys_memory_swap(&rows[i], &rows[j], sizeof(cstring));
    }

    int train_end = (int)(train_frac * row_count);
    int val_end   = train_end + (int)(val_frac * row_count);

    // Write rows to files and hash each row for fingerprinting
    uint8_t hash_buf[32];
    for (int i = 0; i < row_count; ++i) {
        fossil_io_file_t *out = (i < train_end) ? &ftrain :
                                (i < val_end)   ? &fval :
                                                  &ftest;
        fossil_io_fprintf(out, "%s\n", rows[i]);
        fossil_ai_jellyfish_hash(rows[i], "", hash_buf);
        fossil_io_cstring_free(rows[i]);
    }
    fossil_sys_memory_free(rows);

    fossil_io_file_close(&ftrain);
    fossil_io_file_close(&fval);
    fossil_io_file_close(&ftest);

    fossil_io_printf(
        "{green,bold}Dataset split completed:{normal} "
        "{yellow}%d train{normal}, {cyan}%d val{normal}, {magenta}%d test rows{normal}\n",
        train_end, val_end - train_end, row_count - val_end
    );

    return 0;
}
