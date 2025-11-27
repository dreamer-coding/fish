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
 * @brief Save an AI model to a file using fossil_io_file_t streams.
 *
 * Models are stored in "<model>.jfchain".
 * This function loads the model using fossil_ai_jellyfish_load and saves a copy using fossil_ai_jellyfish_save.
 * Uses fossil_sys_memory_strdup and fossil_sys_memory_free for string memory management.
 */
int fish_save(ccstring model_name, ccstring file_path, ccstring format)
{
    if (!model_name || !file_path || !format) {
        fossil_io_printf("{red,bold}fish_save: invalid arguments.{normal}\n");
        return -1;
    }

    /* Only binary export is meaningful */
    if (!fossil_io_cstring_iequals(format, "bin")) {
        fossil_io_printf("{yellow,bold}fish_save: unsupported format '%s' (only 'bin' allowed).{normal}\n", format);
        return -1;
    }

    /* Construct model path using safe string formatting and memory API */
    char temp_path[256];
    snprintf(temp_path, sizeof(temp_path), "%s.jfchain", model_name);
    char *src_path = fossil_sys_memory_strdup(temp_path);
    if (!src_path) {
        fossil_io_printf("{red,bold}fish_save: failed to allocate model path.{normal}\n");
        return -1;
    }

    fossil_ai_jellyfish_chain_t chain;
    fossil_ai_jellyfish_init(&chain);

    if (fossil_ai_jellyfish_load(&chain, src_path) != 0) {
        fossil_io_printf("{red,bold}fish_save: failed to load model from '%s'{normal}\n", src_path);
        fossil_sys_memory_free(src_path);
        return -1;
    }

    int result = fossil_ai_jellyfish_save(&chain, file_path);
    fossil_sys_memory_free(src_path);

    if (result == 0) {
        fossil_io_printf("{green,bold}fish_save: model '%s' saved to '%s'{normal}\n", model_name, file_path);
    } else {
        fossil_io_printf("{red,bold}fish_save: failed to save model to '%s'{normal}\n", file_path);
    }

    return result;
}
