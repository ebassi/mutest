/* mutest-private.h: Private declarations
 *
 * µTest
 *
 * Copyright 2019  Emmanuele Bassi
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "config.h"

#include "mutest.h"

#include <stdint.h>
#include <stdarg.h>

MUTEST_BEGIN_DECLS

typedef enum {
  MUTEST_OUTPUT_MOCHA,
  MUTEST_OUTPUT_TAP
} mutest_output_format_t;

typedef struct {
  bool initialized;

  int term_width;
  int term_height;

  bool is_tty;
  bool use_colors;

  mutest_suite_t *current_suite;
  mutest_spec_t *current_spec;

  int n_tests;
  int pass;
  int fail;
  int skip;

  int64_t start_time;
  int64_t end_time;

  mutest_output_format_t output_format;

  mutest_hook_func_t before_hook;
  mutest_hook_func_t after_hook;
} mutest_state_t;

typedef enum {
  MUTEST_RESULT_PASS,
  MUTEST_RESULT_FAIL,
  MUTEST_RESULT_SKIP
} mutest_result_t;

typedef enum {
  MUTEST_EXPECT_INVALID,

  MUTEST_EXPECT_BOOLEAN,
  MUTEST_EXPECT_INT,
  MUTEST_EXPECT_INT_RANGE,
  MUTEST_EXPECT_FLOAT,
  MUTEST_EXPECT_FLOAT_RANGE,
  MUTEST_EXPECT_STR,
  MUTEST_EXPECT_POINTER
} mutest_expect_type_t;

typedef enum {
  MUTEST_COLLECT_NONE = 0,
  MUTEST_COLLECT_INT = 1 << 0,
  MUTEST_COLLECT_FLOAT = 1 << 1,
  MUTEST_COLLECT_STRING = 1 << 2,
  MUTEST_COLLECT_POINTER = 1 << 3,
  MUTEST_COLLECT_BOOLEAN = 1 << 4,
  MUTEST_COLLECT_PRECISION = 1 << 5,
  MUTEST_COLLECT_RANGE = 1 << 6,
  MUTEST_COLLECT_MATCHING_TYPE = 1 << 7,

  MUTEST_COLLECT_NUMBER = MUTEST_COLLECT_INT | MUTEST_COLLECT_FLOAT,
  MUTEST_COLLECT_SCALAR = MUTEST_COLLECT_INT |
                          MUTEST_COLLECT_FLOAT |
                          MUTEST_COLLECT_STRING |
                          MUTEST_COLLECT_POINTER |
                          MUTEST_COLLECT_BOOLEAN
} mutest_collect_type_t;

typedef mutest_expect_res_t *(* mutest_collect_func_t) (mutest_expect_type_t expect_type,
                                                        mutest_collect_type_t collect_type,
                                                        va_list args);

struct _mutest_expect_res_t
{
  mutest_expect_type_t expect_type;

  union {
    bool v_bool;

    int v_int;

    struct {
      int min;
      int max;
    } v_irange;

    struct {
      double value;
      double tolerance;
    } v_float;

    struct {
      double min;
      double max;
    } v_frange;

    struct {
      char *str;
      size_t len;
    } v_str;

    void *v_pointer;
  } expect;
};

struct _mutest_expect_t
{
  const char *file;
  int line;
  const char *func_name;

  const char *description;

  mutest_expect_res_t *value;

  mutest_result_t result;
};

struct _mutest_spec_t
{
  const char *file;
  int line;
  const char *func_name;

  const char *description;

  int n_tests;
  int pass;
  int fail;
  int skip;

  int64_t start_time;
  int64_t end_time;

  bool skip_all;
};

struct _mutest_suite_t
{
  const char *file;
  int line;
  const char *func_name;

  const char *description;

  int64_t start_time;
  int64_t end_time;

  mutest_hook_func_t before_each_hook;
  mutest_hook_func_t after_each_hook;

  bool skip_all;
};

#define mutest_oom_abort() \
  mutest_assert_message (__FILE__, __LINE__, __func__, "out-of-memory")

#define mutest_assert_if_reached(str) \
  mutest_assert_message (__FILE__, __LINE__, __func__, (str))

#define mutest_assert(x) \
  mutest_assert_message (__FILE__, __LINE__, __func__, #x)

#if defined(__GNUC__) && __GNUC__ > 3
# define mutest_likely(x)       (__builtin_expect((x) ? 1 : 0, 1))
# define mutest_unlikely(x)     (__builtin_expect((x) ? 1 : 0, 0))
#else
# define mutest_likely(x)       (x)
# define mutest_unlikely(x)     (x)
#endif

#ifdef OS_WINDOWS
# define strdup(x) _strdup(x)
# define write(fd,buf,count) _write(fd, buf, count)
#endif

mutest_expect_res_t *
mutest_expect_res_alloc (mutest_expect_type_t type);

void
mutest_expect_res_free (mutest_expect_res_t *res);

void
mutest_print (int fd,
              const char *first_fragment,
              ...) MUTEST_NULL_TERMINATED;

void
mutest_assert_message (const char *file,
                       int line,
                       const char *func,
                       const char *message) MUTEST_NO_RETURN;

mutest_state_t *
mutest_get_global_state (void);

int
mutest_get_term_width (void);

int
mutest_get_term_height (void);

bool
mutest_is_term_tty (void);

bool
mutest_use_colors (void);

mutest_output_format_t
mutest_get_output_format (void);

void
mutest_set_current_suite (mutest_suite_t *suite);

mutest_suite_t *
mutest_get_current_suite (void);

void
mutest_set_current_spec (mutest_spec_t *spec);

mutest_spec_t *
mutest_get_current_spec (void);

int64_t
mutest_get_current_time (void);

void
mutest_add_pass (void);

void
mutest_add_fail (void);

void
mutest_add_skip (void);

void
mutest_expect_res_to_string (mutest_expect_res_t *res,
                             char *buf,
                             size_t len);

void
mutest_spec_add_result (mutest_spec_t *spec,
                        mutest_expect_t *expect);

void
mutest_print_suite_preamble (mutest_suite_t *suite);

void
mutest_print_totals (void);

void
mutest_print_spec_preamble (mutest_spec_t *suite);

void
mutest_print_spec_totals (mutest_spec_t *spec);

void
mutest_print_suite_totals (mutest_suite_t *suite);

void
mutest_print_expect (mutest_expect_t *expect);

void
mutest_print_expect_fail (mutest_expect_t *expect,
                          bool negate,
                          mutest_expect_res_t *check,
                          const char *check_repr);

MUTEST_END_DECLS
