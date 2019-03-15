/* mutest-utils.c: Utilities
 *
 * µTest - Copyright 2019  Emmanuele Bassi
 *
 * SPDX-License-Identifier: MIT
 */

#include "config.h"

#include "mutest-private.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef OS_WINDOWS
#include <windows.h>
#endif

#define ANSI_ESCAPE             "\033"

#define MUTEST_COLOR_GREEN      ANSI_ESCAPE "[1;32m"
#define MUTEST_COLOR_BLUE       ANSI_ESCAPE "[1;34m"
#define MUTEST_COLOR_YELLOW     ANSI_ESCAPE "[1;33m"
#define MUTEST_COLOR_RED        ANSI_ESCAPE "[1;31m"
#define MUTEST_COLOR_NONE       ANSI_ESCAPE "[0m"
#define MUTEST_COLOR_LIGHT_GREY ANSI_ESCAPE "[1;37m"

#define MUTEST_BOLD_DEFAULT             ANSI_ESCAPE "[1;39m"
#define MUTEST_UNDERLINE_DEFAULT        ANSI_ESCAPE "[4;39m"
#define MUTEST_DIM_DEFAULT              ANSI_ESCAPE "[2;39m"

#if defined(OS_WINDOWS) && !defined(STDOUT_FILENO)
# define STDOUT_FILENO _fileno(stdout)
#endif

#if defined(OS_WINDOWS) && !defined(STDERR_FILENO)
# define STDERR_FILENO _fileno(stderr)
#endif

// mutest_get_current_time:
//
// Returns: the current time, in microseconds
#ifdef HAVE_QUERY_PERFORMANCE_COUNTER
static double usec_per_tick;

int64_t
mutest_get_current_time (void)
{
  if (mutest_unlikely (usec_per_tick == 0))
    {
      LARGE_INTEGER freq;

      if (!QueryPerformanceFrequency (&freq) || freq.QuadPart == 0)
        mutest_assert_if_reached ("QueryPerformanceFrequency failed");

      usec_per_tick = (double) 1000000 / freq.QuadPart;
    }

  LARGE_INTEGER ticks;

  if (QueryPerformanceCounter (&ticks))
    return (int64_t) (ticks.QuadPart * usec_per_tick);

  return 0;
}
#elif defined(HAVE_CLOCK_GETTIME)
int64_t
mutest_get_current_time (void)
{
  struct timespec ts;
  int res;

  res = clock_gettime (CLOCK_MONOTONIC, &ts);

  if (res != 0)
    return 0;

  return (((int64_t) ts.tv_sec) * 1000000) + (ts.tv_nsec / 1000);
}
#else
# error "muTest requires a monotonic clock implementation"
#endif

void
mutest_print (int fd,
              const char *first_fragment,
              ...)
{
  va_list args;

  va_start (args, first_fragment);
  
  const char *fragment = first_fragment;
  while (fragment != NULL)
    {
      if (fragment[0] != '\0')
        write (fd, fragment, strlen (fragment));

      fragment = va_arg (args, char *);
    }

  va_end (args);

  write (fd, "\n", 1);
}

void
mutest_assert_message (const char *file,
                       int         line,
                       const char *func,
                       const char *message)
{
  char lstr[32];

  /* We can be called by an OOM handler, or a signal handler,
   * so we should not:
   *
   *  - allocate memory
   *  - re-enter into mutest code
   */
  snprintf (lstr, 32, "%d", line);

  if (mutest_use_colors ())
    mutest_print (STDERR_FILENO,
                  MUTEST_COLOR_RED, "ERROR", MUTEST_COLOR_NONE, ": ",
                  file, ":", lstr, ":", func != NULL ? func : "<local>",
                  " ",
                  message,
                  NULL);
  else
    mutest_print (STDERR_FILENO,
                  "ERROR: ", file, ":", lstr, ":", func != NULL ? func : "<local>", " ",
                  message,
                  NULL);

  abort ();
}

static double
format_time (int64_t t,
             const char **unit)
{
  if (t > 1000000)
    {
      *unit = "s";
      return (double) t / 1000000.0;
    }

  if (t > 1000)
    {
      *unit = "ms";
      return (double) t / 1000.0;
    }

  *unit = "µs";
  return (double) t;
}

static void
mocha_suite_preamble (mutest_suite_t *suite)
{
  if (mutest_use_colors ())
    mutest_print (STDOUT_FILENO,
                  "\n",
                  "  ",
                  MUTEST_BOLD_DEFAULT, suite->description, MUTEST_COLOR_NONE,
                  NULL);
  else
    mutest_print (STDOUT_FILENO,
                  "\n",
                  "  ", suite->description,
                  NULL);
}

static void
mocha_expect_result (mutest_expect_t *expect)
{
  switch (expect->result)
    {
    case MUTEST_RESULT_PASS:
      if (mutest_use_colors ())
        mutest_print (STDOUT_FILENO,
                      "     ",
                      MUTEST_COLOR_GREEN, " ✓ ", MUTEST_DIM_DEFAULT, expect->description,
                      MUTEST_COLOR_NONE,
                      NULL);
      else
        mutest_print (STDOUT_FILENO,
                      "      ✓ ", expect->description,
                      NULL);
      break;

    case MUTEST_RESULT_FAIL:
      if (mutest_use_colors ())
        mutest_print (STDOUT_FILENO,
                      "     ",
                      MUTEST_COLOR_RED, " ✗ ", expect->description, MUTEST_COLOR_NONE,
                      NULL);
      else
        mutest_print (STDOUT_FILENO,
                      "      ✗ ", expect->description,
                      NULL);
      break;

    case MUTEST_RESULT_SKIP:
      if (mutest_use_colors ())
        mutest_print (STDOUT_FILENO,
                      "     ",
                      MUTEST_COLOR_YELLOW, " - ", expect->description, MUTEST_COLOR_NONE,
                      NULL);
      else
        mutest_print (STDOUT_FILENO,
                      "      - ", expect->description,
                      NULL);
      break;
    }
}

static void
mocha_spec_preamble (mutest_spec_t *spec)
{
  mutest_print (STDOUT_FILENO,
                "    ",
                spec->description,
                NULL);
}

static void
mocha_spec_results (mutest_spec_t *spec)
{
  char passing_s[128], failing_s[128], skipped_s[128];

  snprintf (passing_s, 128, "%d passing", spec->pass);
  snprintf (failing_s, 128, "%d failing", spec->fail);
  snprintf (skipped_s, 128, "%d skipped", spec->skip);

  const char *delta_u;
  double delta_t;
  char delta_s[128];

  delta_t = format_time (spec->end_time - spec->start_time, &delta_u);
  snprintf (delta_s, 128, "(%.2f %s)", delta_t, delta_u);

  if (mutest_use_colors ())
    {
      mutest_print (STDOUT_FILENO,
                    "\n",
                    "      ",
                    MUTEST_COLOR_GREEN, passing_s, MUTEST_COLOR_NONE, " ",
                    MUTEST_COLOR_LIGHT_GREY, delta_s, MUTEST_COLOR_NONE,
                    NULL);

      if (spec->skip != 0)
        mutest_print (STDOUT_FILENO,
                      "      ",
                      MUTEST_COLOR_YELLOW, skipped_s, MUTEST_COLOR_NONE,
                      NULL);

      if (spec->fail != 0)
        mutest_print (STDOUT_FILENO,
                      "      ",
                      MUTEST_COLOR_RED, failing_s, MUTEST_COLOR_NONE,
                      NULL);

      mutest_print (STDOUT_FILENO, "", NULL);
    }
  else
    mutest_print (STDOUT_FILENO,
                  "\n",
                  "      ", passing_s, " ", delta_s, "\n",
                  "      ", skipped_s, "\n",
                  "      ", failing_s, "\n",
                  NULL);
}

static void
mocha_suite_results (mutest_suite_t *state MUTEST_UNUSED)
{
}

static void
mocha_total_results (mutest_state_t *state)
{
  char passing_s[128], failing_s[128], skipped_s[128];

  snprintf (passing_s, 128, "%d passing", state->pass);
  snprintf (failing_s, 128, "%d failing", state->fail);
  snprintf (skipped_s, 128, "%d skipped", state->skip);

  const char *delta_u;
  double delta_t;
  char delta_s[128];

  delta_t = format_time (state->end_time - state->start_time, &delta_u);
  snprintf (delta_s, 128, "(%.2f %s)", delta_t, delta_u);

  if (mutest_use_colors ())
    {
      mutest_print (STDOUT_FILENO,
                    "\n",
                    MUTEST_UNDERLINE_DEFAULT, "Total", MUTEST_COLOR_NONE, "\n",
                    MUTEST_COLOR_GREEN, passing_s, MUTEST_COLOR_NONE, " ",
                      MUTEST_COLOR_LIGHT_GREY, delta_s, MUTEST_COLOR_NONE,
                    NULL);

      if (state->skip != 0)
        mutest_print (STDOUT_FILENO,
                      MUTEST_COLOR_YELLOW, skipped_s, MUTEST_COLOR_NONE,
                      NULL);

      if (state->fail != 0)
        mutest_print (STDOUT_FILENO,
                      MUTEST_COLOR_RED, failing_s, MUTEST_COLOR_NONE,
                      NULL);

      mutest_print (STDOUT_FILENO, "", NULL);
    }
  else
    mutest_print (STDOUT_FILENO,
                  "\n",
                  "  Total\n",
                  "  ", passing_s, " ", delta_s, "\n",
                  "  ", skipped_s, "\n",
                  "  ", failing_s, "\n",
                  NULL);
}

static void
tap_expect_result (mutest_expect_t *expect)
{
  mutest_state_t *state = mutest_get_global_state ();

  char num[32];

  snprintf (num, 32, "%d", state->n_tests);

  switch (expect->result)
    {
    case MUTEST_RESULT_PASS:
      mutest_print (STDOUT_FILENO,
                    "ok ", num, " ", expect->description,
                    NULL);
      break;

    case MUTEST_RESULT_FAIL:
      mutest_print (STDOUT_FILENO,
                    "not ok ", num, " ", expect->description,
                    NULL);
      break;

    case MUTEST_RESULT_SKIP:
      mutest_print (STDOUT_FILENO,
                    "ok ", num, " # skip: ", expect->description,
                    NULL);
      break;
    }
}

static void
tap_spec_preamble (mutest_spec_t *spec)
{
  mutest_print (STDOUT_FILENO, "# ", spec->description, NULL);
}

static void
tap_suite_preamble (mutest_suite_t *suite)
{
  mutest_print (STDOUT_FILENO, "# ", suite->description, NULL);
}

static void
tap_total_results (mutest_state_t *state)
{
  if (state->n_tests == state->skip)
    mutest_print (STDOUT_FILENO, "1..0 # skip", NULL);
  else
    {
      char plan[128];

      snprintf (plan, 128, "1..%d", state->n_tests);

      mutest_print (STDOUT_FILENO, plan, NULL);
    }
}

static struct {
  void (* suite_preamble) (mutest_suite_t *suite);
  void (* spec_preamble) (mutest_spec_t *spec);
  void (* expect_result) (mutest_expect_t *expect);
  void (* spec_results) (mutest_spec_t *spec);
  void (* suite_results) (mutest_suite_t *suite);
  void (* total_results) (mutest_state_t *state);
} output_formatters[] = {
  [MUTEST_OUTPUT_MOCHA] = {
    .suite_preamble = mocha_suite_preamble,
    .spec_preamble = mocha_spec_preamble,
    .expect_result = mocha_expect_result,
    .spec_results = mocha_spec_results,
    .suite_results = mocha_suite_results,
    .total_results = mocha_total_results,
  },
  [MUTEST_OUTPUT_TAP] = {
    .suite_preamble = tap_suite_preamble,
    .spec_preamble = tap_spec_preamble,
    .expect_result = tap_expect_result,
    .spec_results = NULL,
    .suite_results = NULL,
    .total_results = tap_total_results,
  },
};

void
mutest_print_suite_preamble (mutest_suite_t *suite)
{
  mutest_output_format_t format = mutest_get_output_format ();

  if (output_formatters[format].suite_preamble != NULL)
    output_formatters[format].suite_preamble (suite);
}

void
mutest_print_expect (mutest_expect_t *expect)
{
  mutest_output_format_t format = mutest_get_output_format ();

  if (output_formatters[format].expect_result != NULL)
    output_formatters[format].expect_result (expect);
}

void
mutest_print_expect_fail (mutest_expect_t *expect,
                          bool negate,
                          mutest_expect_res_t *check,
                          const char *check_repr)
{
  mutest_output_format_t format = mutest_get_output_format ();

  if (format != MUTEST_OUTPUT_MOCHA)
    return;

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

  if (mutest_use_colors ())
    {
      mutest_print (STDOUT_FILENO,
                    "      ",
                    MUTEST_COLOR_RED, "Assertion failure: ",
                    lhs, comparison, rhs,
                    " at ", location, MUTEST_COLOR_NONE,
                    NULL);
    }
  else
    {
      mutest_print (STDOUT_FILENO,
                    "      ",
                    "Assertion failure: ",
                    lhs, " ", comparison, " ", rhs,
                    " at ", location,
                    NULL);
    }
}

void
mutest_print_spec_preamble (mutest_spec_t *spec)
{
  mutest_output_format_t format = mutest_get_output_format ();

  if (output_formatters[format].spec_preamble != NULL)
    output_formatters[format].spec_preamble (spec);
}

void
mutest_print_spec_totals (mutest_spec_t *spec)
{
  mutest_output_format_t format = mutest_get_output_format ();

  if (output_formatters[format].spec_results != NULL)
    output_formatters[format].spec_results (spec);
}

void
mutest_print_suite_totals (mutest_suite_t *suite)
{
  mutest_output_format_t format = mutest_get_output_format ();

  if (output_formatters[format].suite_results != NULL)
    output_formatters[format].suite_results (suite);
}

void
mutest_print_totals (void)
{
  mutest_state_t *state = mutest_get_global_state ();
  mutest_output_format_t format = mutest_get_output_format ();

  if (output_formatters[format].total_results != NULL)
    output_formatters[format].total_results (state);
}
