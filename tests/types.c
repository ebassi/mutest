#include <mutest.h>

static void
check_scalars (mutest_spec_t *spec MUTEST_UNUSED)
{
  mutest_expect ("booleans",
                 mutest_bool_value (true),
                 mutest_to_be_true,
                 mutest_not, mutest_to_be_false,
                 NULL);
  mutest_expect ("integer values",
                 mutest_int_value (42),
                 mutest_to_be_int_value, 42,
                 mutest_not, mutest_to_be_int_value, 47,
                 NULL);
  mutest_expect ("float values",
                 mutest_float_value (3.1416, 0.1),
                 mutest_to_be_float_value, 3.1415, 0.0001,
                 mutest_not, mutest_to_be_float_value, 3.14159, 0.00001,
                 NULL);

  void *p = (void *) 0xdeadbeef;

  mutest_expect ("pointers",
                 mutest_pointer (p),
                 mutest_not, mutest_to_be_null,
                 NULL);
}

static void
check_ranges (mutest_spec_t *spec MUTEST_UNUSED)
{
  mutest_expect ("integer range",
                 mutest_int_value (42),
                 mutest_to_be_in_int_range, 40, 45,
                 mutest_not, mutest_to_be_in_int_range, 30, 41,
                 mutest_not, mutest_to_be_in_int_range, 43, 50,
                 NULL);
  mutest_expect ("float range",
                 mutest_float_value (3.14, 0.01),
                 mutest_to_be_in_float_range, 3.0, 4.0,
                 NULL);
}

static void
check_string (mutest_spec_t *spec MUTEST_UNUSED)
{
  mutest_expect ("exact string",
                 mutest_string_value ("hello, world"),
                 mutest_to_be_string, "hello, world",
                 NULL);
  mutest_expect ("substring",
                 mutest_string_value ("hello, world"),
                 mutest_to_contain_string, "o, w",
                 NULL);
  mutest_expect ("string prefix",
                 mutest_string_value ("hello, world"),
                 mutest_to_start_with_string, "hello",
                 NULL);
  mutest_expect ("string suffix",
                 mutest_string_value ("hello, world"),
                 mutest_to_end_with_string, "world",
                 NULL);
}

static void
value_types (mutest_suite_t *suite MUTEST_UNUSED)
{
  mutest_it ("check scalars", check_scalars);
  mutest_it ("check ranges", check_ranges);
  mutest_it ("check string", check_string);
}

MUTEST_MAIN (
  mutest_describe ("Values", value_types);
)
