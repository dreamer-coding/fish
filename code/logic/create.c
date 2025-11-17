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
#include <sys/stat.h>
#include <errno.h>

/**
 * Write text to a file using fossil_fstream_t.
 */
static int write_file(const char *path, const char *text) {
    fossil_fstream_t stream;
    int rc = fossil_fstream_open(&stream, path, "w");
    if (rc != 0) return errno;

    size_t len = strlen(text);
    size_t written = fossil_fstream_write(&stream, text, 1, len);

    fossil_fstream_close(&stream);
    return (written == len) ? 0 : EIO;
}

/**
 * Create directories recursively: "a/b/c"
 */
static int mkdir_recursive(ccstring path) {
    char tmp[512];

    size_t len = fossil_io_cstring_length_safe(path, sizeof(tmp) - 1);
    if (len >= sizeof(tmp)) return ENAMETOOLONG;

    cstring safe_path = fossil_io_cstring_copy_safe(path, sizeof(tmp) - 1);
    if (!safe_path) return ENOMEM;

    strcpy(tmp, safe_path);
    fossil_io_cstring_free_safe(&safe_path);

    for (char *p = tmp + 1; *p; p++) {
        if (*p == '/' || *p == '\\') {
            *p = '\0';
            if (MKDIR(tmp) != 0 && errno != EEXIST)
                return errno;
            *p = '/';
        }
    }

    if (MKDIR(tmp) != 0 && errno != EEXIST)
        return errno;

    return 0;
}

/**
 * Create a new Jellyfish AI module.
 *
 * Generates:
 *   <name>.fish/
 *       module.fson
 *       records.fson
 *       cross.fson
 *       chains/
 *           chain_0.fson
 */
int fish_create(ccstring name) {
    if (!name || !*name) {
        fossil_io_printf("{red,bold}Error:{normal} Invalid module name.\n");
        return FOSSIL_MEDIA_FSON_ERR_INVALID_ARG;
    }

    char base[256];
    fossil_io_cstring_format_safe(base, sizeof(base), "%s.fish", name);

    // Root directory
    int rc = mkdir_recursive(base);
    if (rc != 0 && rc != EEXIST) {
        fossil_io_printf(
            "{red,bold}Error:{normal} Failed to create directory '%s'.\n",
            base
        );
        return FOSSIL_MEDIA_FSON_ERR_IO;
    }

    // chains/ directory
    char chains_dir[256];
    fossil_io_cstring_format_safe(chains_dir, sizeof(chains_dir), "%s/chains", base);
    rc = mkdir_recursive(chains_dir);
    if (rc != 0 && rc != EEXIST) {
        fossil_io_printf(
            "{red,bold}Error:{normal} Failed to create directory '%s'.\n",
            chains_dir
        );
        return FOSSIL_MEDIA_FSON_ERR_IO;
    }

    fossil_media_fson_error_t err;

    /* -----------------------------
       module.fson
       ----------------------------- */
    fossil_media_fson_value_t *module_obj = fossil_media_fson_new_object();
    if (!module_obj) return FOSSIL_MEDIA_FSON_ERR_NOMEM;

    fossil_media_fson_object_set(module_obj, "name",        fossil_media_fson_new_string("module"));
    fossil_media_fson_object_set(module_obj, "id",          fossil_media_fson_new_string("default"));
    fossil_media_fson_object_set(module_obj, "version",     fossil_media_fson_new_i32(1));
    fossil_media_fson_object_set(module_obj, "description", fossil_media_fson_new_string("Jellyfish AI Module"));

    char module_path[256];
    fossil_io_cstring_format_safe(module_path, sizeof(module_path), "%s/module.fson", base);

    rc = fossil_media_fson_write_file(module_obj, module_path, 1, &err);
    fossil_media_fson_free(module_obj);

    if (rc != FOSSIL_MEDIA_FSON_OK) {
        fossil_io_printf(
            "{red,bold}Error:{normal} Failed to write '%s': %s\n",
            module_path, err.message
        );
        return rc;
    }

    /* -----------------------------
       records.fson
       ----------------------------- */
    fossil_media_fson_value_t *records_obj = fossil_media_fson_new_object();
    if (!records_obj) return FOSSIL_MEDIA_FSON_ERR_NOMEM;

    fossil_media_fson_value_t *records_arr = fossil_media_fson_new_array();
    fossil_media_fson_object_set(records_obj, "records", records_arr);

    char records_path[256];
    fossil_io_cstring_format_safe(records_path, sizeof(records_path), "%s/records.fson", base);

    rc = fossil_media_fson_write_file(records_obj, records_path, 1, &err);
    fossil_media_fson_free(records_obj);

    if (rc != FOSSIL_MEDIA_FSON_OK) {
        fossil_io_printf(
            "{red,bold}Error:{normal} Failed to write '%s': %s\n",
            records_path, err.message
        );
        return rc;
    }

    /* -----------------------------
       cross.fson
       ----------------------------- */
    fossil_media_fson_value_t *cross_obj = fossil_media_fson_new_object();
    if (!cross_obj) return FOSSIL_MEDIA_FSON_ERR_NOMEM;

    fossil_media_fson_value_t *cross_arr = fossil_media_fson_new_array();
    fossil_media_fson_object_set(cross_obj, "cross_reference", cross_arr);

    char cross_path[256];
    fossil_io_cstring_format_safe(cross_path, sizeof(cross_path), "%s/cross.fson", base);

    rc = fossil_media_fson_write_file(cross_obj, cross_path, 1, &err);
    fossil_media_fson_free(cross_obj);

    if (rc != FOSSIL_MEDIA_FSON_OK) {
        fossil_io_printf(
            "{red,bold}Error:{normal} Failed to write '%s': %s\n",
            cross_path, err.message
        );
        return rc;
    }

    /* -----------------------------
       chains/chain_0.fson
       ----------------------------- */
    fossil_media_fson_value_t *chain_obj = fossil_media_fson_new_object();
    if (!chain_obj) return FOSSIL_MEDIA_FSON_ERR_NOMEM;

    fossil_media_fson_object_set(chain_obj, "id", fossil_media_fson_new_i32(0));

    fossil_media_fson_value_t *chain_arr = fossil_media_fson_new_array();
    fossil_media_fson_object_set(chain_obj, "chain", chain_arr);

    char chain0_path[256];
    fossil_io_cstring_format_safe(chain0_path, sizeof(chain0_path),
                                  "%s/chain_0.fson", chains_dir);

    rc = fossil_media_fson_write_file(chain_obj, chain0_path, 1, &err);
    fossil_media_fson_free(chain_obj);

    if (rc != FOSSIL_MEDIA_FSON_OK) {
        fossil_io_printf(
            "{red,bold}Error:{normal} Failed to write '%s': %s\n",
            chain0_path, err.message
        );
        return rc;
    }

    fossil_io_printf(
        "{green,bold}Success:{normal} Jellyfish AI module '{bold}%s{normal}' created.\n",
        name
    );

    return FOSSIL_MEDIA_FSON_OK;
}
