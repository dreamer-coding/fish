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

int fish_chat(const char *model_name, int keep_context, const char *save_file) {
    printf("fish_chat: model=%s, keep_context=%d, save_file=%s\n",
           model_name, keep_context, save_file ? save_file : "NONE");
    return 0;
}
