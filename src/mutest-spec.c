/* mutest-spec.c: Specifications
 *
 * µTest - Copyright 2019  Emmanuele Bassi
 *
 * SPDX-License-Identifier: MIT
 */

#include "config.h"

#include "mutest-private.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void
mutest_it_full (const char *file,
                int line,
                const char *func_name,
                const char *description,
                mutest_spec_func_t func)
{
  if (description == NULL)
    mutest_assert_if_reached ("missing spec description");

  if (func == NULL)
    mutest_assert_if_reached ("missing spec functions");

  if (mutest_get_current_suite () == NULL)
    mutest_assert_if_reached ("missing suite");

  mutest_spec_t spec = {
    .description = description,
    .file = file,
    .line = line,
    .func_name = func_name,
  };

  mutest_print_spec_preamble (&spec);

  mutest_set_current_spec (&spec);

  mutest_suite_t *suite = mutest_get_current_suite ();

  if (suite->before_each_hook != NULL)
    suite->before_each_hook ();

  spec.start_time = mutest_get_current_time (),

  func (&spec);

  spec.end_time = mutest_get_current_time ();

  if (suite->after_each_hook != NULL)
    suite->after_each_hook ();

  mutest_set_current_spec (NULL);

  mutest_print_spec_totals (&spec);
}

void
mutest_spec_add_result (mutest_spec_t *spec,
                        mutest_expect_t *expect)
{
  switch (expect->result)
    {
    case MUTEST_RESULT_PASS:
      spec->n_tests += 1;
      spec->pass += 1;
      mutest_add_pass ();
      break;

    case MUTEST_RESULT_FAIL:
      spec->n_tests += 1;
      spec->fail += 1;
      mutest_add_fail ();
      break;

    case MUTEST_RESULT_SKIP:
      spec->n_tests += 1;
      spec->skip += 1;
      mutest_add_skip ();
      break;
    }
}
