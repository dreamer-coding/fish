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
int fish_create(const char *name) {
    if (!name) return -1;

    fossil_ai_jellyfish_chain_t chain;
    fossil_ai_jellyfish_init(&chain);

    // Timestamps
    uint64_t now = (uint64_t)time(NULL);
    chain.created_at = now;
    chain.updated_at = now;

    // Branch info
    chain.branch_count = 1;
    strncpy(chain.default_branch, "main", sizeof(chain.default_branch) - 1);

    // Repo ID placeholder
    for (size_t i = 0; i < FOSSIL_DEVICE_ID_SIZE; i++) chain.repo_id[i] = (uint8_t)i;

    // Generate first commit hash using the provided hash function
    fossil_ai_jellyfish_hash("init", "init", chain.commits[0].hash);
    chain.commits[0].index = 0;
    memcpy(chain.commits[0].data, &now, sizeof(now)); // Store timestamp in data

    chain.count = 1; // One commit exists

    // Generate output filename
    char filepath[512];
    snprintf(filepath, sizeof(filepath), "%s.jfchain", name);

    fossil_io_file_t file_stream;
    memset(&file_stream, 0, sizeof(file_stream));
    if (fossil_io_file_open(&file_stream, filepath, "wb") != 0) {
        fossil_io_printf("{red,bold}Error:{normal} Could not create file: %s\n", filepath);
        return -1;
    }

    // Write header
    struct {
        char     magic[8];
        uint32_t version;
        uint32_t commit_capacity;
        uint32_t commit_count;
        uint32_t valid_count;
        uint32_t branch_count;
        uint64_t created_at;
        uint64_t updated_at;
        uint8_t  repo_id[FOSSIL_DEVICE_ID_SIZE];
        char     default_branch[64];
    } hdr;

    memset(&hdr, 0, sizeof(hdr));
    memcpy(hdr.magic, "JFCHAIN", 7);
    hdr.version = 1;
    hdr.commit_capacity = FOSSIL_JELLYFISH_MAX_MEM;
    hdr.commit_count = chain.count;
    hdr.valid_count = chain.count;
    hdr.branch_count = chain.branch_count;
    hdr.created_at = chain.created_at;
    hdr.updated_at = chain.updated_at;
    memcpy(hdr.repo_id, chain.repo_id, FOSSIL_DEVICE_ID_SIZE);
    strncpy(hdr.default_branch, chain.default_branch, sizeof(hdr.default_branch) - 1);

    if (fossil_io_file_write(&file_stream, &hdr, 1, sizeof(hdr)) != sizeof(hdr)) {
        fossil_io_printf("{red,bold}Error:{normal} Failed to write header to file: %s\n", filepath);
        fossil_io_file_close(&file_stream);
        return -1;
    }

    // Write branch record
    struct {
        char name[64];
        uint8_t head_hash[FOSSIL_JELLYFISH_HASH_SIZE];
    } br;

    memset(&br, 0, sizeof(br));
    strncpy(br.name, chain.default_branch, sizeof(br.name) - 1);
    memcpy(br.head_hash, chain.commits[0].hash, FOSSIL_JELLYFISH_HASH_SIZE);
    if (fossil_io_file_write(&file_stream, &br, 1, sizeof(br)) != sizeof(br)) {
        fossil_io_printf("{red,bold}Error:{normal} Failed to write branch record to file: %s\n", filepath);
        fossil_io_file_close(&file_stream);
        return -1;
    }

    // Write first commit record
    if (fossil_io_file_write(&file_stream, &chain.commits[0], 1, sizeof(chain.commits[0])) != sizeof(chain.commits[0])) {
        fossil_io_printf("{red,bold}Error:{normal} Failed to write commit record to file: %s\n", filepath);
        fossil_io_file_close(&file_stream);
        return -1;
    }

    fossil_io_file_close(&file_stream);

    fossil_io_printf("{green,bold}Created new Jellyfish AI model:{normal} %s\n", name);
    return 0;
}
