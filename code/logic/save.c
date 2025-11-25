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
 * @brief Save an AI model to a file.
 *
 * Models are stored in "<model>.jfchain".
 * This function loads the model and saves a copy to a new file.
 */
int fish_save(const char *model_name, const char *file_path,
              const char *format)
{
    if (!model_name || !file_path || !format) {
        printf("fish_save: invalid arguments.\n");
        return -1;
    }

    /* Only binary export is meaningful */
    if (strcmp(format, "bin") != 0) {
        printf("fish_save: unsupported format '%s' (only 'bin' allowed).\n", format);
        return -1;
    }

    /* Construct model path */
    char src_path[512];
    snprintf(src_path, sizeof(src_path), "%s.jfchain", model_name);

    /* Load model from existing file */
    fossil_ai_jellyfish_chain_t chain;
    memset(&chain, 0, sizeof(chain));

    if (fossil_ai_jellyfish_load(&chain, src_path) != 0) {
        printf("fish_save: failed to load model '%s'\n", model_name);
        return -1;
    }

    /* Save to the new file path */
    if (fossil_ai_jellyfish_save(&chain, file_path) != 0) {
        printf("fish_save: failed to save model to '%s'\n", file_path);
        return -1;
    }

    printf("fish_save: model '%s' saved to '%s'\n", model_name, file_path);
    return 0;
}
