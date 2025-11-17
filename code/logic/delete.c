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
 * Recursively delete a directory tree.
 * Uses Fossil's I/O wrappers, no external dependencies.
 */
static int delete_recursive(const char *path) {
    fossil_dir_t dir;
    fossil_dirent_t ent;

    if (fossil_io_isfile(path)) {
        return (fossil_io_remove(path) == 0) ? 0 : errno;
    }

    if (!fossil_io_isdir(path)) {
        return ENOENT;
    }

    if (fossil_dir_open(&dir, path) != 0)
        return errno;

    while (fossil_dir_read(&dir, &ent) == 0) {
        if (strcmp(ent.name, ".") == 0 || strcmp(ent.name, "..") == 0)
            continue;

        char full[512];
        fossil_io_cstring_format_safe(full, sizeof(full), "%s/%s", path, ent.name);

        if (fossil_io_isdir(full)) {
            int rc = delete_recursive(full);
            if (rc != 0) {
                fossil_dir_close(&dir);
                return rc;
            }
        } else {
            if (fossil_io_remove(full) != 0) {
                fossil_dir_close(&dir);
                return errno;
            }
        }
    }

    fossil_dir_close(&dir);

    return (fossil_io_rmdir(path) == 0) ? 0 : errno;
}

int fish_delete(const char *target, bool force, bool interactive) {
    if (!target || !*target) {
        fossil_io_printf("{red,bold}Error:{normal} Invalid delete target.\n");
        return FOSSIL_MEDIA_FSON_ERR_INVALID_ARG;
    }

    // --- Validate existence ---
    if (!fossil_io_exists(target)) {
        fossil_io_printf("{red,bold}Error:{normal} Target '%s' does not exist.\n", target);
        return ENOENT;
    }

    // --- Interactive confirmation ---
    if (interactive && !force) {
        char answer[8];
        fossil_io_printf(
            "{yellow,bold}Confirm delete:{normal} Remove '%s'? [y/N]: ",
            target
        );

        // Simple raw read from stdin
        if (!fgets(answer, sizeof(answer), stdin)) {
            fossil_io_printf("{red,bold}Error:{normal} Input failure.\n");
            return EIO;
        }

        if (!(answer[0] == 'y' || answer[0] == 'Y')) {
            fossil_io_printf("{yellow}Aborted.{normal}\n");
            return FOSSIL_MEDIA_FSON_OK;
        }
    }

    // --- Perform deletion ---
    int rc = delete_recursive(target);
    if (rc != 0) {
        fossil_io_printf(
            "{red,bold}Error:{normal} Failed to delete '%s': %s\n",
            target,
            strerror(rc)
        );
        return rc;
    }

    fossil_io_printf(
        "{green,bold}Deleted:{normal} '%s' removed successfully.\n",
        target
    );
    return FOSSIL_MEDIA_FSON_OK;
}
