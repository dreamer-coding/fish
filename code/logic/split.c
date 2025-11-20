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

int fish_dataset_split(float train_frac, float val_frac, float test_frac) {
    printf("fish_dataset_split: train=%.2f, val=%.2f, test=%.2f\n",
           train_frac, val_frac, test_frac);
    return 0;
}
