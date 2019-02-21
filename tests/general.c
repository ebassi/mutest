#include <mutest.h>

static void
general_spec (mutest_spec_t *spec)
{
  bool a = true;

  mutest_expect ("a is true",
                 mutest_bool_value (a),
                 mutest_to_be_true,
                 NULL);
  mutest_expect ("a is not false",
                 mutest_bool_value (a),
                 mutest_not, mutest_to_be_false,
                 NULL);
}

static void
another_spec (mutest_spec_t *spec)
{
  const char *str = "hello, world";

  mutest_expect ("str contains 'hello'",
                 mutest_string_value (str),
                 mutest_to_contain_string, "hello",
                 NULL);
  mutest_expect ("str contains 'world'",
                 mutest_string_value (str),
                 mutest_to_contain_string, "world",
                 NULL);
  mutest_expect ("contains all fragments",
                 mutest_string_value (str),
                 mutest_to_start_with_string, "hello",
                 mutest_to_contain_string, ",",
                 mutest_to_end_with_string, "world",
                 NULL);
}

static void
skip_spec (mutest_spec_t *spec)
{
  mutest_expect ("skip this test",
                 mutest_bool_value (true),
                 mutest_skip,
                 NULL);
}

static void
general_suite (mutest_suite_t *suite)
{
  mutest_it ("contains at least a spec with an expectation", general_spec);
  mutest_it ("can contain multiple specs", another_spec);
  mutest_it ("should be skipped", skip_spec);
}

MUTEST_MAIN (
  mutest_describe ("General", general_suite);
)
