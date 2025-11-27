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
 * @brief Get statistics for the dataset and optionally compute a fingerprint hash using Jellyfish.
 * 
 * @param summary Show summary (1: yes, 0: no).
 * @param columns Comma-separated list of columns to include (NULL = all).
 * @param plot Plot statistics (1: yes, 0: no).
 * @return int Status code.
 */
int fish_dataset_stats(int summary, const char *columns, int plot)
{
    ccstring dataset_path = "datasets/current.dataset";
    fossil_io_file_t dataset_stream;
    if (fossil_io_file_open(&dataset_stream, dataset_path, "r") != 0 || !fossil_io_file_is_open(&dataset_stream)) {
        fossil_io_printf("{red,bold}fish_dataset_stats: no active dataset found.{normal}\n");
        return -1;
    }

    // Use memory API for buffers and arrays
    char *buf = (char *)fossil_sys_memory_alloc(4096);
    int row_count = 0;
    int col_count = 0;
    cstring *col_names = (cstring *)fossil_sys_memory_calloc(64, sizeof(cstring)); // support up to 64 columns
    int *selected = (int *)fossil_sys_memory_calloc(64, sizeof(int));

    // Read header line for column names
    if (!fgets(buf, 4096, dataset_stream.file)) {
        fossil_sys_memory_free(buf);
        fossil_sys_memory_free(col_names);
        fossil_sys_memory_free(selected);
        fossil_io_file_close(&dataset_stream);
        fossil_io_printf("{red,bold}fish_dataset_stats: empty dataset.{normal}\n");
        return -1;
    }

    buf[strcspn(buf, "\r\n")] = 0;
    cstring saveptr = NULL;
    cstring token = fossil_io_cstring_token(buf, ",", &saveptr);
    while (token && col_count < 64) {
        col_names[col_count] = fossil_io_cstring_create(token);
        col_count++;
        token = fossil_io_cstring_token(NULL, ",", &saveptr);
    }

    // Determine selected columns
    if (columns) {
        char *temp = (char *)fossil_sys_memory_alloc(1024);
        strncpy(temp, columns, 1023);
        temp[1023] = 0;
        fossil_sys_memory_zero(selected, sizeof(int) * 64);
        cstring saveptr2 = NULL;
        token = fossil_io_cstring_token(temp, ",", &saveptr2);
        while (token) {
            for (int i = 0; i < col_count; ++i) {
                if (fossil_io_cstring_iequals(token, col_names[i], 256)) selected[i] = 1;
            }
            token = fossil_io_cstring_token(NULL, ",", &saveptr2);
        }
        fossil_sys_memory_free(temp);
    } else {
        for (int i = 0; i < col_count; ++i) selected[i] = 1;
    }

    // Count rows
    while (fgets(buf, 4096, dataset_stream.file)) row_count++;

    if (summary) {
        fossil_io_printf("{green,bold}Dataset summary:{normal}\n");
        fossil_io_printf("{yellow}Rows: {normal}%d\n", row_count);
        fossil_io_printf("{yellow}Columns: {normal}%d\n", col_count);
        fossil_io_printf("{cyan}Selected columns:{normal}\n");
        for (int i = 0; i < col_count; ++i) {
            if (selected[i]) fossil_io_printf(" - {magenta}%s{normal}\n", col_names[i]);
        }
    }

    if (plot) {
        fossil_io_printf("\n{blue,bold}ASCII Plot (row count per column selected):{normal}\n");
        for (int i = 0; i < col_count; ++i) {
            if (!selected[i]) continue;
            fossil_io_printf("{magenta}%-15s{normal}: ", col_names[i]);
            int bars = row_count / 10;
            if (bars == 0) bars = 1;
            for (int j = 0; j < bars; ++j) fossil_io_putchar('#');
            fossil_io_printf(" (%d)\n", row_count);
        }
    }

    // Example: Compute a fingerprint hash of the dataset header using Jellyfish
    uint8_t hash_out[FOSSIL_JELLYFISH_HASH_SIZE];
    fossil_ai_jellyfish_hash(buf, NULL, hash_out);
    fossil_io_printf("{blue}Dataset header hash (Jellyfish):{normal} ");
    for (size_t i = 0; i < FOSSIL_JELLYFISH_HASH_SIZE; ++i)
        fossil_io_printf("%02x", hash_out[i]);
    fossil_io_printf("\n");

    for (int i = 0; i < col_count; ++i) fossil_io_cstring_free(col_names[i]);
    fossil_sys_memory_free(col_names);
    fossil_sys_memory_free(selected);
    fossil_sys_memory_free(buf);
    fossil_io_file_close(&dataset_stream);
    return 0;
}
