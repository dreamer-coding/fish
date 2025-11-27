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
 * @brief Export the dataset to a file.
 * 
 * @param file_path Path to export file.
 * @param format Export format: "csv", "json", "fson", "jelly".
 * @return int Status code.
 */
int fish_dataset_export(ccstring file_path, ccstring format)
{
    if (!file_path || !format) {
        fossil_io_printf("{red,bold}fish_dataset_export: invalid parameters.{normal}\n", "");
        return -1;
    }

    ccstring src_path = "datasets/current.dataset";
    fossil_io_file_t src_stream = {0};
    fossil_io_file_t dst_stream = {0};

    if (fossil_io_file_open(&src_stream, src_path, "r") != 0) {
        fossil_io_printf("{red,bold}fish_dataset_export: no active dataset found.{normal}\n", "");
        return -1;
    }

    if (fossil_io_file_open(&dst_stream, file_path, "w+b") != 0) {
        fossil_io_file_close(&src_stream);
        fossil_io_printf("{red,bold}fish_dataset_export: cannot open output file.{normal}\n", "");
        return -1;
    }

    fossil_sys_memory_t buf = fossil_sys_memory_alloc(4096);

    if (fossil_io_cstring_iequals(format, "csv")) {
        // simple copy for CSV
        while (fgets((char *)buf, 4096, src_stream.file)) {
            fossil_io_file_write(&dst_stream, buf, 1, strlen((char *)buf));
        }
    }
    else if (fossil_io_cstring_iequals(format, "json")) {
        fossil_io_file_write(&dst_stream, "[\n", 1, 2);
        int first = 1;
        while (fgets((char *)buf, 4096, src_stream.file)) {
            if (!first) fossil_io_file_write(&dst_stream, ",\n", 1, 2);
            first = 0;
            ((char *)buf)[strcspn((char *)buf, "\r\n")] = 0; // strip newline
            cstring escaped = fossil_io_cstring_escape_json((char *)buf);
            if (escaped) {
                fossil_sys_memory_t line = fossil_sys_memory_alloc(4096);
                snprintf((char *)line, 4096, "  [\"%s\"]", escaped);
                fossil_io_file_write(&dst_stream, line, 1, strlen((char *)line));
                fossil_sys_memory_free(line);
                fossil_io_cstring_free(escaped);
            } else {
                fossil_io_file_write(&dst_stream, "  [\"\"]", 1, 7);
            }
        }
        fossil_io_file_write(&dst_stream, "\n]\n", 1, 3);
    }
    else if (fossil_io_cstring_iequals(format, "fson")) {
        // minimal binary dump: length-prefixed lines
        while (fgets((char *)buf, 4096, src_stream.file)) {
            size_t len = strcspn((char *)buf, "\r\n");
            fossil_io_file_write(&dst_stream, &len, sizeof(size_t), 1);
            fossil_io_file_write(&dst_stream, buf, 1, len);
        }
    }
    else if (fossil_io_cstring_iequals(format, "jelly")) {
        // Export using jellyfish chain serialization
        fossil_ai_jellyfish_chain_t chain;
        fossil_ai_jellyfish_init(&chain);
        // Load dataset lines as input/output pairs into chain
        while (fgets((char *)buf, 4096, src_stream.file)) {
            ((char *)buf)[strcspn((char *)buf, "\r\n")] = 0; // strip newline
            // For demo: treat line as both input and output
            fossil_ai_jellyfish_learn(&chain, (char *)buf, (char *)buf);
        }
        int rc = fossil_ai_jellyfish_save(&chain, file_path);
        fossil_ai_jellyfish_cleanup(&chain);
        fossil_io_file_close(&src_stream);
        fossil_io_file_close(&dst_stream);
        fossil_sys_memory_free(buf);
        if (rc != 0) {
            fossil_io_printf("{red,bold}fish_dataset_export: jellyfish export failed.{normal}\n", "");
            return -1;
        }
        fossil_io_printf("{green,bold}fish_dataset_export: dataset exported to '%s' as jellyfish chain.{normal}\n", file_path, "");
        return 0;
    }
    else {
        fossil_io_file_close(&src_stream);
        fossil_io_file_close(&dst_stream);
        fossil_sys_memory_free(buf);
        fossil_io_printf("{yellow,bold}fish_dataset_export: unsupported format '%s'.{normal}\n", format, "");
        return -1;
    }

    fossil_io_file_close(&src_stream);
    fossil_io_file_close(&dst_stream);
    fossil_sys_memory_free(buf);
    fossil_io_printf("{green,bold}fish_dataset_export: dataset exported to '%s' as %s.{normal}\n", file_path, format);
    return 0;
}
