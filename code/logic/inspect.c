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
 * @brief Inspect an AI model's details.
 *
 * Loads <model_name>.jfchain and prints structural info:
 * - Summary: commit counts, branch counts, timestamps
 * - Weights: in Jellyfish AI this means commit hashes + relationships
 * - Layer: maps to commit index or specific commit ID filter
 */
int fish_inspect(ccstring model_name, int show_weights,
                 int summary, ccstring layer_name)
{
    if (!model_name || model_name[0] == '\0') {
        fossil_io_printf("{red,bold}fish_inspect: model_name missing.{normal}\n");
        return -1;
    }

    cstring path = fossil_io_cstring_format("%s.jfchain", model_name);

    fossil_ai_jellyfish_chain_t chain;
    fossil_sys_memory_zero(&chain, sizeof(chain));

    fossil_io_file_t model_file;
    if (fossil_io_file_open(&model_file, path, "rb") != 0) {
        fossil_io_printf("{red,bold}fish_inspect: failed to open model file '%s'{normal}\n", path);
        fossil_io_cstring_free(path);
        return -1;
    }

    if (fossil_ai_jellyfish_load(&chain, path) != 0) {
        fossil_io_printf("{red,bold}fish_inspect: failed to load model '%s'{normal}\n", model_name);
        fossil_io_file_close(&model_file);
        fossil_io_cstring_free(path);
        return -1;
    }

    fossil_io_printf("{green,bold}Inspecting AI model: %s{normal}\n", model_name);
    fossil_io_printf("{yellow}--------------------------------------{normal}\n");

    /* SUMMARY SECTION */
    if (summary) {
        fossil_io_printf("{cyan,bold}Summary:{normal}\n");
        fossil_io_printf("  Branch count : {bold}%u{normal}\n", (unsigned)chain.branch_count);
        fossil_io_printf("  Commit count : {bold}%u{normal}\n", (unsigned)chain.count);
        fossil_io_printf("  Created at   : {bold}%llu{normal}\n",
               (unsigned long long)chain.created_at);
        fossil_io_printf("  Updated at   : {bold}%llu{normal}\n",
               (unsigned long long)chain.updated_at);
        fossil_io_printf("  Default branch: {bold}%s{normal}\n", chain.default_branch);
        fossil_io_printf("  Repo ID      : ");
        for (int i = 0; i < FOSSIL_DEVICE_ID_SIZE; i++)
            fossil_io_printf("{magenta}%02X{normal}", chain.repo_id[i]);
        fossil_io_printf("\n");

        // Print chain trust score
        float trust_score = fossil_ai_jellyfish_chain_trust_score(&chain);
        fossil_io_printf("  Trust score  : {bold}%.2f{normal}\n", trust_score);

        // Print knowledge coverage
        float coverage = fossil_ai_jellyfish_knowledge_coverage(&chain);
        fossil_io_printf("  Knowledge coverage: {bold}%.2f{normal}\n", coverage);

        // Print fingerprint
        uint8_t fingerprint[FOSSIL_JELLYFISH_HASH_SIZE];
        fossil_sys_memory_zero(fingerprint, sizeof(fingerprint));
        fossil_ai_jellyfish_chain_fingerprint(&chain, fingerprint);
        fossil_io_printf("  Fingerprint  : ");
        for (int i = 0; i < FOSSIL_JELLYFISH_HASH_SIZE; i++)
            fossil_io_printf("{magenta}%02X{normal}", fingerprint[i]);
        fossil_io_printf("\n\n");
    }

    /* If user wants a specific "layer" → treat as commit index or hash prefix */
    int filter_specific_commit = 0;
    unsigned target_index = 0;

    if (layer_name && layer_name[0] != '\0') {
        filter_specific_commit = 1;
        target_index = (unsigned)atoi(layer_name);
    }

    /* COMMIT / WEIGHT INSPECTION SECTION */
    if (show_weights || filter_specific_commit) {
        fossil_io_printf("{cyan,bold}Model Structure:{normal}\n");

        for (size_t i = 0; i < chain.count; i++) {
            fossil_ai_jellyfish_block_t *b = &chain.commits[i];
            if (!b->attributes.valid)
                continue;

            if (filter_specific_commit && b->identity.commit_index != target_index)
                continue;

            fossil_io_printf("{yellow}Commit [{bold}%u{normal}{yellow}]{normal}\n", b->identity.commit_index);

            fossil_io_printf("  Type     : {bold}%u{normal} (%s)\n", 
                (unsigned)b->block_type, fossil_ai_jellyfish_commit_type_name(b->block_type));
            fossil_io_printf("  Parents  : {bold}%u{normal}\n", (unsigned)b->identity.parent_count);

            fossil_io_printf("  Hash     : ");
            for (int h = 0; h < FOSSIL_JELLYFISH_HASH_SIZE; h++)
                fossil_io_printf("{magenta}%02X{normal}", b->identity.commit_hash[h]);
            fossil_io_printf("\n");

            fossil_io_printf("  Tree     : ");
            for (int h = 0; h < FOSSIL_JELLYFISH_HASH_SIZE; h++)
                fossil_io_printf("{blue}%02X{normal}", b->identity.tree_hash[h]);
            fossil_io_printf("\n");

            fossil_io_printf("  Message  : {bold}%s{normal}\n", b->identity.commit_message);
            fossil_io_printf("  Timestamp: {bold}%llu{normal}\n",
                   (unsigned long long)b->time.timestamp);
            fossil_io_printf("  Confidence: {bold}%.2f{normal}\n", b->attributes.confidence);

            // Print block trust score
            bool valid_block = fossil_ai_jellyfish_verify_block(b);
            fossil_io_printf("  Valid     : {bold}%s{normal}\n", valid_block ? "yes" : "no");

            // Print block age
            uint64_t now = fossil_io_time_now();
            uint64_t age = fossil_ai_jellyfish_block_age(b, now);
            fossil_io_printf("  Age (us)  : {bold}%llu{normal}\n", (unsigned long long)age);

            // Print block explanation
            char explain[128];
            fossil_sys_memory_zero(explain, sizeof(explain));
            fossil_ai_jellyfish_block_explain(b, explain, sizeof(explain));
            fossil_io_printf("  Explain   : {bold}%s{normal}\n", explain);

            if (b->identity.parent_count > 0) {
                fossil_io_printf("  Parent hashes:\n");
                for (int p = 0; p < b->identity.parent_count; ++p) {
                    fossil_io_printf("    - ");
                    for (int h = 0; h < FOSSIL_JELLYFISH_HASH_SIZE; h++)
                        fossil_io_printf("{magenta}%02X{normal}", b->identity.parent_hashes[p][h]);
                    fossil_io_printf("\n");
                }
            }

            fossil_io_printf("\n");

            if (filter_specific_commit)
                break;  // only show one
        }
    }

    fossil_io_printf("{green}Inspection complete.{normal}\n");
    fossil_io_file_close(&model_file);
    fossil_io_cstring_free(path);
    return 0;
}
