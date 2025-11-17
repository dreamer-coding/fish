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
 * 🧬 AI Development Commands (Model Lifecycle)
 *
 * These commands manage the lifecycle of AI modules, including creation,
 * deletion, inspection, training, auditing, content generation, pruning,
 * decay, and search. Each function corresponds to a CLI command and
 * supports various flags for customization.
 */

/**
 * Create a new AI module.
 * @param name Name of the AI module to create.
 * @return Status code (0 for success, non-zero for error).
 * Flags:
 *   -n, --name <id> : Name of the AI module to create.
 */
int fish_create(const char *name);

/**
 * Remove modules or checkpoints.
 * @param target Target module or checkpoint to delete.
 * @param force Force deletion without confirmation.
 * @param interactive Prompt before deleting.
 * @return Status code.
 * Flags:
 *   -f, --force      : Force deletion without confirmation.
 *   -i, --interactive: Prompt before deleting.
 */
int fish_delete(const char *target, bool force, bool interactive);

/**
 * Inspect internal structure of a model or dataset.
 * @param target Target model or dataset to inspect.
 * @param crossref Check cross references.
 * @param core Show core module info.
 * @param chains Inspect memory chains.
 * @return Status code.
 * Flags:
 *   --crossref : Check cross references.
 *   --core     : Show core module info.
 *   --chains   : Inspect memory chains.
 */
int fish_introspect(const char *target, bool crossref, bool core, bool chains);

/**
 * Train or fine-tune an AI module.
 * @param module Module to train.
 * @param epochs Number of training epochs.
 * @param batch Batch size.
 * @param lr Learning rate.
 * @param resume_ckpt Resume from checkpoint.
 * @param save_path Save output path.
 * @return Status code.
 * Flags:
 *   --epochs <n>      : Number of training epochs.
 *   --batch <n>       : Batch size.
 *   --lr <rate>       : Learning rate.
 *   --resume <ckpt>   : Resume from checkpoint.
 *   --save <path>     : Save output path.
 */
int fish_train(const char *module, int epochs, int batch, float lr,
               const char *resume_ckpt, const char *save_path);

/**
 * Analyze model behavior for safety, bias, drift, or anomalies.
 * @param module Module to audit.
 * @param drift Check for drift.
 * @param bias Check for bias.
 * @param toxic Check for toxicity.
 * @param woke Check for woke signals.
 * @param explain Explain findings.
 * @param export_path Export results path.
 * @return Status code.
 * Flags:
 *   --drift    : Check for drift.
 *   --bias     : Check for bias.
 *   --toxic    : Check for toxicity.
 *   --woke     : Check for woke signals.
 *   --explain  : Explain findings.
 *   --export <path> : Export results.
 */
int fish_audit(const char *module, bool drift, bool bias, bool toxic, bool woke,
               bool explain, const char *export_path);

/**
 * Generate new content, modules, or data samples.
 * @param model Model to use.
 * @param prompt Prompt text.
 * @param type Output type (fson/json/yaml/etc).
 * @param length Output length.
 * @param count Number of samples.
 * @param seed Random seed.
 * @param save_path Save output path.
 * @param temperature Sampling temperature.
 * @return Status code.
 * Flags:
 *   -m, --model <id>      : Model to use.
 *   -p, --prompt <text>   : Prompt text.
 *   --type <format>       : Output type (fson/json/yaml/etc).
 *   --length <n>          : Output length.
 *   --count <n>           : Number of samples.
 *   --seed <n>            : Random seed.
 *   --save <path>         : Save output path.
 *   --temperature <value> : Sampling temperature.
 */
int fish_imagine(const char *model, const char *prompt, const char *type,
                 int length, int count, int seed, const char *save_path,
                 float temperature);

/**
 * Remove unused or low-value parameters, entries, or metadata.
 * @param module Module to prune.
 * @param small Remove small parameters.
 * @param redundant Remove redundant items.
 * @param orphans Remove orphaned entries.
 * @return Status code.
 * Flags:
 *   --small     : Remove small parameters.
 *   --redundant : Remove redundant items.
 *   --orphans   : Remove orphaned entries.
 */
int fish_prune(const char *module, bool small, bool redundant, bool orphans);

/**
 * Apply weight decay or gradual forgetting to models or chains.
 * @param module Module to decay.
 * @param strength Decay strength.
 * @param simulate Simulate decay only.
 * @return Status code.
 * Flags:
 *   --strength <value> : Decay strength.
 *   --simulate         : Simulate decay only.
 */
int fish_decay(const char *module, float strength, bool simulate);

/**
 * AI-enhanced search across datasets, chains, or models.
 * @param query Search query.
 * @param semantic Enable semantic search.
 * @return Status code.
 * Flags:
 *   --query <text> : Search query.
 *   --semantic     : Enable semantic search.
 */
int fish_seek(const char *query, bool semantic);


/**
 * 🧪 AI Testing Commands
 *
 * These commands are used for testing and interacting with AI modules,
 * including running prompts, interactive chat, and summarization.
 */

/**
 * Run a one-shot prompt against a module or chain.
 * @param model Model to use.
 * @param file Input file.
 * @param explain Explain output.
 * @param quiet Suppress output.
 * @return Status code.
 * Flags:
 *   -m, --model <id> : Model to use.
 *   -f, --file <path>: Input file.
 *   --explain        : Explain output.
 *   --quiet          : Suppress output.
 */
int fish_ask(const char *model, const char *file, bool explain, bool quiet);

/**
 * Interactive conversation session with a local module.
 * @param model Model to use.
 * @param keep_context Keep conversation history.
 * @param save_file Save transcript to file.
 * @return Status code.
 * Flags:
 *   --context     : Keep conversation history.
 *   --save <file> : Save transcript to file.
 *   -m, --model <id> : Model to use.
 */
int fish_chat(const char *model, bool keep_context, const char *save_file);

/**
 * Summarize datasets, chains, logs, or model states.
 * @param file Input file.
 * @param depth Summary depth.
 * @param color Color output.
 * @param time Include timing info.
 * @return Status code.
 * Flags:
 *   -f, --file <path>: Input file.
 *   --depth <n>      : Summary depth.
 *   --color          : Color output.
 *   --time           : Include timing info.
 */
int fish_summary(const char *file, int depth, bool color, bool time);


/**
 * 🧩 Dataset Subcommands (fish dataset)
 *
 * These subcommands operate on datasets for adding, removing, tagging,
 * cleaning, statistics, splitting, and verification.
 */

/**
 * Add new data samples (text, json, binary).
 * @param dataset Dataset to add to.
 * @param source Source identifier.
 * @param label Label for the data.
 * @return Status code.
 * Flags:
 *   --label <tag>   : Label for the data.
 *   --source <id>   : Source identifier.
 */
int fish_dataset_add(const char *dataset, const char *source, const char *label);

/**
 * Delete dataset items.
 * @param dataset Dataset to remove from.
 * @param id_range Range of IDs to remove.
 * @param filter Filter expression.
 * @return Status code.
 * Flags:
 *   --id <range>    : Range of IDs to remove.
 *   --filter <expr> : Filter expression.
 */
int fish_dataset_remove(const char *dataset, const char *id_range, const char *filter);

/**
 * Apply internal tags to dataset entries.
 * @param dataset Dataset to tag.
 * @param add_tag Tag to add.
 * @param remove_tag Tag to remove.
 * @return Status code.
 * Flags:
 *   --add <tag>     : Tag to add.
 *   --remove <tag>  : Tag to remove.
 */
int fish_dataset_tag(const char *dataset, const char *add_tag, const char *remove_tag);

/**
 * Sanitize dataset entries (dedupe, normalize).
 * @param dataset Dataset to clean.
 * @param dedupe Remove duplicates.
 * @param normalize Normalize entries.
 * @param strip_html Strip HTML tags.
 * @return Status code.
 * Flags:
 *   --dedupe        : Remove duplicates.
 *   --normalize     : Normalize entries.
 *   --strip-html    : Strip HTML tags.
 */
int fish_dataset_clean(const char *dataset, bool dedupe, bool normalize, bool strip_html);

/**
 * Show analytics and distribution stats.
 * @param dataset Dataset to analyze.
 * @param tokens Show token statistics.
 * @param lengths Show length statistics.
 * @param labels Show label statistics.
 * @return Status code.
 * Flags:
 *   --tokens        : Show token statistics.
 *   --lengths       : Show length statistics.
 *   --labels        : Show label statistics.
 */
int fish_dataset_stats(const char *dataset, bool tokens, bool lengths, bool labels);

/**
 * Train/test/validation splitting.
 * @param dataset Dataset to split.
 * @param train_pct Training set percentage.
 * @param test_pct Test set percentage.
 * @param seed Random seed.
 * @return Status code.
 * Flags:
 *   --train <pct>   : Training set percentage.
 *   --test <pct>    : Test set percentage.
 *   --seed <n>      : Random seed.
 */
int fish_dataset_split(const char *dataset, int train_pct, int test_pct, int seed);

/**
 * Validate dataset structure and format.
 * @param dataset Dataset to validate.
 * @param strict Enable strict validation.
 * @return Status code.
 * Flags:
 *   --strict        : Enable strict validation.
 */
int fish_dataset_verify(const char *dataset, bool strict);

#ifdef __cplusplus
}
#endif

#endif /* FOSSIL_APP_CODE_H */
