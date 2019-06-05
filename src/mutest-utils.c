/* mutest-utils.c: Utilities
 *
 * µTest - Copyright 2019  Emmanuele Bassi
 *
 * SPDX-License-Identifier: MIT
 */

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
#include <io.h>
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

char *
mutest_strdup (const char *str)
{
  if (str == NULL)
    return NULL;

  size_t len = strlen (str) + 1;
  char *res = malloc (len * sizeof (char));
  if (res == NULL)
    mutest_oom_abort ();

  memcpy (res, str, len * sizeof (char));

  return res;
}

char *
mutest_getenv (const char *str)
{
#ifdef HAVE__DUPENV_S
  size_t len = 0;
  char *res = NULL;

  errno_t err = _dupenv_s (&res, &len, str);
  if (err)
    return NULL;

  return res;
#else
  return mutest_strdup (getenv (str));
#endif
}

void
mutest_print (FILE *stream,
              const char *first_fragment,
              ...)
{
  va_list args;

  va_start (args, first_fragment);
  
  const char *fragment = first_fragment;
  while (fragment != NULL)
    {
#ifdef OS_WINDOWS
      if (fragment[0] != '\0')
        fprintf (stream, "%s", fragment);
#else
      if (fragment[0] != '\0')
        write (fileno (stream), fragment, strlen (fragment));
#endif

      fragment = va_arg (args, char *);
    }

  va_end (args);

#ifdef OS_WINDOWS
  fputc ('\n', stream);
#else
  write (fileno (stream), "\n", 1);
#endif
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
    mutest_print (stderr,
                  MUTEST_COLOR_RED, "ERROR", MUTEST_COLOR_NONE, ": ",
                  file, ":", lstr, ":", func != NULL ? func : "<local>",
                  " ",
                  message,
                  NULL);
  else
    mutest_print (stderr,
                  "ERROR: ", file, ":", lstr, ":", func != NULL ? func : "<local>", " ",
                  message,
                  NULL);

  abort ();
}

double
mutest_format_time (int64_t t,
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

static struct {
  const mutest_formatter_t *(* get_formatter) (void);
  const char *formatter;
} output_formatters[] = {
  [MUTEST_OUTPUT_MOCHA] = {
    .get_formatter = mutest_get_mocha_formatter,
    .formatter = "mocha",
  },
  [MUTEST_OUTPUT_TAP] = {
    .get_formatter = mutest_get_tap_formatter,
    .formatter = "TAP",
  },
};

static const mutest_formatter_t *
mutest_get_formatter (void)
{
  mutest_output_format_t format = mutest_get_output_format ();

  return output_formatters[format].get_formatter ();
}

void
mutest_format_spec_preamble (mutest_spec_t *spec)
{
  const mutest_formatter_t *vtable = mutest_get_formatter ();

  if (vtable->spec_preamble != NULL)
    vtable->spec_preamble (spec);
}

void
mutest_format_spec_results (mutest_spec_t *spec)
{
  const mutest_formatter_t *vtable = mutest_get_formatter ();

  if (vtable->spec_results != NULL)
    vtable->spec_results (spec);
}

void
mutest_format_suite_results (mutest_suite_t *suite)
{
  const mutest_formatter_t *vtable = mutest_get_formatter ();

  if (vtable->suite_results != NULL)
    vtable->suite_results (suite);
}

void
mutest_format_total_results (mutest_state_t *state)
{
  const mutest_formatter_t *vtable = mutest_get_formatter ();

  if (vtable->total_results != NULL)
    vtable->total_results (state);
}

void
mutest_format_suite_preamble (mutest_suite_t *suite)
{
  const mutest_formatter_t *vtable = mutest_get_formatter ();

  if (vtable->suite_preamble != NULL)
    vtable->suite_preamble (suite);
}

void
mutest_format_expect_fail (mutest_expect_t *expect,
                           bool negate,
                           mutest_expect_res_t *check,
                           const char *check_repr)
{
  const mutest_formatter_t *vtable = mutest_get_formatter ();

  if (vtable->expect_fail != NULL)
    vtable->expect_fail (expect, negate, check, check_repr);
}

void
mutest_format_expect_result (mutest_expect_t *expect)
{
  const mutest_formatter_t *vtable = mutest_get_formatter ();

  if (vtable->expect_result != NULL)
    vtable->expect_result (expect);
}
