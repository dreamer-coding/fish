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

int fish_ask(const char *model_name, const char *prompt,
             const char *file_path, int explain) {
    printf("fish_ask: model=%s, prompt=%s, file=%s, explain=%d\n",
           model_name, prompt, file_path ? file_path : "NONE", explain);
    return 0;
}
