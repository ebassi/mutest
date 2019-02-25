/* mutest-expect-funcs.c: Comparators
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
#include <math.h>

bool
mutest_not (mutest_expect_t *e MUTEST_UNUSED,
            mutest_expect_res_t *check MUTEST_UNUSED)
{
  return true;
}

bool
mutest_skip (mutest_expect_t *e MUTEST_UNUSED,
             mutest_expect_res_t *check MUTEST_UNUSED)
{
  return true;
}

bool
mutest_to_be_true (mutest_expect_t *e,
                   mutest_expect_res_t *check MUTEST_UNUSED)
{
  if (e->value->expect_type == MUTEST_EXPECT_BOOL_TRUE)
    return e->value->expect.v_bool;

  if (e->value->expect_type == MUTEST_EXPECT_INT)
    return e->value->expect.v_int != 0;

  return false;
}

bool
mutest_to_be_false (mutest_expect_t *e,
                    mutest_expect_res_t *check MUTEST_UNUSED)
{
  if (e->value->expect_type == MUTEST_EXPECT_BOOL_FALSE)
    return !e->value->expect.v_bool;

  if (e->value->expect_type == MUTEST_EXPECT_INT)
    return e->value->expect.v_int == 0;

  return false;
}

bool
mutest_to_be_null (mutest_expect_t *e,
                   mutest_expect_res_t *check MUTEST_UNUSED)
{
  if (e->value->expect_type == MUTEST_EXPECT_POINTER ||
      e->value->expect_type == MUTEST_EXPECT_POINTER_NULL)
    return e->value->expect.v_pointer == NULL;

  return false;
}

bool
mutest_to_be_int_value (mutest_expect_t *e,
                        mutest_expect_res_t *check)
{
  if (e->value->expect_type == MUTEST_EXPECT_INT &&
      check->expect_type == MUTEST_EXPECT_INT)
    return e->value->expect.v_int == check->expect.v_int;

  return false;
}

bool
mutest_to_be_in_int_range (mutest_expect_t *e,
                           mutest_expect_res_t *check)
{
  if (e->value->expect_type == MUTEST_EXPECT_INT &&
      check->expect_type == MUTEST_EXPECT_INT_RANGE)
    {
      if (e->value->expect.v_int >= check->expect.v_irange.min &&
          e->value->expect.v_int <= check->expect.v_irange.max)
        return true;
    }

  return false;
}

static bool
near_equal (double a,
            double b,
            double epsilon)
{
  return (a > b ? (a - b) : (b - a)) <= epsilon;
}

bool
mutest_to_be_float_value (mutest_expect_t *e,
                          mutest_expect_res_t *check)
{
  if (e->value->expect_type == MUTEST_EXPECT_FLOAT &&
      check->expect_type == MUTEST_EXPECT_FLOAT)
    {
      return near_equal (e->value->expect.v_float.value,
                         check->expect.v_float.value,
                         check->expect.v_float.tolerance);
    }

  return false;
}

bool
mutest_to_be_in_float_range (mutest_expect_t *e,
                             mutest_expect_res_t *check)
{
  if (e->value->expect_type == MUTEST_EXPECT_FLOAT &&
      check->expect_type == MUTEST_EXPECT_FLOAT_RANGE)
    {
      if (e->value->expect.v_float.value >= check->expect.v_frange.min &&
          e->value->expect.v_float.value <= check->expect.v_frange.max)
        return true;
    }

  return false;
}

bool
mutest_to_be_string (mutest_expect_t *e,
                     mutest_expect_res_t *check)
{
  if (e->value->expect_type == MUTEST_EXPECT_STR &&
      check->expect_type == MUTEST_EXPECT_STR)
    {
      if (e->value->expect.v_str.len != check->expect.v_str.len)
        return false;

      if (e->value->expect.v_str.str == NULL && check->expect.v_str.str == NULL)
        return true;

      if (e->value->expect.v_str.str != NULL &&
          check->expect.v_str.str != NULL &&
          strcmp (e->value->expect.v_str.str, check->expect.v_str.str) == 0)
        return true;
    }

  return false;
}

bool
mutest_to_contain_string (mutest_expect_t *e,
                          mutest_expect_res_t *check)
{
  if (e->value->expect_type == MUTEST_EXPECT_STR &&
      check->expect_type == MUTEST_EXPECT_STR)
    {
      if (e->value->expect.v_str.len < check->expect.v_str.len)
        return false;

      if (strstr (e->value->expect.v_str.str, check->expect.v_str.str) != NULL)
        return true;
    }

  return false;
}

bool
mutest_to_start_with_string (mutest_expect_t *e,
                             mutest_expect_res_t *check)
{
  if (e->value->expect_type == MUTEST_EXPECT_STR &&
      check->expect_type == MUTEST_EXPECT_STR)
    {
      if (e->value->expect.v_str.len < check->expect.v_str.len)
        return false;

      const char *str = e->value->expect.v_str.str;
      const char *prefix = check->expect.v_str.str;

      size_t prefix_len = check->expect.v_str.len;

      return strncmp (str, prefix, prefix_len) == 0;
    }

  return false;
}

bool
mutest_to_end_with_string (mutest_expect_t *e,
                           mutest_expect_res_t *check)
{
  if (e->value->expect_type == MUTEST_EXPECT_STR &&
      check->expect_type == MUTEST_EXPECT_STR)
    {
      if (e->value->expect.v_str.len < check->expect.v_str.len)
        return false;

      const char *str = e->value->expect.v_str.str;
      const char *suffix = check->expect.v_str.str;

      size_t str_len = e->value->expect.v_str.len;
      size_t suffix_len = check->expect.v_str.len;

      return strcmp (str + str_len - suffix_len, suffix) == 0;
    }

  return false;
}
