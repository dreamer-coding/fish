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
        printf("fish_dataset_split: Fractions must sum to 1.0\n");
        return -1;
    }

    const char *dataset_path = "datasets/current.dataset";
    FILE *fp = fopen(dataset_path, "r");
    if (!fp) {
        printf("fish_dataset_split: No active dataset found.\n");
        return -1;
    }

    // Open output files
    FILE *ftrain = fopen("datasets/train.dataset", "w");
    FILE *fval   = fopen("datasets/val.dataset", "w");
    FILE *ftest  = fopen("datasets/test.dataset", "w");
    if (!ftrain || !fval || !ftest) {
        fclose(fp);
        if (ftrain) fclose(ftrain);
        if (fval) fclose(fval);
        if (ftest) fclose(ftest);
        printf("fish_dataset_split: Failed to create output files.\n");
        return -1;
    }

    // Read header
    char header[4096];
    if (!fgets(header, sizeof(header), fp)) {
        fclose(fp); fclose(ftrain); fclose(fval); fclose(ftest);
        printf("fish_dataset_split: Dataset empty.\n");
        return -1;
    }
    fputs(header, ftrain);
    fputs(header, fval);
    fputs(header, ftest);

    // Count lines (rows)
    int row_count = 0;
    char line[4096];
    while (fgets(line, sizeof(line), fp)) row_count++;

    if (row_count == 0) {
        fclose(fp); fclose(ftrain); fclose(fval); fclose(ftest);
        printf("fish_dataset_split: Dataset has no rows.\n");
        return -1;
    }

    // Rewind to data lines
    fseek(fp, 0, SEEK_SET);
    fgets(header, sizeof(header), fp); // skip header

    // Allocate array to shuffle rows
    char **rows = (char **)malloc(sizeof(char *) * row_count);
    if (!rows) {
        fclose(fp); fclose(ftrain); fclose(fval); fclose(ftest);
        printf("fish_dataset_split: Memory allocation failed.\n");
        return -1;
    }

    int idx = 0;
    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\r\n")] = 0;
        rows[idx] = strdup(line);
        if (!rows[idx]) {
            printf("fish_dataset_split: Memory allocation failed for row.\n");
            for (int j=0; j<idx; ++j) free(rows[j]);
            free(rows);
            fclose(fp); fclose(ftrain); fclose(fval); fclose(ftest);
            return -1;
        }
        idx++;
    }
    fclose(fp);

    // Shuffle rows
    srand((unsigned int)time(NULL));
    for (int i = row_count - 1; i > 0; --i) {
        int j = rand() % (i + 1);
        char *tmp = rows[i];
        rows[i] = rows[j];
        rows[j] = tmp;
    }

    int train_end = (int)(train_frac * row_count);
    int val_end   = train_end + (int)(val_frac * row_count);

    // Write rows to files
    for (int i = 0; i < row_count; ++i) {
        FILE *out = (i < train_end) ? ftrain :
                    (i < val_end)   ? fval :
                                     ftest;
        fprintf(out, "%s\n", rows[i]);
        free(rows[i]);
    }
    free(rows);

    fclose(ftrain);
    fclose(fval);
    fclose(ftest);

    printf("Dataset split completed: %d train, %d val, %d test rows\n",
           train_end, val_end - train_end, row_count - val_end);

    return 0;
}
