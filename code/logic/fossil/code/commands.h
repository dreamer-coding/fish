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

#include <fossil/sys/framework.h>
#include <fossil/io/framework.h>

#include <errno.h>

#if defined(_WIN32)
#include <direct.h>
#define MKDIR(path) _mkdir(path)
#define PATH_SEPARATOR "\\"
#else
#include <sys/stat.h>
#include <sys/types.h>
#define MKDIR(path) mkdir(path, 0755)
#define PATH_SEPARATOR "/"
#endif

#ifdef __cplusplus
extern "C" {
#endif

// ================================
// AI Development Commands (Model Lifecycle)
// ================================

/**
 * Creates a new AI model or component of specified type.
 * Supports creating modules, datasets, profiles, or training plans with templates.
 * 
 * @param type The type of component to create (model, dataset, etc.)
 * @param name The name identifier for the new component
 * @param config Path to configuration file or configuration string
 * @param template_id Optional template identifier to base creation on
 * @return 0 on success, non-zero error code on failure
 * 
 * Common flags: -t/--type <module/dataset/profile>, -n/--name <id>, 
 *               --config <file>, --template <id>
 */
int fish_create(const char *type, const char *name,
                const char *config, const char *template_id);

/**
 * Permanently removes AI models, datasets, or other components from the system.
 * Provides options for forced deletion and cleanup of orphaned metadata.
 * 
 * @param target The identifier of the target to delete
 * @param force Force deletion without additional confirmation
 * @param interactive Enable interactive mode for selective deletion
 * @param prune Remove associated orphaned data and dependencies
 * @return 0 on success, non-zero error code on failure
 * 
 * Common flags: -f/--force, -i/--interactive, --prune (remove orphaned metadata)
 */
int fish_delete(const char *target, bool force,
                bool interactive, bool prune);

/**
 * Performs deep introspection and analysis of AI models.
 * Inspects internal structure including weights, layers, tokens, and validates integrity.
 * 
 * @param path Path to the model or component to introspect
 * @param weights Include weight analysis in the introspection
 * @param layers Include layer-by-layer analysis
 * @param tokens Include tokenization and vocabulary analysis
 * @param verify Perform integrity verification during introspection
 * @return 0 on success, non-zero error code on failure
 * 
 * Common flags: --weights (show tensor stats), --layers (architecture), 
 *               --tokens (token stats), --verify (validate integrity)
 */
int fish_introspect(const char *path, bool weights,
                    bool layers, bool tokens, bool verify);

/**
 * Trains or fine-tunes a Jellyfish AI model using specified dataset and parameters.
 * Supports resuming from checkpoints and customizable training hyperparameters.
 * 
 * @param module The model module to train
 * @param dataset Path or identifier of the training dataset
 * @param epochs Number of training epochs to perform
 * @param batch Batch size for training
 * @param lr Learning rate for the training process
 * @param resume_ckpt Optional checkpoint to resume training from
 * @param save_path Path to save the trained model
 * @return 0 on success, non-zero error code on failure
 * 
 * Common flags: -d/--dataset <path>, --epochs <n>, --batch <n>, --lr <rate>,
 *               --resume <checkpoint>, --save <path>
 */
int fish_train(const char *module, const char *dataset,
               int epochs, int batch, float lr,
               const char *resume_ckpt, const char *save_path);

/**
 * Conducts comprehensive audit of AI model for various quality metrics.
 * Analyzes model behavior for safety, bias, drift, toxicity, and other anomalies.
 * 
 * @param module The model module to audit
 * @param drift Check for model drift and performance degradation
 * @param bias Analyze model for bias and fairness issues
 * @param toxicity Test model outputs for toxic or harmful content
 * @param explain Generate explanations for audit findings
 * @param export_path Optional path to export detailed audit report
 * @return 0 on success, non-zero error code on failure
 * 
 * Common flags: --drift, --bias, --toxicity, --explain, --export <path>
 */
int fish_audit(const char *module, bool drift,
               bool bias, bool toxicity, bool explain,
               const char *export_path);

/**
 * Displays information and metadata about AI components.
 * Shows comprehensive details including statistics, metadata, and tag information.
 * 
 * @param target The component to display information about
 * @param all Show all available information
 * @param stats Include statistical information
 * @param meta Include metadata and properties
 * @param tags Include tag information
 * @return 0 on success, non-zero error code on failure
 * 
 * Common flags: -a/--all, --stats, --meta, --tags
 */
int fish_show(const char *target, bool all, bool stats,
              bool meta, bool tags);

/**
 * Views and examines dataset contents and structure.
 * Displays raw dataset entries, tokens, or training samples with flexible options.
 * 
 * @param dataset The dataset to view
 * @param number Number of entries to display
 * @param sample Display a random sample of entries
 * @param shuffle Shuffle the order of displayed entries
 * @return 0 on success, non-zero error code on failure
 * 
 * Common flags: -n/--number <count>, --sample (random selection), --shuffle
 */
int fish_view(const char *dataset, int number,
              bool sample, bool shuffle);

/**
 * Exports AI models or datasets to various formats for interoperability and deployment.
 * Supports AI-centric formats including fson (Fossil AI JSON), onnx, and tensor formats.
 * 
 * @param id Identifier of the component to export
 * @param format Target export format (fson/onnx/tensor)
 * @param dest Destination path for the exported file
 * @param compress Apply compression to the exported data
 * @param include_meta Include metadata in the export
 * @param overwrite Overwrite existing files at destination
 * @param quiet Suppress verbose output during export
 * @return 0 on success, non-zero error code on failure
 * 
 * Common flags: -f/--format <fson/onnx/tensor>, --dest <path>, --compress,
 *               --include-metadata, --overwrite, --quiet
 */
int fish_export(const char *id, const char *format,
                const char *dest, bool compress,
                bool include_meta, bool overwrite, bool quiet);

/**
 * Imports AI models or datasets from external sources with validation.
 * Supports various AI formats with integrity checking and conflict resolution.
 * 
 * @param format Source format of the data being imported
 * @param source Path or URL of the source data
 * @param validate Perform validation during import process
 * @param name Optional name to assign to imported component
 * @param replace Replace existing component with same name
 * @param quiet Suppress verbose output during import
 * @return 0 on success, non-zero error code on failure
 * 
 * Common flags: -f/--format <fson/onnx/tensor>, --source <path>, --validate,
 *               --name <id>, --replace, --quiet
 */
int fish_import(const char *format, const char *source,
                bool validate, const char *name,
                bool replace, bool quiet);

/**
 * Generates creative content using AI models based on prompts.
 * Ideal for prototyping, synthetic data creation, or creative outputs with multiple format support.
 * 
 * @param model The AI model to use for generation
 * @param prompt Input prompt or seed text for generation
 * @param type Type of content to generate (fson, json, yaml, toml, markdown, html, xml, csv, text, ini)
 * @param length Maximum length of generated content
 * @param count Number of variations to generate
 * @param seed Random seed for reproducible generation
 * @param save_path Path to save generated content
 * @param temperature Creativity/randomness parameter (0.0-1.0)
 * @return 0 on success, non-zero error code on failure
 * 
 * Common flags: -m/--model <id>, -p/--prompt <text>, --type <format>,
 *               --length <n>, --count <n>, --seed <n>, --save <path>, --temperature <value>
 */
int fish_imagine(const char *model, const char *prompt,
                 const char *type, int length, int count,
                 int seed, const char *save_path,
                 float temperature);

/**
 * Merges two AI models or datasets using specified strategy.
 * Supports union, overwrite, and append strategies with dry-run capability.
 * 
 * @param left First model/dataset to merge
 * @param right Second model/dataset to merge
 * @param strategy Merge strategy (union/overwrite/append)
 * @param dry_run Perform merge simulation without actual changes
 * @return 0 on success, non-zero error code on failure
 * 
 * Common flags: -s/--strategy <union/overwrite/append>, --dry-run
 */
int fish_merge(const char *left, const char *right,
               const char *strategy, bool dry_run);

/**
 * Rebases and reconfigures AI model architecture or parameters.
 * Applies dataset or configuration changes onto existing models with alignment options.
 * 
 * @param module The model module to rebase
 * @param config New configuration to apply during rebase
 * @param align Perform alignment optimization during rebase
 * @return 0 on success, non-zero error code on failure
 * 
 * Common flags: --config <file>, --align
 */
int fish_rebase(const char *module, const char *config,
                bool align);

/**
 * Removes unnecessary or redundant components from AI models.
 * Cleans up unused parameters, entries, or metadata to optimize model size.
 * 
 * @param module The model module to prune
 * @param small Remove small/insignificant weights and connections
 * @param redundant Remove redundant layers and parameters
 * @param orphans Remove orphaned data and unused components
 * @return 0 on success, non-zero error code on failure
 * 
 * Common flags: --small, --redundant, --orphans
 */
int fish_prune(const char *module, bool small,
               bool redundant, bool orphans);

/**
 * Applies controlled degradation to AI models for robustness testing.
 * Implements weight decay or gradual forgetting mechanisms with simulation support.
 * 
 * @param module The model module to apply decay to
 * @param strength Intensity of decay effect (0.0-1.0)
 * @param simulate Perform simulation without permanent changes
 * @return 0 on success, non-zero error code on failure
 * 
 * Common flags: --strength <value>, --simulate
 */
int fish_decay(const char *module, float strength,
               bool simulate);

/**
 * AI-enhanced search across datasets or models using various methods.
 * Supports both semantic and regex-based pattern matching.
 * 
 * @param query Search query string or pattern
 * @param semantic Use semantic search instead of literal matching
 * @param regex_mode Enable regular expression pattern matching
 * @return 0 on success, non-zero error code on failure
 * 
 * Common flags: --query <text>, --semantic, --regex
 */
int fish_seek(const char *query, bool semantic,
              bool regex_mode);

// ================================
// AI Testing Commands
// ================================

/**
 * Runs one-shot prompts against AI models and receives responses.
 * Supports file input and detailed explanations of model responses.
 * 
 * @param model The AI model to query
 * @param file Optional file containing question or context
 * @param explain Request detailed explanation of the response
 * @param quiet Suppress verbose output and formatting
 * @return 0 on success, non-zero error code on failure
 * 
 * Common flags: -m/--model <id>, -f/--file <path>, --explain, --quiet
 */
int fish_ask(const char *model, const char *file,
             bool explain, bool quiet);

/**
 * Initiates interactive conversation session with AI model.
 * Maintains context across interactions with optional conversation saving.
 * 
 * @param model The AI model to chat with
 * @param keep_context Maintain conversation context across interactions
 * @param save_file Optional file to save chat conversation history
 * @return 0 on success, non-zero error code on failure
 * 
 * Common flags: -m/--model <id>, --context (keep history), --save <file>
 */
int fish_chat(const char *model, bool keep_context,
              const char *save_file);

/**
 * Generates summaries of files, datasets, or model outputs.
 * Provides configurable depth and formatting options for comprehensive analysis.
 * 
 * @param file Path to file or data to summarize
 * @param depth Level of detail in the summary (1-10)
 * @param color Enable colored output for better readability
 * @param time Include timing information in summary
 * @return 0 on success, non-zero error code on failure
 * 
 * Common flags: -f/--file <path>, --depth <n>, --color, --time
 */
int fish_summary(const char *file, int depth,
                 bool color, bool time);

// ================================
// Dataset Subcommands (fish dataset)
// ================================

/**
 * Adds new data entries to an existing dataset.
 * Supports various data types (text, json, binary) with labeling.
 * 
 * @param dataset Target dataset to add data to
 * @param source Path or URL of data source to add
 * @param label Optional label or category for the added data
 * @return 0 on success, non-zero error code on failure
 * 
 * Common flags: --source <id>, --label <tag>
 */
int fish_dataset_add(const char *dataset, const char *source,
                     const char *label);

/**
 * Removes specific entries from a dataset based on ID or filter criteria.
 * Supports range-based and conditional deletion with flexible filtering.
 * 
 * @param dataset Target dataset to remove data from
 * @param id_range Range of IDs to remove (e.g., "1-100", "50")
 * @param filter Filter expression for conditional removal
 * @return 0 on success, non-zero error code on failure
 * 
 * Common flags: --id <range>, --filter <expr>
 */
int fish_dataset_remove(const char *dataset,
                        const char *id_range,
                        const char *filter);

/**
 * Manages tags and labels associated with dataset entries.
 * Provides flexible tagging system for data organization and categorization.
 * 
 * @param dataset Target dataset to modify tags for
 * @param add_tag Tag to add to selected entries
 * @param remove_tag Tag to remove from selected entries
 * @return 0 on success, non-zero error code on failure
 * 
 * Common flags: --add <tag>, --remove <tag>
 */
int fish_dataset_tag(const char *dataset,
                     const char *add_tag,
                     const char *remove_tag);

/**
 * Performs data cleaning and preprocessing operations on dataset.
 * Sanitizes dataset entries with deduplication, normalization, and HTML stripping.
 * 
 * @param dataset Target dataset to clean
 * @param dedupe Remove duplicate entries from dataset
 * @param normalize Apply normalization to data values
 * @param strip_html Remove HTML tags and formatting from text data
 * @return 0 on success, non-zero error code on failure
 * 
 * Common flags: --dedupe, --normalize, --strip-html
 */
int fish_dataset_clean(const char *dataset,
                       bool dedupe, bool normalize,
                       bool strip_html);

/**
 * Generates statistical analysis and reports for dataset contents.
 * Provides comprehensive analytics including token, length, and label distribution stats.
 * 
 * @param dataset Target dataset to analyze
 * @param tokens Include token-level statistics for text data
 * @param lengths Include length distribution statistics
 * @param labels Include label and category distribution stats
 * @return 0 on success, non-zero error code on failure
 * 
 * Common flags: --tokens, --lengths, --labels
 */
int fish_dataset_stats(const char *dataset,
                       bool tokens, bool lengths,
                       bool labels);

/**
 * Splits dataset into training, validation, and test subsets.
 * Provides reproducible splitting with customizable ratios and random seeding.
 * 
 * @param dataset Target dataset to split
 * @param train_pct Percentage of data for training set (0-100)
 * @param test_pct Percentage of data for test set (0-100)
 * @param seed Random seed for reproducible splitting
 * @return 0 on success, non-zero error code on failure
 * 
 * Common flags: --train <pct>, --test <pct>, --seed <n>
 */
int fish_dataset_split(const char *dataset,
                       int train_pct, int test_pct,
                       int seed);

/**
 * Verifies dataset integrity, format compliance, and quality.
 * Validates dataset structure with optional strict checking for enhanced validation.
 * 
 * @param dataset Target dataset to verify
 * @param strict Enable strict validation with enhanced checks
 * @return 0 on success, non-zero error code on failure
 * 
 * Common flags: --strict
 */
int fish_dataset_verify(const char *dataset,
                        bool strict);

#ifdef __cplusplus
}
#endif

#endif /* FOSSIL_APP_CODE_H */
