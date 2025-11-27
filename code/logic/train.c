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

int fish_train(ccstring model_name, ccstring dataset_path,
               int epochs, int batch_size, float lr)
{
    if (!model_name) return -1;

    // Allocate filepath using memory API
    cstring filepath = (cstring)fossil_sys_memory_alloc(512);
    if (!filepath) return -1;
    fossil_io_cstring_format(filepath, 512, "%s.jfchain", model_name);

    // Use fossil_io_file_t to check if the model file exists and is readable
    if (!fossil_io_file_file_exists(filepath) || !fossil_io_file_is_readable(filepath)) {
        fossil_io_printf("{red,bold}Model file does not exist or is not readable: %s{normal}\n", filepath);
        fossil_sys_memory_free(filepath);
        return -1;
    }

    fossil_ai_jellyfish_chain_t chain;
    if (fossil_ai_jellyfish_load(&chain, filepath) != 0) {
        fossil_io_printf("{red,bold}Failed to load model: %s{normal}\n", filepath);
        fossil_sys_memory_free(filepath);
        return -1;
    }

    if (chain.count >= FOSSIL_JELLYFISH_MAX_MEM) {
        fossil_io_printf("{yellow,bold}Chain is full, cannot add new commit.{normal}\n");
        fossil_sys_memory_free(filepath);
        return -1;
    }

    // Simulate training by adding a new commit using the learn API
    cstring input = (cstring)fossil_sys_memory_alloc(128);
    cstring output = (cstring)fossil_sys_memory_alloc(128);
    fossil_io_cstring_format(input, 128, "epoch:%d batch:%d lr:%.4f", epochs, batch_size, lr);
    fossil_io_cstring_format(output, 128, "trained on %s", dataset_path ? dataset_path : "N/A");

    // Generate hash for input/output pair
    uint8_t hash[FOSSIL_JELLYFISH_HASH_SIZE];
    fossil_ai_jellyfish_hash(input, output, hash);

    // Learn new input-output pair (commit)
    fossil_ai_jellyfish_learn(&chain, input, output);

    // Optionally, find the block just added and set its hash explicitly (if needed)
    fossil_ai_jellyfish_block_t *block = fossil_ai_jellyfish_find(&chain, hash);
    if (block) {
        fossil_ai_jellyfish_mark_immutable(block);
        fossil_ai_jellyfish_block_set_message(block, "Training commit");
        fossil_ai_jellyfish_tag_block(block, "train");
    }

    fossil_sys_memory_free(input);
    fossil_sys_memory_free(output);

    chain.updated_at = (uint64_t)time(NULL);

    // Use fossil_io_file_is_writable before saving
    if (!fossil_io_file_is_writable(filepath)) {
        fossil_io_printf("{red,bold}Model file is not writable: %s{normal}\n", filepath);
        fossil_sys_memory_free(filepath);
        return -1;
    }

    if (fossil_ai_jellyfish_save(&chain, filepath) != 0) {
        fossil_io_printf("{red,bold}Failed to save model after training.{normal}\n");
        fossil_sys_memory_free(filepath);
        return -1;
    }

    fossil_io_printf(
        "{green,bold}Trained model '{cyan}%s{green}' on dataset '{magenta}%s{green}' "
        "(%d epochs, batch %d, lr %.4f){normal}\n",
        model_name, dataset_path ? dataset_path : "N/A", epochs, batch_size, lr
    );

    fossil_sys_memory_free(filepath);
    return 0;
}
