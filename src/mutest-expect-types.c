#include "config.h"

#include "mutest-private.h"

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

void
mutest_expect_res_free (mutest_expect_res_t *res)
{
  if (res == NULL)
    return;

  switch (res->expect_type)
    {
    case MUTEST_EXPECT_INVALID:
      mutest_assert_if_reached ("invalid value");
      break;

    case MUTEST_EXPECT_BOOL:
    case MUTEST_EXPECT_INT:
    case MUTEST_EXPECT_INT_RANGE:
    case MUTEST_EXPECT_FLOAT:
    case MUTEST_EXPECT_FLOAT_RANGE:
    case MUTEST_EXPECT_POINTER:
    case MUTEST_EXPECT_CLOSURE:
      break;

    case MUTEST_EXPECT_STR:
      free (res->expect.v_str.str);
      break;

    case MUTEST_EXPECT_BYTE_ARRAY:
      break;
    }

  free (res);
}

void
mutest_expect_res_to_string (mutest_expect_res_t *res,
                             char *buf,
                             size_t len)
{
  switch (res->expect_type)
    {
    case MUTEST_EXPECT_INVALID:
      snprintf (buf, len, "invalid");
      break;

    case MUTEST_EXPECT_BOOL:
      snprintf (buf, len, "%s", res->expect.v_bool ? "true" : "false");
      break;

    case MUTEST_EXPECT_INT:
      snprintf (buf, len, "%d", res->expect.v_int);
      break;

    case MUTEST_EXPECT_INT_RANGE:
      snprintf (buf, len, "[ %d, %d ]",
                res->expect.v_irange.min,
                res->expect.v_irange.max);
      break;

    case MUTEST_EXPECT_FLOAT:
      snprintf (buf, len, "%g (± %g)",
                res->expect.v_float.value,
                res->expect.v_float.tolerance);
      break;

    case MUTEST_EXPECT_FLOAT_RANGE:
      snprintf (buf, len, "[ %g, %g ]",
                res->expect.v_frange.min,
                res->expect.v_frange.max);
      break;

    case MUTEST_EXPECT_POINTER:
      snprintf (buf, len, "%p", res->expect.v_pointer);
      break;

    case MUTEST_EXPECT_STR:
      snprintf (buf, len, "%s", res->expect.v_str.str);
      break;

    case MUTEST_EXPECT_CLOSURE:
    case MUTEST_EXPECT_BYTE_ARRAY:
      break;
    }
}

mutest_expect_res_t *
mutest_expect_res_collect_value (mutest_expect_type_t type,
                                 va_list args)
{
  mutest_expect_res_t *retval = malloc (sizeof (mutest_expect_res_t));

  if (retval == NULL)
    mutest_oom_abort ();

  retval->expect_type = type;

  switch (retval->expect_type)
    {
    case MUTEST_EXPECT_INVALID:
      mutest_assert_if_reached ("invalid value");
      break;

    case MUTEST_EXPECT_BOOL:
      break;

    case MUTEST_EXPECT_INT:
      retval->expect.v_int = va_arg (args, int);
      break;

    case MUTEST_EXPECT_INT_RANGE:
      {
        int min_val = va_arg (args, int);
        int max_val = va_arg (args, int);

        if (min_val > max_val)
          mutest_assert_if_reached ("invalid integer range");

        retval->expect.v_irange.min = min_val;
        retval->expect.v_irange.max = max_val;
      }
      break;

    case MUTEST_EXPECT_FLOAT:
      retval->expect.v_float.value = va_arg (args, double);
      retval->expect.v_float.tolerance = va_arg (args, double);
      break;

    case MUTEST_EXPECT_FLOAT_RANGE:
      {
        double min_val = va_arg (args, double);
        double max_val = va_arg (args, double);

        if (min_val > max_val)
          mutest_assert_if_reached ("invalid floating point range");

        retval->expect.v_frange.min = min_val;
        retval->expect.v_frange.max = max_val;
      }
      break;

    case MUTEST_EXPECT_STR:
      retval->expect.v_str.str = va_arg (args, char *);
      retval->expect.v_str.str = strdup (retval->expect.v_str.str);
      retval->expect.v_str.len = strlen (retval->expect.v_str.str);
      break;

    case MUTEST_EXPECT_POINTER:
      retval->expect.v_pointer = va_arg (args, void *);
      break;

    case MUTEST_EXPECT_BYTE_ARRAY:
    case MUTEST_EXPECT_CLOSURE:
      mutest_assert_if_reached ("not implemented");
      break;
    }

  return retval;
}

mutest_expect_res_t *
mutest_bool_value (bool value)
{
  mutest_expect_res_t *res = malloc (sizeof (mutest_expect_res_t));
  if (res == NULL)
    mutest_oom_abort ();

  res->expect_type = MUTEST_EXPECT_BOOL;
  res->expect.v_bool = value;

  return res;
}

mutest_expect_res_t *
mutest_int_value (int value)
{
  mutest_expect_res_t *res = malloc (sizeof (mutest_expect_res_t));
  if (res == NULL)
    mutest_oom_abort ();

  res->expect_type = MUTEST_EXPECT_INT;
  res->expect.v_int = value;

  return res;
}

mutest_expect_res_t *
mutest_string_value (const char *value)
{
  mutest_expect_res_t *res = malloc (sizeof (mutest_expect_res_t));
  if (res == NULL)
    mutest_oom_abort ();

  res->expect_type = MUTEST_EXPECT_STR;
  res->expect.v_str.str = strdup (value);
  if (res->expect.v_str.str == NULL)
    mutest_oom_abort ();

  res->expect.v_str.len = strlen (value);

  return res;
}

mutest_expect_res_t *
mutest_float_value (double value,
                    double tolerance)
{
  mutest_expect_res_t *res = malloc (sizeof (mutest_expect_res_t));
  if (res == NULL)
    mutest_oom_abort ();

  res->expect_type = MUTEST_EXPECT_FLOAT;
  res->expect.v_float.value = value;
  res->expect.v_float.tolerance = tolerance;

  return res;
}

mutest_expect_res_t *
mutest_int_range (int min,
                  int max)
{
  mutest_expect_res_t *res = malloc (sizeof (mutest_expect_res_t));
  if (res == NULL)
    mutest_oom_abort ();

  res->expect_type = MUTEST_EXPECT_INT_RANGE;
  res->expect.v_irange.min = min;
  res->expect.v_irange.max = max;

  return res;
}

mutest_expect_res_t *
mutest_float_range (double min,
                    double max)
{
  mutest_expect_res_t *res = malloc (sizeof (mutest_expect_res_t));
  if (res == NULL)
    mutest_oom_abort ();

  res->expect_type = MUTEST_EXPECT_FLOAT_RANGE;
  res->expect.v_frange.min = min;
  res->expect.v_frange.max = max;

  return res;
}

mutest_expect_res_t *
mutest_pointer (void *pointer)
{
  mutest_expect_res_t *res = malloc (sizeof (mutest_expect_res_t));
  if (res == NULL)
    mutest_oom_abort ();

  res->expect_type = MUTEST_EXPECT_POINTER;
  res->expect.v_pointer = pointer;

  return res;
}

#if 0
mutest_expect_res_t *
mutest_byte_array (void *data,
                   size_t element_size,
                   size_t length)
{
  mutest_expect_res_t *res = malloc (sizeof (mutest_expect_res_t));
  if (res == NULL)
    mutest_oom_abort ();

  res->expect_type = MUTEST_EXPECT_BYTE_ARRAY;
  res->expect.v_bytearray.data = NULL;
  res->expect.v_bytearray.element_size = element_size;
  res->expect.v_bytearray.length = length;

  if (element_size == 0 || length == 0)
    return res;

  size_t max_size = (size_t) -1;

  if (element_size > 0 && length > max_size / element_size)
    mutest_assert_if_reached ("overflow");

  size_t total_size = element_size * length;

  res->expect.v_bytearray.data = malloc (total_size);
  if (res->expect.v_bytearray.data == NULL)
    mutest_oom_abort ();

  memcpy (res->expect.v_bytearray.data, data, total_size);

  return res;
}

mutest_expect_res_t *
mutest_closure (mutest_expect_closure_func_t func,
                void *data)
{
  mutest_expect_res_t *res = malloc (sizeof (mutest_expect_res_t));
  if (res == NULL)
    mutest_oom_abort ();

  res->expect_type = MUTEST_EXPECT_CLOSURE;
  res->expect.v_closure.func = func;
  res->expect.v_closure.data = data;

  return res;
}
#endif
