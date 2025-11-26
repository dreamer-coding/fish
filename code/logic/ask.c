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

/* --------------------------------------------------------------
 * Internal mock backend call.
 * In the real system, this would call Jellyfish AI or Shark AI.
 * For now it generates a deterministic pseudo-answer.
 * -------------------------------------------------------------- */
static void backend_generate_reply(const char *model,
                                   const char *prompt,
                                   int explain,
                                   char *out, size_t out_sz)
{
    snprintf(out, out_sz,
        "[model=%s]\n"
        "Answer: \"%s\" -> processed.\n"
        "%s",
        model,
        prompt,
        explain ? "Explanation: (placeholder reasoning output).\n" : ""
    );
}

/* --------------------------------------------------------------
 * PUBLIC API
 * -------------------------------------------------------------- */
int fish_ask(const char *model_name, const char *prompt,
             const char *file_path, int explain)
{
    if (!model_name || !prompt) {
        fprintf(stderr, "fish_ask: invalid arguments.\n");
        return -1;
    }

    /* Prepare input buffer */
    char combined_input[8192] = {0};

    /* If file_path provided, append file contents to prompt */
    if (file_path) {
        FILE *fp = fopen(file_path, "r");
        if (!fp) {
            fprintf(stderr,
                "fish_ask: failed to open input file '%s'.\n",
                file_path);
            return -1;
        }

        char filebuf[4096];
        size_t r = fread(filebuf, 1, sizeof(filebuf)-1, fp);
        filebuf[r] = '\0';
        fclose(fp);

        snprintf(combined_input, sizeof(combined_input),
            "%s\n\n[Attached file contents:]\n%s",
            prompt, filebuf);
    } else {
        strncpy(combined_input, prompt, sizeof(combined_input)-1);
    }

    /* Prepare output buffer */
    char reply[16384];
    memset(reply, 0, sizeof(reply));

    /* Call internal backend generator */
    backend_generate_reply(model_name, combined_input, explain,
                           reply, sizeof(reply));

    /* If writing output to a file */
    if (file_path) {
        FILE *fp = fopen(file_path, "w");
        if (!fp) {
            fprintf(stderr,
                "fish_ask: failed to open output file '%s' for writing.\n",
                file_path);
            return -1;
        }
        fputs(reply, fp);
        fclose(fp);

        printf("fish_ask: output written to '%s'.\n", file_path);
    } else {
        /* STDOUT output */
        printf("%s\n", reply);
    }

    return 0;
}
