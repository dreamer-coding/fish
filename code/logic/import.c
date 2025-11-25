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
 * @brief Import a dataset from a file.
 *
 * This function validates the file and format, then copies the dataset
 * into the local "datasets/" directory under a canonical name derived
 * from the input filename. No parsing is performed.
 */
int fish_dataset_import(const char *file_path, const char *format)
{
    if (!file_path || !format) {
        printf("fish_dataset_import: invalid arguments.\n");
        return -1;
    }

    /* Validate format */
    if (strcmp(format, "csv") != 0 &&
        strcmp(format, "json") != 0) {
        printf("fish_dataset_import: unsupported format '%s'.\n", format);
        return -1;
    }

    /* Open source file */
    FILE *src = fopen(file_path, "rb");
    if (!src) {
        printf("fish_dataset_import: cannot open input file '%s'.\n",
               file_path);
        return -1;
    }

    /* Extract base filename */
    const char *base = strrchr(file_path, '/');
    base = base ? base + 1 : file_path;

    /* Build output path */
    char out_path[512];
    snprintf(out_path, sizeof(out_path),
             "datasets/%s", base);

    /* Ensure datasets directory exists (best-effort) */
#if defined(_WIN32)
    _mkdir("datasets");
#else
    mkdir("datasets", 0755);
#endif

    /* Open destination file */
    FILE *dst = fopen(out_path, "wb");
    if (!dst) {
        fclose(src);
        printf("fish_dataset_import: cannot write '%s'.\n", out_path);
        return -1;
    }

    /* Copy contents */
    char buffer[4096];
    size_t n;
    while ((n = fread(buffer, 1, sizeof(buffer), src)) > 0) {
        fwrite(buffer, 1, n, dst);
    }

    fclose(src);
    fclose(dst);

    printf("fish_dataset_import: imported '%s' as '%s' (%s format).\n",
           file_path, out_path, format);

    return 0;
}
