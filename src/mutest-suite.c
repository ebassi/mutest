#include "config.h"

#include "mutest-private.h"

#include <string.h>
#include <stdlib.h>

// mutest_describe:
// @description: The human readable description of the test suite
//   used when writing the report
// @func: The test suite function
//
// Describes a new test suite, represented by the given @func.
//
// The #mutest_describe_func_t function is responsible for adding
// specifications that need to be satisfied, e.g.:
//
// ```cpp
// static void
// string_tests (mutest_suite_t *suite)
// {
//   /* string_start_end() is defined elsewhere */
//   mutest_it ("must check start and end", string_start_end);
//
//   /* string_contains() is defined elsewhere */
//   mutest_it ("must containt substring", string_contains);
// }
//
// int
// main (int argc, char *argv[])
// {
//   mutest_describe ("string tests", string_tests);
//
//   return mutest_run (argc, argv);
// }
// ```
//
// It's not possible to call mutest_describe() from within a
// another test suite.
void
mutest_describe_full (const char *file,
                      int line,
                      const char *func_name,
                      const char *description,
                      mutest_describe_func_t func)
{
  if (description == NULL)
    mutest_assert_if_reached ("missing spec description");

  if (func == NULL)
    mutest_assert_if_reached ("missing spec functions");

  if (mutest_get_current_suite () != NULL)
    mutest_assert_if_reached ("nested suite");

  mutest_init ();

  mutest_suite_t suite = {
    .file = file,
    .line = line,
    .func_name = func_name,
    .description = description,
  };

  mutest_set_current_suite (&suite);

  mutest_print_suite_preamble (&suite);

  func (&suite);

  mutest_set_current_suite (NULL);
}
