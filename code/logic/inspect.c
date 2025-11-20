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

int fish_inspect(const char *model_name, int show_weights,
                 int summary, const char *layer_name) {
    printf("fish_inspect: model=%s, show_weights=%d, summary=%d, layer=%s\n",
           model_name, show_weights, summary, layer_name ? layer_name : "ALL");
    return 0;
}
