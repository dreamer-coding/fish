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
 * This function loads the chain, securely overwrites the file, and removes it after confirmation.
 */
int fish_delete_model(ccstring model_name, int force)
{
    if (!model_name || model_name[0] == '\0') {
        fossil_io_printf("{red,bold}fish_delete_model: missing model name.{normal}\n");
        return -1;
    }

    cstring path = fossil_io_cstring_format("%s.jfchain", model_name);

    // Check file existence first
    if (!fossil_io_file_file_exists(path)) {
        fossil_io_printf("{red}fish_delete_model: model not found: %s{normal}\n", path);
        fossil_io_cstring_free(path);
        return -1;
    }

    // Ask for confirmation if not forced
    if (!force) {
        fossil_io_printf("{yellow}Are you sure you want to delete model '{bold}%s{normal}{yellow}'? [y/N]: {normal}", model_name);
        fossil_io_file_flush(FOSSIL_STDOUT);

        char answer = getchar();
        if (answer != 'y' && answer != 'Y') {
            fossil_io_printf("{green}Deletion cancelled.{normal}\n");
            fossil_io_cstring_free(path);
            return 1;
        }
    }

    // Load the chain for possible cleanup before deletion
    fossil_ai_jellyfish_chain_t chain;
    if (fossil_ai_jellyfish_load(&chain, path) == 0) {
        fossil_ai_jellyfish_cleanup(&chain);
    }

    // Securely overwrite the file using fossil_sys_memory_secure_zero
    fossil_io_file_t file_stream;
    if (fossil_io_file_open(&file_stream, path, "rb+") == 0) {
        int32_t size = fossil_io_file_get_size(&file_stream);
        fossil_io_file_rewind(&file_stream);

        fossil_sys_memory_t zero_buf = fossil_sys_memory_alloc(size);
        fossil_sys_memory_secure_zero(zero_buf, size);
        fossil_io_file_write(&file_stream, zero_buf, 1, size);
        fossil_sys_memory_free(zero_buf);

        fossil_io_file_close(&file_stream);
    }

    // Now remove it
    if (fossil_io_file_delete(path) != 0) {
        fossil_io_printf("{red}fish_delete_model: failed to delete file: %s{normal}\n", path);
        fossil_io_cstring_free(path);
        return -1;
    }

    fossil_io_printf("{green,bold}Model '%s' has been deleted.{normal}\n", model_name);
    fossil_io_cstring_free(path);
    return 0;
}

/**
 * @brief Delete a dataset.
 *
 * Datasets are stored under "datasets/<name>" as raw files.
 * This function removes the dataset file after optional confirmation.
 */
int fish_delete_dataset(ccstring dataset_name, int force)
{
    if (!dataset_name) {
        fossil_io_printf("{red}fish_delete_dataset: invalid dataset name.{normal}\n");
        return -1;
    }

    /* Construct dataset path */
    cstring path = fossil_io_cstring_format("datasets/%s", dataset_name);

    /* Check if file exists */
    if (!fossil_io_file_file_exists(path)) {
        fossil_io_printf("{red}fish_delete_dataset: dataset '{bold}%s{normal}{red}' not found.{normal}\n", dataset_name);
        fossil_io_cstring_free(path);
        return -1;
    }

    /* If not forced, ask (via console) */
    if (!force) {
        fossil_io_printf("{yellow}Delete dataset '{bold}%s{normal}{yellow}'? [y/N]: {normal}", dataset_name);
        fossil_io_file_flush(FOSSIL_STDOUT);

        char response[8] = {0};
        if (!fgets(response, sizeof(response), stdin)) {
            fossil_io_printf("{green}fish_delete_dataset: cancelled.{normal}\n");
            fossil_io_cstring_free(path);
            return -1;
        }

        if (response[0] != 'y' && response[0] != 'Y') {
            fossil_io_printf("{green}fish_delete_dataset: cancelled.{normal}\n");
            fossil_io_cstring_free(path);
            return -1;
        }
    }

    /* Securely overwrite the dataset file before deletion */
    fossil_io_file_t file_stream;
    if (fossil_io_file_open(&file_stream, path, "rb+") == 0) {
        int32_t size = fossil_io_file_get_size(&file_stream);
        fossil_io_file_rewind(&file_stream);

        fossil_sys_memory_t zero_buf = fossil_sys_memory_alloc(size);
        fossil_sys_memory_secure_zero(zero_buf, size);
        fossil_io_file_write(&file_stream, zero_buf, 1, size);
        fossil_sys_memory_free(zero_buf);

        fossil_io_file_close(&file_stream);
    }

    /* Delete file */
    if (fossil_io_file_delete(path) != 0) {
        fossil_io_printf("{red}fish_delete_dataset: failed to remove '{bold}%s{normal}{red}'.{normal}\n", path);
        fossil_io_cstring_free(path);
        return -1;
    }

    fossil_io_printf("{green,bold}fish_delete_dataset: removed '{bold}%s{normal}{green}'.{normal}\n", path);
    fossil_io_cstring_free(path);
    return 0;
}
