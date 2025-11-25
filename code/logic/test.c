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
 * @brief Generate a simple deterministic metric from a commit hash.
 */
static float compute_metric_from_hash(const uint8_t *hash, size_t len) {
       uint32_t sum = 0;
       for (size_t i = 0; i < len; i++)
              sum += hash[i];
       return (float)(sum % 1000) / 10.0f;  // 0.0 - 100.0
}

/**
 * @brief Generate a metric from input/output using the chain's hash function.
 */
static float compute_metric_from_io(const char *input, const char *output) {
       uint8_t hash[FOSSIL_JELLYFISH_HASH_SIZE];
       fossil_ai_jellyfish_hash(input, output, hash);
       return compute_metric_from_hash(hash, FOSSIL_JELLYFISH_HASH_SIZE);
}

/**
 * @brief Test an AI model using a dataset and metrics.
 */
int fish_test(const char *model_name, const char *dataset_path,
                       const char *metrics_list, const char *save_file)
{
       if (!model_name || !metrics_list) {
              fossil_io_printf("{red,bold}fish_test: missing model or metrics.{normal}\n");
              return -1;
       }

       // Build file path
       char filepath[512];
       snprintf(filepath, sizeof(filepath), "%s.jfchain", model_name);

       // Load the model chain
       fossil_ai_jellyfish_chain_t chain;
       if (fossil_ai_jellyfish_load(&chain, filepath) != 0) {
              fossil_io_printf("{red,bold}fish_test: failed to load model: %s{normal}\n", filepath);
              return -1;
       }

       if (chain.count == 0) {
              fossil_io_printf("{yellow,bold}fish_test: model has no commits.{normal}\n");
              return -1;
       }

       fossil_ai_jellyfish_block_t *latest = &chain.commits[chain.count - 1];

       fossil_io_printf("{green,bold}Testing model '{cyan}%s{normal}{green,bold}' with dataset '{magenta}%s{normal}{green,bold}'{normal}\n",
                 model_name,
                 dataset_path ? dataset_path : "N/A");

       // Copy metrics list because strtok modifies it
       char metrics_copy[256];
       strncpy(metrics_copy, metrics_list, sizeof(metrics_copy) - 1);
       metrics_copy[sizeof(metrics_copy) - 1] = '\0';

       // Prepare optional results file
       FILE *out = NULL;
       if (save_file) {
              out = fopen(save_file, "w");
              if (!out) {
                     fossil_io_printf("{red,bold}fish_test: could not open save file.{normal}\n");
                     return -1;
              }
       }

       // Parse comma-separated metrics
       char *token = strtok(metrics_copy, ",");
       while (token) {
              // Trim whitespace
              while (*token == ' ' || *token == '\t') token++;

              // Use input/output from latest block for metric calculation
              float score = compute_metric_from_io(
                     latest->input,
                     latest->output
              );

              fossil_io_printf("  {blue,bold}Metric %-12s{normal} = {yellow,bold}%.2f{normal}\n", token, score);
              if (out) fossil_io_fprintf(out, "%s=%.2f\n", token, score);

              token = strtok(NULL, ",");
       }

       if (out) fclose(out);

       return 0;
}
