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
int fish_inspect(const char *model_name, int show_weights,
                 int summary, const char *layer_name)
{
    if (!model_name || model_name[0] == '\0') {
        printf("fish_inspect: model_name missing.\n");
        return -1;
    }

    char path[512];
    snprintf(path, sizeof(path), "%s.jfchain", model_name);

    fossil_ai_jellyfish_chain_t chain;
    memset(&chain, 0, sizeof(chain));

    if (fossil_ai_jellyfish_load(&chain, path) != 0) {
        printf("fish_inspect: failed to load model '%s'\n", model_name);
        return -1;
    }

    printf("Inspecting AI model: %s\n", model_name);
    printf("--------------------------------------\n");

    /* SUMMARY SECTION */
    if (summary) {
        printf("Summary:\n");
        printf("  Branch count : %u\n", (unsigned)chain.branch_count);
        printf("  Commit count : %u\n", (unsigned)chain.count);
        printf("  Created at   : %llu\n",
               (unsigned long long)chain.created_at);
        printf("  Updated at   : %llu\n",
               (unsigned long long)chain.updated_at);
        printf("  Default branch: %s\n", chain.default_branch);
        printf("  Repo ID      : ");
        for (int i = 0; i < FOSSIL_DEVICE_ID_SIZE; i++)
            printf("%02X", chain.repo_id[i]);
        printf("\n\n");
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
        printf("Model Structure:\n");

        for (size_t i = 0; i < chain.count; i++) {
            fossil_ai_jellyfish_block_t *b = &chain.commits[i];
            if (!b->attributes.valid)
                continue;

            if (filter_specific_commit && b->identity.commit_index != target_index)
                continue;

            printf("Commit [%u]\n", b->identity.commit_index);

            printf("  Type     : %u\n", (unsigned)b->block_type);
            printf("  Parents  : %u\n", (unsigned)b->identity.parent_count);

            printf("  Hash     : ");
            for (int h = 0; h < FOSSIL_JELLYFISH_HASH_SIZE; h++)
                printf("%02X", b->identity.commit_hash[h]);
            printf("\n");

            printf("  Tree     : ");
            for (int h = 0; h < FOSSIL_JELLYFISH_HASH_SIZE; h++)
                printf("%02X", b->identity.tree_hash[h]);
            printf("\n");

            printf("  Message  : %s\n", b->identity.commit_message);
            printf("  Timestamp: %llu\n",
                   (unsigned long long)b->time.timestamp);
            printf("  Confidence: %.2f\n", b->attributes.confidence);

            if (b->identity.parent_count > 0) {
                printf("  Parent hashes:\n");
                for (int p = 0; p < b->identity.parent_count; ++p) {
                    printf("    - ");
                    for (int h = 0; h < FOSSIL_JELLYFISH_HASH_SIZE; h++)
                        printf("%02X", b->identity.parent_hashes[p][h]);
                    printf("\n");
                }
            }

            printf("\n");

            if (filter_specific_commit)
                break;  // only show one
        }
    }

    printf("Inspection complete.\n");
    return 0;
}
