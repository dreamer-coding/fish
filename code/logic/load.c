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
 * @brief Load an AI model from a file using fossil_io_file_t.
 *
 * This loads the .jfchain file and, if override_session=1,
 * writes it into "<model_name>.jfchain".
 *
 * If override_session=0, the model is only validated/loaded but not persisted.
 * After loading, verifies chain integrity and prints fingerprint.
 */
int fish_load(ccstring file_path, int override_session)
{
    if (!file_path) {
        fossil_io_printf("{red,bold}fish_load: invalid path.{normal}\n");
        return -1;
    }

    fossil_io_printf("{green,bold}fish_load: loading model from '{normal}%s{green,bold}'\n", file_path);

    /* Open the model file using fossil_io_file_t */
    fossil_io_file_t model_file;
    if (fossil_io_file_open(&model_file, file_path, "rb") != 0) {
        fossil_io_printf("{red,bold}fish_load: failed to open model file '{normal}%s{red,bold}'\n", file_path);
        return -1;
    }

    /* Allocate and zero the model chain using fossil_sys_memory_calloc */
    fossil_ai_jellyfish_chain_t *chain = (fossil_ai_jellyfish_chain_t *)
        fossil_sys_memory_calloc(1, sizeof(fossil_ai_jellyfish_chain_t));

    if (!chain) {
        fossil_io_printf("{red,bold}fish_load: failed to allocate memory for model chain.{normal}\n");
        fossil_io_file_close(&model_file);
        return -1;
    }

    /* Load the model into memory */
    if (fossil_ai_jellyfish_load(chain, file_path) != 0) {
        fossil_io_printf("{red,bold}fish_load: failed to load model file '{normal}%s{red,bold}'\n", file_path);
        fossil_sys_memory_free(chain);
        fossil_io_file_close(&model_file);
        return -1;
    }

    fossil_io_file_close(&model_file);

    /* Verify chain integrity */
    bool valid = fossil_ai_jellyfish_verify_chain(chain);
    fossil_io_printf(valid
        ? "{green,bold}fish_load: chain integrity verified.{normal}\n"
        : "{red,bold}fish_load: chain integrity check failed!{normal}\n");

    /* Print fingerprint */
    uint8_t fingerprint[FOSSIL_JELLYFISH_HASH_SIZE];
    fossil_ai_jellyfish_chain_fingerprint(chain, fingerprint);
    fossil_io_printf("{cyan,bold}fish_load: chain fingerprint: {normal}");
    for (size_t i = 0; i < FOSSIL_JELLYFISH_HASH_SIZE; ++i)
        fossil_io_printf("%02x", fingerprint[i]);
    fossil_io_printf("\n");

    /* If override disabled, we stop here */
    if (!override_session) {
        fossil_io_printf("{yellow,bold}fish_load: model loaded into memory (override disabled).{normal}\n");
        fossil_sys_memory_free(chain);
        return 0;
    }

    /* Otherwise, write it to a default model filename */
    ccstring base = strrchr(file_path, '/');
    base = base ? base + 1 : file_path;

    char *model_name = fossil_sys_memory_strdup(base);
    char *dot = strrchr(model_name, '.');
    if (dot) *dot = '\0'; /* remove extension */

    cstring out_path = fossil_io_cstring_format("%s.jfchain", model_name);

    fossil_io_file_t out_file;
    if (fossil_io_file_open(&out_file, out_path, "wb") != 0) {
        fossil_io_printf("{red,bold}fish_load: failed to open output file '{normal}%s{red,bold}'\n", out_path);
        fossil_sys_memory_free(model_name);
        fossil_io_cstring_free(out_path);
        fossil_sys_memory_free(chain);
        return -1;
    }

    if (fossil_ai_jellyfish_save(chain, out_path) != 0) {
        fossil_io_printf("{red,bold}fish_load: failed to save loaded model to '{normal}%s{red,bold}'\n", out_path);
        fossil_io_file_close(&out_file);
        fossil_sys_memory_free(model_name);
        fossil_io_cstring_free(out_path);
        fossil_sys_memory_free(chain);
        return -1;
    }

    fossil_io_file_close(&out_file);

    fossil_io_printf("{green,bold}fish_load: model persisted as '{normal}%s{green,bold}'\n", out_path);

    fossil_sys_memory_free(model_name);
    fossil_io_cstring_free(out_path);
    fossil_sys_memory_free(chain);
    return 0;
}
