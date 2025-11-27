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
 * @brief Create a new Jellyfish AI model (chain) and save it to disk.
 * 
 * @param name Model name (used for file naming).
 * @return int Status code.
 */
int fish_create(ccstring name) {
    if (!name) return -1;

    fossil_ai_jellyfish_chain_t chain;
    fossil_sys_memory_zero(&chain, sizeof(chain)); // Zero the chain memory

    fossil_ai_jellyfish_init(&chain);

    // Timestamps
    uint64_t now = (uint64_t)time(NULL);
    chain.created_at = now;
    chain.updated_at = now;

    // Branch info
    chain.branch_count = 1;
    cstring branch_name = fossil_io_cstring_create_safe("main", sizeof(chain.default_branch));
    fossil_sys_memory_zero(chain.default_branch, sizeof(chain.default_branch));
    fossil_sys_memory_copy(chain.default_branch, branch_name, sizeof(chain.default_branch) - 1);
    fossil_io_cstring_free_safe(&branch_name);

    // Repo ID placeholder
    for (size_t i = 0; i < FOSSIL_DEVICE_ID_SIZE; i++)
        chain.repo_id[i] = (uint8_t)i;

    // Generate first commit using prototype function
    fossil_ai_jellyfish_block_t *init_block = fossil_ai_jellyfish_add_commit(
        &chain,
        "init",
        "init",
        FOSSIL_JELLYFISH_COMMIT_INIT,
        NULL,
        0,
        "Initial commit"
    );
    if (!init_block) {
        fossil_io_printf("{red,bold}Error:{normal} Failed to create initial commit block.\n");
        return -1;
    }
    fossil_sys_memory_copy(init_block->data, &now, sizeof(now));
    chain.count = 1;

    cstring filepath = fossil_io_cstring_format_safe(512, "%s.jfchain", name);

    // Save chain using prototype function
    if (fossil_ai_jellyfish_save(&chain, filepath) != 0) {
        fossil_io_printf("{red,bold}Error:{normal} Could not save chain to file: %s\n", filepath);
        fossil_io_cstring_free_safe(&filepath);
        return -1;
    }

    fossil_io_printf("{green,bold}Created new Jellyfish AI model:{normal} %s\n", name);
    fossil_io_cstring_free_safe(&filepath);
    return 0;
}
