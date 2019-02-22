/* mutest-private.h: Private declarations
 *
 * µTest
 *
 * Copyright 2019  Emmanuele Bassi
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "mutest.h"

#include <stdint.h>
#include <stdarg.h>

MUTEST_BEGIN_DECLS

typedef struct _mutest_list_t   mutest_list_t;

struct _mutest_list_t {
  mutest_list_t *next, *prev;
  // back pointer, because we cannot portable use container_of()
  void *data;
};

#define MUTEST_LIST_INIT(name)  { .next = &(name), .prev = &(name) }

static inline void
mutest_list_init (mutest_list_t *head,
                  void *data)
{
  head->next = head;
  head->prev = head;
  head->data = data;
}

static inline void
mutest_list_add (mutest_list_t *item,
                 mutest_list_t *prev,
                 mutest_list_t *next)
{
  next->prev = item;
  item->next = next;
  item->prev = prev;
  prev->next = item;
}

static inline void
mutest_list_prepend (mutest_list_t *item,
                     mutest_list_t *head)
{
  mutest_list_add (item, head, head->next);
}

static inline void
mutest_list_append (mutest_list_t *item,
                    mutest_list_t *head)
{
  mutest_list_add (item, head->prev, head);
}

static inline bool
mutest_list_is_empty (mutest_list_t *head)
{
  return head->next == head;
}

#define mutest_list_foreach(iter_name, head) \
  for (mutest_list_t *iter_name = (head)->next; \
       iter_name != head; \
       iter_name = iter_name->next)

#define mutest_list_entry(ptr, type) \
  (type *) ((ptr)->data)

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
} mutest_state_t;

typedef enum {
  MUTEST_RESULT_PASS,
  MUTEST_RESULT_FAIL,
  MUTEST_RESULT_SKIP
} mutest_result_t;

typedef enum {
  MUTEST_EXPECT_INVALID,

  MUTEST_EXPECT_BOOL,
  MUTEST_EXPECT_INT,
  MUTEST_EXPECT_INT_RANGE,
  MUTEST_EXPECT_FLOAT,
  MUTEST_EXPECT_FLOAT_RANGE,
  MUTEST_EXPECT_STR,
  MUTEST_EXPECT_POINTER,
  MUTEST_EXPECT_BYTE_ARRAY,
  MUTEST_EXPECT_CLOSURE,
} mutest_expect_type_t;

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

    struct {
      void *data;
      size_t element_size;
      size_t length;
    } v_bytearray;

    struct {
      mutest_expect_closure_func_t func;
      void *data;
    } v_closure;
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
};

struct _mutest_suite_t
{
  const char *file;
  int line;
  const char *func_name;

  const char *description;

  int64_t start_time;
  int64_t end_time;
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

mutest_expect_res_t *
mutest_expect_res_collect_value (mutest_expect_type_t type,
                                 va_list args);

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
mutest_print_expect (mutest_expect_t *expect);

MUTEST_END_DECLS
