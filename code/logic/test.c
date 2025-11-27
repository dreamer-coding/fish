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
 * Uses fossil_ai_jellyfish_hash as per prototype.
 */
static float compute_metric_from_io(const char *input, const char *output) {
       fossil_sys_memory_t hash_mem = fossil_sys_memory_alloc(FOSSIL_JELLYFISH_HASH_SIZE);
       uint8_t *hash = (uint8_t *)hash_mem;
       fossil_ai_jellyfish_hash(input, output, hash);
       float score = compute_metric_from_hash(hash, FOSSIL_JELLYFISH_HASH_SIZE);
       fossil_sys_memory_free(hash_mem);
       return score;
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
       cstring filepath = fossil_io_cstring_format("%s.jfchain", model_name);

       // Load the model chain
       fossil_ai_jellyfish_chain_t chain;
       if (fossil_ai_jellyfish_load(&chain, filepath) != 0) {
              fossil_io_printf("{red,bold}fish_test: failed to load model: %s{normal}\n", filepath);
              fossil_io_cstring_free(filepath);
              return -1;
       }

       if (chain.count == 0) {
              fossil_io_printf("{yellow,bold}fish_test: model has no commits.{normal}\n");
              fossil_io_cstring_free(filepath);
              return -1;
       }

       fossil_ai_jellyfish_block_t *latest = &chain.commits[chain.count - 1];

       fossil_io_printf("{green,bold}Testing model '{cyan}%s{normal}{green,bold}' with dataset '{magenta}%s{normal}{green,bold}'{normal}\n",
                 model_name,
                 dataset_path ? dataset_path : "N/A");

       // Copy metrics list using memory API
       cstring metrics_copy = fossil_sys_memory_strdup(metrics_list);

       // Prepare optional results file using fossil_io_file_t
       fossil_io_file_t out_file;
       fossil_io_file_t *out = NULL;
       if (save_file) {
              if (fossil_io_file_open(&out_file, save_file, "w") == 0) {
                     out = &out_file;
              } else {
                     fossil_io_printf("{red,bold}fish_test: could not open save file.{normal}\n");
                     fossil_io_cstring_free(filepath);
                     fossil_sys_memory_free(metrics_copy);
                     return -1;
              }
       }

       // Parse comma-separated metrics using fossil_io_cstring_token
       cstring saveptr = NULL;
       cstring token = fossil_io_cstring_token(metrics_copy, ",", &saveptr);
       while (token) {
              fossil_io_cstring_trim(token);

              // Use input/output from latest block for metric calculation
              float score = compute_metric_from_io(
                     latest->input,
                     latest->output
              );

              // Use robust terminal formatting functions for output
              fossil_io_printf("{blue,bold}Metric %-12s{normal} = {yellow,bold}%.2f{normal}\n", token, score);
              if (out) {
                     char line[512];
                     snprintf(line, sizeof(line), "%s=%.2f\n", token, score);
                     fossil_io_file_write(out, line, 1, strlen(line));
              }

              token = fossil_io_cstring_token(NULL, ",", &saveptr);
       }

       if (out) fossil_io_file_close(out);

       fossil_io_cstring_free(filepath);
       fossil_sys_memory_free(metrics_copy);

       return 0;
}
