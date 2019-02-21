/* µTest
 *
 * Copyright 2019  Emmanuele Bassi
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#ifdef __cplusplus
# define MUTEST_BEGIN_DECLS     extern "C" {
# define MUTEST_END_DECLS       }
#else
# define MUTEST_BEGIN_DECLS
# define MUTEST_END_DECLS
#endif

#ifndef MUTEST_PUBLIC
# define MUTEST_PUBLIC          extern
#endif

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#if __GNUC__ >= 4
# define MUTEST_NULL_TERMINATED __attribute__((__sentinel__))
#else
# define MUTEST_NULL_TERMINATED
#endif

MUTEST_BEGIN_DECLS

typedef struct _mutest_suite_t mutest_suite_t;
typedef struct _mutest_spec_t mutest_spec_t;
typedef struct _mutest_expect_t mutest_expect_t;
typedef struct _mutest_expect_res_t mutest_expect_res_t;

typedef void (* mutest_describe_func_t) (mutest_suite_t *suite);

typedef void (* mutest_spec_func_t) (mutest_spec_t *spec);

typedef bool (* mutest_expect_func_t) (mutest_expect_t *e,
                                       mutest_expect_res_t *check);

typedef bool (* mutest_expect_closure_func_t) (const mutest_expect_res_t *value,
                                               void *data);

MUTEST_PUBLIC
mutest_expect_res_t *
mutest_bool_value (bool value);

MUTEST_PUBLIC
mutest_expect_res_t *
mutest_int_value (int value);

MUTEST_PUBLIC
mutest_expect_res_t *
mutest_string_value (const char *value);

MUTEST_PUBLIC
mutest_expect_res_t *
mutest_static_string_value (const char *value);

MUTEST_PUBLIC
mutest_expect_res_t *
mutest_float_value (double value,
                    double tolerance);

MUTEST_PUBLIC
mutest_expect_res_t *
mutest_int_range (int min,
                  int max);

MUTEST_PUBLIC
mutest_expect_res_t *
mutest_float_range (double min,
                    double max);

MUTEST_PUBLIC
mutest_expect_res_t *
mutest_pointer (void *pointer);

MUTEST_PUBLIC
mutest_expect_res_t *
mutest_byte_array (void *data,
                   size_t element_size,
                   size_t length);

MUTEST_PUBLIC
mutest_expect_res_t *
mutest_closure (mutest_expect_closure_func_t func,
                void *data);

MUTEST_PUBLIC
bool
mutest_not (mutest_expect_t *e,
            mutest_expect_res_t *check);

MUTEST_PUBLIC
bool
mutest_skip (mutest_expect_t *e,
             mutest_expect_res_t *check);

MUTEST_PUBLIC
bool
mutest_to_be_true (mutest_expect_t *e,
                   mutest_expect_res_t *check);

MUTEST_PUBLIC
bool
mutest_to_be_false (mutest_expect_t *e,
                    mutest_expect_res_t *check);

MUTEST_PUBLIC
bool
mutest_to_be_null (mutest_expect_t *e,
                   mutest_expect_res_t *check);

MUTEST_PUBLIC
bool
mutest_to_be_int_value (mutest_expect_t *e,
                        mutest_expect_res_t *check);

MUTEST_PUBLIC
bool
mutest_to_be_in_int_range (mutest_expect_t *e,
                           mutest_expect_res_t *check);

MUTEST_PUBLIC
bool
mutest_to_be_float_value (mutest_expect_t *e,
                          mutest_expect_res_t *check);

MUTEST_PUBLIC
bool
mutest_to_be_in_float_range (mutest_expect_t *e,
                             mutest_expect_res_t *check);

MUTEST_PUBLIC
bool
mutest_to_be_string (mutest_expect_t *e,
                     mutest_expect_res_t *check);

MUTEST_PUBLIC
bool
mutest_to_contain_string (mutest_expect_t *e,
                          mutest_expect_res_t *check);

MUTEST_PUBLIC
bool
mutest_to_start_with_string (mutest_expect_t *e,
                             mutest_expect_res_t *check);

MUTEST_PUBLIC
bool
mutest_to_end_with_string (mutest_expect_t *e,
                           mutest_expect_res_t *check);

MUTEST_PUBLIC
void
mutest_expect_full (const char *file,
                    int line,
                    const char *func_name,
                    const char *description,
                    mutest_expect_res_t *value,
                    mutest_expect_func_t expect_func,
                    ...) MUTEST_NULL_TERMINATED;

#define mutest_expect(description,value,expect_func,...) \
  mutest_expect_full (__FILE__, __LINE__, __func__, description, value, expect_func, __VA_ARGS__)

MUTEST_PUBLIC
void
mutest_it_full (const char *file,
                int line,
                const char *func_name,
                const char *description,
                mutest_spec_func_t func);

#define mutest_it(description,func) \
  mutest_it_full (__FILE__, __LINE__, __func__, description, func)

MUTEST_PUBLIC
void
mutest_describe_full (const char *file,
                      int line,
                      const char *func_name,
                      const char *description,
                      mutest_describe_func_t func);

#define mutest_describe(description,func) \
  mutest_describe_full (__FILE__, __LINE__, __func__, description, func)

MUTEST_PUBLIC
void
mutest_init (void);

MUTEST_PUBLIC
int
mutest_report (void);

#define MUTEST_MAIN(_C_) \
int main (int argc, char *argv[]) { \
  mutest_init (); \
\
  { _C_ } \
\
  return mutest_report (); \
}

MUTEST_END_DECLS
