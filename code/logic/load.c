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
 * @brief Load an AI model from a file.
 *
 * This loads the .jfchain file and, if override_session=1,
 * writes it into "<model_name>.jfchain".
 *
 * If override_session=0, the model is only validated/loaded but not persisted.
 */
int fish_load(const char *file_path, int override_session)
{
    if (!file_path) {
        printf("fish_load: invalid path.\n");
        return -1;
    }

    printf("fish_load: loading model from '%s'\n", file_path);

    /* Load the model into memory */
    fossil_ai_jellyfish_chain_t chain;
    memset(&chain, 0, sizeof(chain));

    if (fossil_ai_jellyfish_load(&chain, file_path) != 0) {
        printf("fish_load: failed to load model file '%s'\n", file_path);
        return -1;
    }

    /* If override disabled, we stop here */
    if (!override_session) {
        printf("fish_load: model loaded into memory (override disabled).\n");
        return 0;
    }

    /* Otherwise, write it to a default model filename */

    /* Derive model name from input filename (strip extension) */
    const char *base = strrchr(file_path, '/');
    base = base ? base + 1 : file_path;

    char model_name[256];
    strncpy(model_name, base, sizeof(model_name)-1);
    model_name[sizeof(model_name)-1] = '\0';

    char *dot = strrchr(model_name, '.');
    if (dot) *dot = '\0'; /* remove extension */

    /* Construct output file name */
    char out_path[512];
    snprintf(out_path, sizeof(out_path), "%s.jfchain", model_name);

    /* Save the loaded model to the new canonical filename */
    if (fossil_ai_jellyfish_save(&chain, out_path) != 0) {
        printf("fish_load: failed to save loaded model to '%s'\n", out_path);
        return -1;
    }

    printf("fish_load: model persisted as '%s'\n", out_path);
    return 0;
}
