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

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

/**
 * @brief Import a dataset from a file.
 *
 * This function validates the file and format, then copies the dataset
 * into the local "datasets/" directory under a canonical name derived
 * from the input filename. No parsing is performed.
 */
int fish_dataset_import(const char *file_path, const char *format)
{
    if (!file_path || !format) {
        fossil_io_printf("{red,bold}fish_dataset_import: invalid arguments.{normal}\n");
        return -1;
    }

    /* Validate format (case-insensitive) */
    if (!fossil_io_cstring_iequals_safe(format, "csv", 16) &&
        !fossil_io_cstring_iequals_safe(format, "json", 16)) {
        fossil_io_printf("{red,bold}fish_dataset_import: unsupported format '{yellow}%s{red}'.{normal}\n", format);
        return -1;
    }

    /* Extract base filename using safe substring */
    const char *base = strrchr(file_path, '/');
    base = base ? base + 1 : file_path;

    /* Build output path using Fossil IO cstring */
    cstring out_path = fossil_io_cstring_format_safe(512, "datasets/%s", base);

    /* Ensure datasets directory exists (best-effort) */
#if defined(_WIN32)
    _mkdir("datasets");
#else
    mkdir("datasets", 0755);
#endif

    fossil_io_file_t src_stream = {0};
    fossil_io_file_t dst_stream = {0};

    /* Open source file */
    if (fossil_io_file_open(&src_stream, file_path, "rb") != 0) {
        fossil_io_printf("{red,bold}fish_dataset_import: cannot open input file '{yellow}%s{red}'.{normal}\n", file_path);
        fossil_io_cstring_free(out_path);
        return -1;
    }

    /* Open destination file */
    if (fossil_io_file_open(&dst_stream, out_path, "wb") != 0) {
        fossil_io_file_close(&src_stream);
        fossil_io_printf("{red,bold}fish_dataset_import: cannot write '{yellow}%s{red}'.{normal}\n", out_path);
        fossil_io_cstring_free(out_path);
        return -1;
    }

    /* Copy contents using fossil_sys_memory_t and memory API */
    size_t buffer_size = 4096;
    fossil_sys_memory_t buffer = fossil_sys_memory_alloc(buffer_size);
    size_t n;
    while ((n = fossil_io_file_read(&src_stream, buffer, 1, buffer_size)) > 0) {
        fossil_io_file_write(&dst_stream, buffer, 1, n);
    }
    fossil_sys_memory_free(buffer);

    fossil_io_file_close(&src_stream);
    fossil_io_file_close(&dst_stream);

    /* Generate hash for imported file (content addressing) */
    uint8_t hash_out[FOSSIL_JELLYFISH_HASH_SIZE] = {0};
    fossil_ai_jellyfish_hash(file_path, out_path, hash_out);

    /* Print hash as hex string for diagnostics */
    char hash_hex[2 * FOSSIL_JELLYFISH_HASH_SIZE + 1] = {0};
    for (size_t i = 0; i < FOSSIL_JELLYFISH_HASH_SIZE; ++i)
        sprintf(hash_hex + 2 * i, "%02x", hash_out[i]);

    fossil_io_printf("{green,bold}fish_dataset_import: imported '{yellow}%s{green}' as '{yellow}%s{green}' ({cyan}%s{green} format).{normal}\n",
           file_path, out_path, format);
    fossil_io_printf("{blue}Content hash: {yellow}%s{normal}\n", hash_hex);

    fossil_io_cstring_free(out_path);
    return 0;
}
