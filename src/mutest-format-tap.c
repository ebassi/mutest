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
  switch (expect->result)
    {
    case MUTEST_RESULT_PASS:
      mutest_print (stdout,
                    "ok ", expect->description,
                    NULL);
      break;

    case MUTEST_RESULT_FAIL:
      mutest_print (stdout,
                    "not ok ", expect->description,
                    NULL);
      break;

    case MUTEST_RESULT_SKIP:
      mutest_print (stdout,
                    "ok ", expect->description, " # SKIP",
                    NULL);
      break;
    }
}

static void
tap_expect_fail (mutest_expect_t *expect,
                 bool negate,
                 mutest_expect_res_t *check,
                 const char *check_repr)
{
  char location[256];
  snprintf (location, 256, "%s (%s:%d)",
            expect->func_name,
            expect->file,
            expect->line);

  char lhs[256], rhs[256], comparison[16];

  mutest_expect_res_to_string (expect->value, lhs, 256);

  if (check != NULL)
    {
      if (check_repr != NULL)
        snprintf (rhs, 256, "%s", check_repr);
      else
        mutest_expect_res_to_string (check, rhs, 256);

      switch (check->expect_type)
        {
        case MUTEST_EXPECT_INVALID:
          snprintf (comparison, 16, " ? ");
          break;
        case MUTEST_EXPECT_BOOLEAN:
        case MUTEST_EXPECT_INT:
        case MUTEST_EXPECT_STR:
        case MUTEST_EXPECT_POINTER:
          snprintf (comparison, 16, " %s ", negate ? "≢" : "≡");
          break;
        case MUTEST_EXPECT_FLOAT:
          snprintf (comparison, 16, " %s ", negate ? "≉" : "≈");
          break;
        case MUTEST_EXPECT_INT_RANGE:
        case MUTEST_EXPECT_FLOAT_RANGE:
          snprintf (comparison, 16, " %s ", negate ? "∉" : "∈");
          break;
        }
    }
  else
    {
      rhs[0] = '\0';
      comparison[0] = '\0';
    }

  mutest_print (stdout,
                "# Assertion failure: ",
                lhs, " ", comparison, " ", rhs,
                " at ", location,
                NULL);
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
tap_total_results (mutest_state_t *state MUTEST_UNUSED)
{
  int n_tests, n_skipped;

  n_tests = mutest_get_results (NULL, NULL, &n_skipped);

  if (n_tests == n_skipped)
    mutest_print (stdout, "1..0 # skip", NULL);
  else
    {
      char plan[128];

      snprintf (plan, 128, "1..%d", n_tests);

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
    .expect_fail = tap_expect_fail,
    .spec_results = NULL,
    .suite_results = NULL,
    .total_results = tap_total_results,
  };

  return &tap;
}
