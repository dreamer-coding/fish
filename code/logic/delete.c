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
#include <stdlib.h>

/**
 * @brief Delete a model.
 *
 * A Jellyfish AI model is represented by "<model_name>.jfchain".
 * This function removes that file after confirming with the user
 * unless force = 1.
 */
int fish_delete_model(const char *model_name, int force)
{
    if (!model_name || model_name[0] == '\0') {
        printf("fish_delete_model: missing model name.\n");
        return -1;
    }

    char path[512];
    snprintf(path, sizeof(path), "%s.jfchain", model_name);

    // Check file existence first
    FILE *fp = fopen(path, "rb");
    if (!fp) {
        printf("fish_delete_model: model not found: %s\n", path);
        return -1;
    }
    fclose(fp);

    // Ask for confirmation if not forced
    if (!force) {
        printf("Are you sure you want to delete model '%s'? [y/N]: ", model_name);
        fflush(stdout);

        char answer = getchar();
        if (answer != 'y' && answer != 'Y') {
            printf("Deletion cancelled.\n");
            return 1;
        }
    }

    // Securely overwrite the file first (simple but effective)
    fp = fopen(path, "rb+");
    if (fp) {
        fseek(fp, 0, SEEK_END);
        long size = ftell(fp);
        rewind(fp);

        for (long i = 0; i < size; i++)
            fputc(0x00, fp);

        fclose(fp);
    }

    // Now remove it
    if (remove(path) != 0) {
        printf("fish_delete_model: failed to delete file: %s\n", path);
        return -1;
    }

    printf("Model '%s' has been deleted.\n", model_name);
    return 0;
}

/**
 * @brief Delete a dataset.
 *
 * Datasets are stored under "datasets/<name>" as raw files.
 * This function removes the dataset file after optional confirmation.
 */
int fish_delete_dataset(const char *dataset_name, int force)
{
    if (!dataset_name) {
        printf("fish_delete_dataset: invalid dataset name.\n");
        return -1;
    }

    /* Construct dataset path */
    char path[512];
    snprintf(path, sizeof(path), "datasets/%s", dataset_name);

    /* Check if file exists */
    FILE *fp = fopen(path, "rb");
    if (!fp) {
        printf("fish_delete_dataset: dataset '%s' not found.\n",
               dataset_name);
        return -1;
    }
    fclose(fp);

    /* If not forced, ask (via console) */
    if (!force) {
        printf("Delete dataset '%s'? [y/N]: ", dataset_name);
        fflush(stdout);

        char response[8] = {0};
        if (!fgets(response, sizeof(response), stdin)) {
            printf("fish_delete_dataset: cancelled.\n");
            return -1;
        }

        if (response[0] != 'y' && response[0] != 'Y') {
            printf("fish_delete_dataset: cancelled.\n");
            return -1;
        }
    }

    /* Delete file */
    if (remove(path) != 0) {
        printf("fish_delete_dataset: failed to remove '%s'.\n", path);
        return -1;
    }

    printf("fish_delete_dataset: removed '%s'.\n", path);
    return 0;
}
