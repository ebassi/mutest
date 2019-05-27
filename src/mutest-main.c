/* mutest-main.c: Global state
 *
 * µTest - Copyright 2019  Emmanuele Bassi
 *
 * SPDX-License-Identifier: MIT
 */

#include "mutest-private.h"

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif
#ifdef HAVE_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

static mutest_state_t global_state = {
  .initialized = false,

  .term_width = 80,
  .term_height = 25,
  .is_tty = false,
  .use_colors = false,

  .current_suite = NULL,
  .current_spec = NULL,

  .n_tests = 0,
  .pass = 0,
  .fail = 0,
  .skip = 0,

  .start_time = 0,
  .end_time = 0,
};

mutest_state_t *
mutest_get_global_state (void)
{
  return &global_state;
}

int
mutest_get_term_width (void)
{
  return global_state.term_width;
}

int
mutest_get_term_height (void)
{
  return global_state.term_height;
}

bool
mutest_is_term_tty (void)
{
  return global_state.is_tty;
}

bool
mutest_use_colors (void)
{
  return global_state.use_colors;
}

mutest_suite_t *
mutest_get_current_suite (void)
{
  return global_state.current_suite;
}

mutest_spec_t *
mutest_get_current_spec (void)
{
  return global_state.current_spec;
}

void
mutest_set_current_suite (mutest_suite_t *suite)
{
  if (suite != NULL && global_state.current_suite != NULL)
    mutest_assert_if_reached ("overriding the current suite");

  global_state.current_suite = suite;
}

void
mutest_set_current_spec (mutest_spec_t *spec)
{
  if (spec != NULL && global_state.current_spec != NULL)
    mutest_assert_if_reached ("overriding the current spec");

  global_state.current_spec = spec;
}

void
mutest_add_pass (void)
{
  global_state.n_tests += 1;
  global_state.pass += 1;
}

void
mutest_add_fail (void)
{
  global_state.n_tests += 1;
  global_state.fail += 1;
}

void
mutest_add_skip (void)
{
  global_state.n_tests += 1;
  global_state.skip += 1;
}

static void
update_term_caps (void)
{
#ifdef HAVE_ISATTY
  global_state.is_tty = isatty (STDOUT_FILENO);
#endif

  if (!global_state.is_tty)
    {
      global_state.use_colors = false;
      return;
    }

  const char *env = getenv ("FORCE_COLOR");
  if (env != NULL && env[0] != '\0')
    {
      global_state.use_colors = true;
      return;
    }

  env = getenv ("TERM");
  if (env != NULL)
    {
      if (strncmp (env, "xterm", 5) == 0 ||
          strncmp (env, "rxvt", 4) == 0 ||
          strncmp (env, "vt100", 5) == 0 ||
          strncmp (env, "vt220", 5) == 0 ||
          strncmp (env, "linux", 5) == 0 ||
          strncmp (env, "screen", 6) == 0)
        {
          global_state.use_colors = true;
          return;
        }
    }

  env = getenv ("COLORTERM");
  if (env != NULL)
    {
      global_state.use_colors = true;
      return;
    }

  global_state.use_colors = false;
}

static void
update_term_size (void)
{
  if (!global_state.is_tty)
    return;

#ifdef HAVE_SYS_IOCTL_H
  struct winsize ws;

  if (ioctl (STDOUT_FILENO, TIOCGWINSZ, &ws) != 0)
    perror ("ioctl");

  global_state.term_width = ws.ws_col;
  global_state.term_height = ws.ws_row;
#endif
}

void
mutest_before (mutest_hook_func_t hook)
{
  global_state.before_hook = hook;
}

void
mutest_after (mutest_hook_func_t hook)
{
  global_state.after_hook = hook;
}

void
mutest_init (void)
{
  if (mutest_likely (global_state.initialized))
    return;

  update_term_caps ();
  update_term_size ();

  const char *env = getenv ("MUTEST_OUTPUT");
  if (env != NULL && strcmp (env, "tap") == 0)
    global_state.output_format = MUTEST_OUTPUT_TAP;
  else
    global_state.output_format = MUTEST_OUTPUT_MOCHA;

  global_state.start_time = mutest_get_current_time ();

  global_state.initialized = true;
}

mutest_output_format_t
mutest_get_output_format (void)
{
  return global_state.output_format;
}

int
mutest_report (void)
{
  global_state.end_time = mutest_get_current_time ();

  mutest_print_totals ();

  if (global_state.output_format != MUTEST_OUTPUT_TAP)
    {
      /* The Autotools test harness uses the 77 exit code to signify
       * that all tests have been skipped
       */
      if (global_state.n_tests == global_state.skip)
        return 77;
    }

  return global_state.fail == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
