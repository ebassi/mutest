/* mutest-expect.c: Expected result
 *
 * µTest - Copyright 2019  Emmanuele Bassi
 *
 * SPDX-License-Identifier: MIT
 */

#include "config.h"

#include "mutest-private.h"

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

#define EXPECT_FUNC(func, collect, type) \
  { .expect_name = #func, \
    .expect_func = func, \
    .collect_value = collect, \
    .collect_type = type, \
  }

static const struct {
  const char *expect_name;
  mutest_expect_func_t expect_func;
  bool collect_value;
  mutest_expect_type_t collect_type;
} collectors[] = {
  EXPECT_FUNC (mutest_to_be_true, false, MUTEST_EXPECT_INVALID),
  EXPECT_FUNC (mutest_to_be_false, false, MUTEST_EXPECT_INVALID),
  EXPECT_FUNC (mutest_to_be_null, false, MUTEST_EXPECT_INVALID),
  EXPECT_FUNC (mutest_to_be_int_value, true, MUTEST_EXPECT_INT),
  EXPECT_FUNC (mutest_to_be_in_int_range, true, MUTEST_EXPECT_INT_RANGE),
  EXPECT_FUNC (mutest_to_be_float_value, true, MUTEST_EXPECT_FLOAT),
  EXPECT_FUNC (mutest_to_be_in_float_range, true, MUTEST_EXPECT_FLOAT_RANGE),
  EXPECT_FUNC (mutest_to_be_string, true, MUTEST_EXPECT_STR),
  EXPECT_FUNC (mutest_to_contain_string, true, MUTEST_EXPECT_STR),
  EXPECT_FUNC (mutest_to_start_with_string, true, MUTEST_EXPECT_STR),
  EXPECT_FUNC (mutest_to_end_with_string, true, MUTEST_EXPECT_STR),
};

static const size_t n_collectors = sizeof (collectors) / sizeof (collectors[0]);

void
mutest_expect_full (const char *file,
                    int line,
                    const char *func_name,
                    const char *description,
                    mutest_expect_res_t *value,
                    mutest_expect_func_t first_expect_func,
                    ...)
{
  if (description == NULL)
    mutest_assert_if_reached ("invalid description");

  if (value == NULL)
    mutest_assert_if_reached ("invalid data pointer");

  if (first_expect_func == NULL)
    mutest_assert_if_reached ("invalid expect function");

  mutest_expect_func_t expect_func = first_expect_func;

  mutest_expect_t e = {
    .description = description,
    .value = value,
    .file = file,
    .line = line,
    .func_name = func_name,
    .result = MUTEST_RESULT_PASS,
  };

  va_list args;

  va_start (args, first_expect_func);

  while (expect_func != NULL)
    {
      mutest_expect_res_t *check = NULL;
      bool negate = false;

      if (expect_func == mutest_skip)
        {
          e.result = MUTEST_RESULT_SKIP;
          break;
        }

      if (expect_func == mutest_not)
        {
          negate = true;
          expect_func = va_arg (args, void *);
        }

      for (size_t i = 0; i < n_collectors; i++)
        {
          if (collectors[i].expect_func == expect_func &&
              collectors[i].collect_value)
            {
              check = mutest_expect_res_collect_value (collectors[i].collect_type, args);
              break;
            }
        }

      bool res = expect_func (&e, check);

      res = negate ? !res : res;

      if (!res)
        mutest_print_expect_fail (&e, negate, check);

      if (e.result == MUTEST_RESULT_PASS)
        e.result = res ? MUTEST_RESULT_PASS : MUTEST_RESULT_FAIL;

      mutest_expect_res_free (check);

      expect_func = va_arg (args, void *);
    }

  mutest_spec_add_result (mutest_get_current_spec (), &e);

  mutest_print_expect (&e);

  va_end (args);
}
