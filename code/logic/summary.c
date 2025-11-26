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

#define MAX_INPUT_SIZE   65536
#define MAX_SUMMARY_SIZE 8192

/* --------------------------------------------------------------------
 * Simple stub summary generator.
 * Replace with Jellyfish/Fossil AI summarizer later.
 * -------------------------------------------------------------------- */
static void backend_generate_summary(const char *content,
                                     int depth,
                                     char *out,
                                     size_t out_sz)
{
    /* Extremely simple logic for now */
    snprintf(out, out_sz,
        "[Summary depth=%d]: First 120 chars:\n%.120s\n",
        depth,
        content
    );
}

/* --------------------------------------------------------------------
 * fish_summary
 * -------------------------------------------------------------------- */
int fish_summary(const char *file_path, int depth, int time_flag)
{
    if (!file_path) {
        fprintf(stderr, "fish_summary: null file path.\n");
        return -1;
    }

    /* Load file */
    FILE *fp = fopen(file_path, "r");
    if (!fp) {
        fprintf(stderr,
            "fish_summary: cannot open '%s'.\n",
            file_path
        );
        return -1;
    }

    char *content = malloc(MAX_INPUT_SIZE);
    if (!content) {
        fclose(fp);
        fprintf(stderr, "fish_summary: out of memory.\n");
        return -1;
    }

    size_t read_bytes =
        fread(content, 1, MAX_INPUT_SIZE - 1, fp);
    content[read_bytes] = 0;
    fclose(fp);

    /* Start timing */
    clock_t start = 0, end = 0;
    if (time_flag)
        start = clock();

    /* Generate summary (stubbed for now) */
    char summary[MAX_SUMMARY_SIZE];
    backend_generate_summary(content, depth, summary, sizeof(summary));

    /* End timing */
    if (time_flag)
        end = clock();

    /* Print result */
    printf("%s\n", summary);

    if (time_flag) {
        double elapsed = (double)(end - start) /
                         (double)CLOCKS_PER_SEC;
        printf("[Timing] Generated summary in %.4f seconds.\n",
               elapsed);
    }

    free(content);
    return 0;
}
