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
 * @brief Get statistics for the dataset.
 * 
 * @param summary Show summary (1: yes, 0: no).
 * @param columns Comma-separated list of columns to include (NULL = all).
 * @param plot Plot statistics (1: yes, 0: no).
 * @return int Status code.
 */
int fish_dataset_stats(int summary, const char *columns, int plot)
{
    const char *dataset_path = "datasets/current.dataset";
    FILE *fp = fopen(dataset_path, "r");
    if (!fp) {
        printf("fish_dataset_stats: no active dataset found.\n");
        return -1;
    }

    char buf[4096];
    int row_count = 0;
    int col_count = 0;
    char *col_names[64]; // support up to 64 columns
    int selected[64] = {0};

    // Read header line for column names
    if (!fgets(buf, sizeof(buf), fp)) {
        fclose(fp);
        printf("fish_dataset_stats: empty dataset.\n");
        return -1;
    }

    buf[strcspn(buf, "\r\n")] = 0;
    char *token = strtok(buf, ",");
    while (token && col_count < 64) {
        col_names[col_count] = strdup(token);
        col_count++;
        token = strtok(NULL, ",");
    }

    // Determine selected columns
    if (columns) {
        char temp[1024];
        strncpy(temp, columns, sizeof(temp)-1);
        temp[sizeof(temp)-1] = 0;
        for (int i = 0; i < col_count; ++i) selected[i] = 0;
        token = strtok(temp, ",");
        while (token) {
            for (int i = 0; i < col_count; ++i) {
                if (strcmp(token, col_names[i]) == 0) selected[i] = 1;
            }
            token = strtok(NULL, ",");
        }
    } else {
        for (int i = 0; i < col_count; ++i) selected[i] = 1;
    }

    // Count rows
    while (fgets(buf, sizeof(buf), fp)) row_count++;

    if (summary) {
        printf("Dataset summary:\n");
        printf("Rows: %d\n", row_count);
        printf("Columns: %d\n", col_count);
        printf("Selected columns:\n");
        for (int i = 0; i < col_count; ++i) {
            if (selected[i]) printf(" - %s\n", col_names[i]);
        }
    }

    if (plot) {
        printf("\nASCII Plot (row count per column selected):\n");
        for (int i = 0; i < col_count; ++i) {
            if (!selected[i]) continue;
            printf("%-15s: ", col_names[i]);
            int bars = row_count / 10;
            if (bars == 0) bars = 1;
            for (int j = 0; j < bars; ++j) putchar('#');
            printf(" (%d)\n", row_count);
        }
    }

    for (int i = 0; i < col_count; ++i) free(col_names[i]);
    fclose(fp);
    return 0;
}
