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
 * @brief Export the dataset to a file.
 * 
 * @param file_path Path to export file.
 * @param format Export format: "csv", "json", "fson".
 * @return int Status code.
 */
int fish_dataset_export(const char *file_path, const char *format)
{
    if (!file_path || !format) {
        printf("fish_dataset_export: invalid parameters.\n");
        return -1;
    }

    const char *src_path = "datasets/current.dataset";
    FILE *src = fopen(src_path, "r");
    if (!src) {
        printf("fish_dataset_export: no active dataset found.\n");
        return -1;
    }

    FILE *dst = fopen(file_path, "w+b");
    if (!dst) {
        fclose(src);
        printf("fish_dataset_export: cannot open output file.\n");
        return -1;
    }

    char buf[4096];

    if (strcmp(format, "csv") == 0) {
        // simple copy for CSV
        while (fgets(buf, sizeof(buf), src)) {
            fputs(buf, dst);
        }
    }
    else if (strcmp(format, "json") == 0) {
        fprintf(dst, "[\n");
        int first = 1;
        while (fgets(buf, sizeof(buf), src)) {
            if (!first) fprintf(dst, ",\n");
            first = 0;
            buf[strcspn(buf, "\r\n")] = 0; // strip newline
            fprintf(dst, "  [\"%s\"]", buf); // simple JSON array per row
        }
        fprintf(dst, "\n]\n");
    }
    else if (strcmp(format, "fson") == 0) {
        // minimal binary dump: length-prefixed lines
        while (fgets(buf, sizeof(buf), src)) {
            size_t len = strcspn(buf, "\r\n");
            if (fwrite(&len, sizeof(size_t), 1, dst) != 1) break;
            if (fwrite(buf, 1, len, dst) != len) break;
        }
    }
    else {
        fclose(src);
        fclose(dst);
        printf("fish_dataset_export: unsupported format '%s'.\n", format);
        return -1;
    }

    fclose(src);
    fclose(dst);
    printf("fish_dataset_export: dataset exported to '%s' as %s.\n", file_path, format);
    return 0;
}
