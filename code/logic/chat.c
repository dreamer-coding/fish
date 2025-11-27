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

#define MAX_HISTORY 32768
#define MAX_LINE    2048

/* --------------------------------------------------------------
 * Internal Jellyfish model reply generator.
 * Uses fossil_ai_jellyfish_chain_t for reasoning.
 * -------------------------------------------------------------- */
static void backend_chat_reply(const char *model,
                               const char *user_msg,
                               char *out, size_t out_sz)
{
    static fossil_ai_jellyfish_chain_t chain;
    static int initialized = 0;

    if (!initialized) {
        fossil_ai_jellyfish_init(&chain);
        initialized = 1;
    }

    // Allocate output buffer using memory API
    fossil_sys_memory_t output_mem = fossil_sys_memory_alloc(MAX_LINE * 2);
    char *output = (char *)output_mem;
    float confidence = 0.0f;
    const fossil_ai_jellyfish_block_t *block = NULL;
    bool found = fossil_ai_jellyfish_reason_verbose(&chain, user_msg, output, &confidence, &block);

    if (found) {
        snprintf(out, out_sz,
            "[{cyan}%s{normal}]: {yellow}%s{normal}\n{dim}(confidence: %.2f){normal}\n",
            model, output, confidence);
    } else {
        snprintf(out, out_sz,
            "[{cyan}%s{normal}]: I received: \"{yellow}%s{normal}\"\n",
            model, user_msg);
    }

    // Learn this input/output pair for future context
    fossil_ai_jellyfish_learn(&chain, user_msg, out);

    // Free output buffer
    fossil_sys_memory_free(output_mem);
}
