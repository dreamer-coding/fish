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

/* simple numeric checker */
static int is_numeric(const char *s) {
    char *end;
    strtod(s, &end);
    return end != s;
}

/* random small noise between -0.05 and +0.05 */
static double rand_noise(void) {
    return ((rand() % 10000) / 10000.0 - 0.5) * 0.1;
}

int fish_dataset_augment(const char *type, int factor)
{
    if (!type || factor <= 0) {
        printf("fish_dataset_augment: invalid parameters.\n");
        return -1;
    }

    const char *path = "datasets/current.dataset";
    FILE *fp = fopen(path, "r");
    if (!fp) {
        printf("fish_dataset_augment: no active dataset.\n");
        return -1;
    }

    char **rows = malloc(sizeof(char*) * 200000);
    if (!rows) { fclose(fp); return -1; }

    size_t count = 0;
    char buf[4096];

    /* LOAD */
    while (fgets(buf, sizeof(buf), fp)) {
        rows[count++] = strdup(buf);
    }
    fclose(fp);

    if (count == 0) {
        printf("fish_dataset_augment: dataset empty.\n");
        free(rows);
        return 0;
    }

    /* determine number of columns */
    size_t cols = 1;
    for (char *p = rows[0]; *p; p++)
        if (*p == ',') cols++;

    printf("fish_dataset_augment: type=%s factor=%d columns=%zu\n",
           type, factor, cols);

    /* prepare output rows list with room for augmented rows */
    char **out = malloc(sizeof(char*) * (count * (factor + 1)));
    if (!out) return -1;

    size_t out_count = 0;

    /* copy originals */
    for (size_t i = 0; i < count; i++) {
        out[out_count++] = strdup(rows[i]);
    }

    /* AUGMENT */
    for (size_t i = 0; i < count; i++) {
        for (int k = 0; k < factor; k++) {
            char line[4096] = {0};
            char temp[4096] = {0};
            strcpy(temp, rows[i]);

            /* split row */
            char *fields[512] = {0};
            size_t fc = 0;
            char *tok = strtok(temp, ",");

            while (tok && fc < cols) {
                fields[fc++] = tok;
                tok = strtok(NULL, ",");
            }

            /* noise augmentation */
            if (strcmp(type, "noise") == 0) {
                for (size_t c = 0; c < fc; c++) {
                    if (is_numeric(fields[c])) {
                        double v = atof(fields[c]);
                        v += rand_noise();
                        char tmp[64];
                        snprintf(tmp, sizeof(tmp), "%.6f", v);
                        fields[c] = strdup(tmp);
                    }
                }
            }

            /* flip augmentation */
            else if (strcmp(type, "flip") == 0) {
                for (size_t c = 0; c < fc / 2; c++) {
                    char *tmp = fields[c];
                    fields[c] = fields[fc - 1 - c];
                    fields[fc - 1 - c] = tmp;
                }
            }

            /* shift augmentation */
            else if (strcmp(type, "shift") == 0) {
                char *last = fields[fc - 1];
                for (size_t c = fc - 1; c > 0; c--) {
                    fields[c] = fields[c - 1];
                }
                fields[0] = last;
            }

            /* reassemble row */
            for (size_t c = 0; c < fc; c++) {
                strcat(line, fields[c]);
                if (c + 1 < fc)
                    strcat(line, ",");
            }
            strcat(line, "\n");

            out[out_count++] = strdup(line);
        }
    }

    /* SAVE */
    fp = fopen(path, "w");
    if (!fp) {
        printf("fish_dataset_augment: write error.\n");
        return -1;
    }

    for (size_t i = 0; i < out_count; i++) {
        fputs(out[i], fp);
        free(out[i]);
    }

    fclose(fp);
    free(out);
    for (size_t i = 0; i < count; i++) free(rows[i]);
    free(rows);

    printf("fish_dataset_augment: wrote %zu rows.\n", out_count);
    return 0;
}
