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

#define MAX_HISTORY 32768
#define MAX_LINE    2048

/* --------------------------------------------------------------
 * Internal stub model reply generator.
 * Replace with Jellyfish/Fossil AI call later.
 * -------------------------------------------------------------- */
static void backend_chat_reply(const char *model,
                               const char *user_msg,
                               char *out, size_t out_sz)
{
    snprintf(out, out_sz,
        "[%s]: I received: \"%s\"\n",
        model,
        user_msg
    );
}

/* --------------------------------------------------------------
 * fish_chat - Start an interactive chat session.
 * -------------------------------------------------------------- */
int fish_chat(const char *model_name, int keep_context,
              const char *save_file)
{
    if (!model_name) {
        fprintf(stderr, "fish_chat: null model name.\n");
        return -1;
    }

    char history[MAX_HISTORY] = {0};
    char line[MAX_LINE];
    char reply[MAX_LINE * 4];

    FILE *save = NULL;

    if (save_file) {
        save = fopen(save_file, "w");
        if (!save) {
            fprintf(stderr,
                "fish_chat: failed to open '%s' for writing.\n",
                save_file
            );
            return -1;
        }
    }

    printf("=== Starting chat with model '%s' ===\n", model_name);
    printf("(type '/exit' to quit)\n\n");

    while (1) {
        printf("You> ");
        fflush(stdout);

        if (!fgets(line, sizeof(line), stdin))
            break;

        /* Strip newline */
        line[strcspn(line, "\n")] = 0;

        if (strcmp(line, "/exit") == 0)
            break;

        /* Update history if enabled */
        if (keep_context) {
            strncat(history, "You: ", sizeof(history)-1);
            strncat(history, line, sizeof(history)-1);
            strncat(history, "\n", sizeof(history)-1);
        }

        /* Generate model reply */
        backend_chat_reply(model_name, line, reply, sizeof(reply));

        printf("%s", reply);

        if (keep_context) {
            strncat(history, reply, sizeof(history)-1);
        }

        /* Save to file if enabled */
        if (save) {
            fputs("You: ", save);
            fputs(line, save);
            fputc('\n', save);

            fputs(reply, save);
            fflush(save);
        }
    }

    printf("\n=== Chat session ended ===\n");

    if (save) {
        fclose(save);
        printf("Chat history saved to '%s'\n", save_file);
    }

    return 0;
}
