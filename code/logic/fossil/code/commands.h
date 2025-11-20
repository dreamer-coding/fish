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
#ifndef FOSSIL_APP_COMMANDS_H
#define FOSSIL_APP_COMMANDS_H

#include <fossil/media/framework.h>
#include <fossil/sys/framework.h>
#include <fossil/ai/framework.h>
#include <fossil/io/framework.h>

#include <errno.h>

#ifdef _WIN32
#  include <direct.h>
#  define MKDIR(name) _mkdir(name)
#else
#  define MKDIR(name) mkdir(name, 0755)
#endif

#define MAX_PATH 1024

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create a new AI model.
 * 
 * @param name Name of the model.
 * @param type Type of the model.
 * @return int Status code (0 for success, non-zero for error).
 */
int fish_create(const char *name, const char *type);

/**
 * @brief Delete a model.
 * 
 * @param model_name Name of the model to delete.
 * @param force Flag to force deletion without confirmation (1: yes, 0: no).
 * @return int Status code.
 */
int fish_delete_model(const char *model_name, int force);

/**
 * @brief Train an AI model with a dataset.
 * 
 * @param model_name Name of the model to train.
 * @param dataset_path Path to the training dataset.
 * @param epochs Number of training epochs.
 * @param batch_size Size of each training batch.
 * @param lr Learning rate.
 * @return int Status code.
 */
int fish_train(const char *model_name, const char *dataset_path,
               int epochs, int batch_size, float lr);

/**
 * @brief Test an AI model using a dataset and metrics.
 * 
 * @param model_name Name of the model to test.
 * @param dataset_path Path to the test dataset.
 * @param metrics_list Comma-separated list of metrics.
 * @param save_file File to save test results.
 * @return int Status code.
 */
int fish_test(const char *model_name, const char *dataset_path,
              const char *metrics_list, const char *save_file);

/**
 * @brief Inspect an AI model's details.
 * 
 * @param model_name Name of the model.
 * @param show_weights Flag to show weights (1: show, 0: hide).
 * @param summary Flag to show summary (1: show, 0: hide).
 * @param layer_name Specific layer to inspect (NULL for all).
 * @return int Status code.
 */
int fish_inspect(const char *model_name, int show_weights,
                 int summary, const char *layer_name);

/**
 * @brief Save an AI model to a file.
 * 
 * @param model_name Name of the model.
 * @param file_path Path to save the model.
 * @param format File format (e.g., "bin", "json").
 * @return int Status code.
 */
int fish_save(const char *model_name, const char *file_path,
              const char *format);

/**
 * @brief Load an AI model from a file.
 * 
 * @param file_path Path to the model file.
 * @param override_session Flag to override current session (1: yes, 0: no).
 * @return int Status code.
 */
int fish_load(const char *file_path, int override_session);

/**
 * @brief Import a dataset from a file.
 * 
 * @param file_path Path to the dataset file.
 * @param format File format (e.g., "csv", "json").
 * @return int Status code.
 */
int fish_dataset_import(const char *file_path, const char *format);

/**
 * @brief Delete a dataset.
 * 
 * @param dataset_name Name of the dataset to delete.
 * @param force Flag to force deletion without confirmation (1: yes, 0: no).
 * @return int Status code.
 */
int fish_delete_dataset(const char *dataset_name, int force);

/**
 * @brief Clean the dataset (drop nulls, deduplicate, normalize).
 * 
 * @param drop_null Drop null values (1: yes, 0: no).
 * @param dedup Deduplicate entries (1: yes, 0: no).
 * @param normalize Normalize data (1: yes, 0: no).
 * @return int Status code.
 */
int fish_dataset_clean(int drop_null, int dedup, int normalize);

/**
 * @brief Preprocess the dataset (tokenize, scale, encode).
 * 
 * @param tokenize Tokenize data (1: yes, 0: no).
 * @param scale Scale data (1: yes, 0: no).
 * @param encode Encode data (1: yes, 0: no).
 * @return int Status code.
 */
int fish_dataset_preprocess(int tokenize, int scale, int encode);

/**
 * @brief Augment the dataset.
 * 
 * @param type Type of augmentation.
 * @param factor Augmentation factor.
 * @return int Status code.
 */
int fish_dataset_augment(const char *type, int factor);

/**
 * @brief Export the dataset to a file.
 * 
 * @param file_path Path to export file.
 * @param format Export format.
 * @return int Status code.
 */
int fish_dataset_export(const char *file_path, const char *format);

/**
 * @brief Get statistics for the dataset.
 * 
 * @param summary Show summary (1: yes, 0: no).
 * @param columns Columns to include (comma-separated).
 * @param plot Plot statistics (1: yes, 0: no).
 * @return int Status code.
 */
int fish_dataset_stats(int summary, const char *columns, int plot);

/**
 * @brief Split the dataset into train, validation, and test sets.
 * 
 * @param train_frac Fraction for training set.
 * @param val_frac Fraction for validation set.
 * @param test_frac Fraction for test set.
 * @return int Status code.
 */
int fish_dataset_split(float train_frac, float val_frac, float test_frac);

/**
 * @brief Ask a model a question using a prompt.
 * 
 * @param model_name Name of the model.
 * @param prompt Prompt string.
 * @param file_path Optional file path for input/output.
 * @param explain Flag to explain answer (1: yes, 0: no).
 * @return int Status code.
 */
int fish_ask(const char *model_name, const char *prompt,
             const char *file_path, int explain);

/**
 * @brief Start a chat session with a model.
 * 
 * @param model_name Name of the model.
 * @param keep_context Keep chat context (1: yes, 0: no).
 * @param save_file File to save chat history.
 * @return int Status code.
 */
int fish_chat(const char *model_name, int keep_context, const char *save_file);

/**
 * @brief Summarize a file using a model.
 * 
 * @param file_path Path to the file.
 * @param depth Summary depth.
 * @param time_flag Include timing info (1: yes, 0: no).
 * @return int Status code.
 */
int fish_summary(const char *file_path, int depth, int time_flag);


#ifdef __cplusplus
}
#endif

#endif /* FOSSIL_APP_CODE_H */
