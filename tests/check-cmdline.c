/*
 * This file is part of clr-boot-manager.
 *
 * Copyright © 2016-2018 Intel Corporation
 *
 * clr-boot-manager is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 */

#define _GNU_SOURCE
#include <check.h>
#include <stdbool.h>
#include <stdlib.h>

#include "cmdline.h"
#include "log.h"
#include "util.h"

START_TEST(cbm_cmdline_test_comments)
{
        const char *file = TOP_DIR "/tests/data/cmdline/comments";
        autofree(char) *p = NULL;

        p = cbm_parse_cmdline_file(file);
        fail_if(!p, "Failed to parse cmdline file");
        fail_if(!streq(p, "init=/bin/bash"), "Comments file does not match");
}
END_TEST

START_TEST(cbm_cmdline_test_mangledmess)
{
        const char *file = TOP_DIR "/tests/data/cmdline/mangledmess";
        autofree(char) *p = NULL;

        p = cbm_parse_cmdline_file(file);
        fail_if(!p, "Failed to parse cmdline file");
        fail_if(!streq(p, "init=/bin/bash rw i8042.nomux thing=off"),
                "Mangled file does not match");
}
END_TEST

START_TEST(cbm_cmdline_test_multi)
{
        const char *file = TOP_DIR "/tests/data/cmdline/multi";
        autofree(char) *p = NULL;

        p = cbm_parse_cmdline_file(file);
        fail_if(!p, "Failed to parse cmdline file");
        fail_if(!streq(p, "one two three"), "Multi file does not match");
}
END_TEST

START_TEST(cbm_cmdline_test_oneline)
{
        const char *file = TOP_DIR "/tests/data/cmdline/oneline";

        autofree(char) *p = NULL;

        p = cbm_parse_cmdline_file(file);
        fail_if(!p, "Failed to parse cmdline file");
        fail_if(!streq(p, "a single line command line file"), "Single file does not match");
}
END_TEST

START_TEST(cbm_cmdline_test_dirs)
{
        const char *dir = TOP_DIR "/tests/data";

        autofree(char) *p = NULL;

        p = cbm_parse_cmdline_files(dir);
        fail_if(!p, "Failed to parse cmdline dirs");
        fail_if(!streq(p, "this is an example of split cmdline handling"),
                "cmdline dirs does not match");
}
END_TEST

START_TEST(cbm_cmdline_test_dirs_vendor_only)
{
        const char *dir = TOP_DIR "/tests/data/cmdline_vendor_only";

        autofree(char) *p = NULL;

        p = cbm_parse_cmdline_files(dir);
        fail_if(!p, "Failed to parse cmdline dirs");
        fail_if(!streq(p, "this is an example of vendor only cmdline handling"),
                "cmdline dirs does not match");
}
END_TEST

START_TEST(cbm_cmdline_test_dirs_vendor_merged)
{
        const char *dir = TOP_DIR "/tests/data/cmdline_vendor_merged";

        autofree(char) *p = NULL;

        p = cbm_parse_cmdline_files(dir);
        fail_if(!p, "Failed to parse cmdline dirs");
        fail_if(!streq(p,
                       "this goes before /etc/ <hi from cmdline> and this one goes last. :) "
                       "overridden with mask"),
                "cmdline dirs does not match");
}
END_TEST

START_TEST(cbm_cmdline_test_delete_middle)
{
        const char *dir = TOP_DIR "/tests/data/cmdline_delete_middle";
        const char *cmdline = "pre init=/bin/bash foobar rw i8042.nomux thing=off one two three a single line command line file post\n";

        autofree(char) *p = strdup(cmdline);

        cbm_parse_cmdline_removal_files_directory(dir, p);
        fail_if(!streq(p, "pre post"), "Delete middle file does not match");
}
END_TEST

START_TEST(cbm_cmdline_test_delete_ends)
{
        const char *dir = TOP_DIR "/tests/data/cmdline_delete_ends";
        const char *cmdline = "one two three four\n";

        autofree(char) *p = strdup(cmdline);

        cbm_parse_cmdline_removal_files_directory(dir, p);
        fail_if(!streq(p, "two three "), "Delete ends does not match");
}
END_TEST

START_TEST(cbm_cmdline_test_delete_all)
{
        const char *dir = TOP_DIR "/tests/data/cmdline_delete_all";
        const char *cmdline = "init=/bin/bash foobar rw i8042.nomux thing=off one two three a single line command line file\n";

        autofree(char) *p = strdup(cmdline);

        cbm_parse_cmdline_removal_files_directory(dir, p);
        fail_if(!streq(p, ""), "Delete all cmdline does not match");
}
END_TEST

static Suite *core_suite(void)
{
        Suite *s = NULL;
        TCase *tc = NULL;

        s = suite_create("cbm_cmdline");
        tc = tcase_create("cbm_cmdline_functions");
        tcase_add_test(tc, cbm_cmdline_test_comments);
        tcase_add_test(tc, cbm_cmdline_test_mangledmess);
        tcase_add_test(tc, cbm_cmdline_test_multi);
        tcase_add_test(tc, cbm_cmdline_test_oneline);
        tcase_add_test(tc, cbm_cmdline_test_dirs);
        tcase_add_test(tc, cbm_cmdline_test_dirs_vendor_only);
        tcase_add_test(tc, cbm_cmdline_test_dirs_vendor_merged);
        tcase_add_test(tc, cbm_cmdline_test_delete_middle);
        tcase_add_test(tc, cbm_cmdline_test_delete_ends);
        tcase_add_test(tc, cbm_cmdline_test_delete_all);
        suite_add_tcase(s, tc);

        return s;
}

int main(void)
{
        Suite *s;
        SRunner *sr;
        int fail;

        /* Ensure that logging is set up properly. */
        setenv("CBM_DEBUG", "1", 1);
        cbm_log_init(stderr);

        /* Turn off the EFI variable manipulation. */
        setenv("CBM_BOOTVAR_TEST_MODE", "yes", 1);

        s = core_suite();
        sr = srunner_create(s);
        srunner_run_all(sr, CK_VERBOSE);
        fail = srunner_ntests_failed(sr);
        srunner_free(sr);

        if (fail > 0) {
                return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
}

/*
 * Editor modelines  -  https://www.wireshark.org/tools/modelines.html
 *
 * Local variables:
 * c-basic-offset: 8
 * tab-width: 8
 * indent-tabs-mode: nil
 * End:
 *
 * vi: set shiftwidth=8 tabstop=8 expandtab:
 * :indentSize=8:tabSize=8:noTabs=true:
 */
