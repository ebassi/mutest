/* mutest-format-tap.c: TAP format output
 *
 * µTest - Copyright 2019  Emmanuele Bassi
 *
 * SPDX-License-Identifier: MIT
 */

#include "mutest-private.h"

static void
tap_expect_result (mutest_expect_t *expect)
{
  mutest_state_t *state = mutest_get_global_state ();

  char num[32];

  snprintf (num, 32, "%d", state->n_tests);

  switch (expect->result)
    {
    case MUTEST_RESULT_PASS:
      mutest_print (stdout,
                    "ok ", num, " ", expect->description,
                    NULL);
      break;

    case MUTEST_RESULT_FAIL:
      mutest_print (stdout,
                    "not ok ", num, " ", expect->description,
                    NULL);
      break;

    case MUTEST_RESULT_SKIP:
      mutest_print (stdout,
                    "ok ", num, " # skip: ", expect->description,
                    NULL);
      break;
    }
}

static void
tap_spec_preamble (mutest_spec_t *spec)
{
  mutest_print (stdout, "# ", spec->description, NULL);
}

static void
tap_suite_preamble (mutest_suite_t *suite)
{
  mutest_print (stdout, "# ", suite->description, NULL);
}

static void
tap_total_results (mutest_state_t *state)
{
  if (state->n_tests == state->skip)
    mutest_print (stdout, "1..0 # skip", NULL);
  else
    {
      char plan[128];

      snprintf (plan, 128, "1..%d", state->n_tests);

      mutest_print (stdout, plan, NULL);
    }
}

const mutest_formatter_t *
mutest_get_tap_formatter (void)
{
  static mutest_formatter_t tap = {
    .suite_preamble = tap_suite_preamble,
    .spec_preamble = tap_spec_preamble,
    .expect_result = tap_expect_result,
    .expect_fail = NULL,
    .spec_results = NULL,
    .suite_results = NULL,
    .total_results = tap_total_results,
  };

  return &tap;
}
