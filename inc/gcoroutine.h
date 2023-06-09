/*
 * Copyright (c) 2023 Gembera Team
 * https://github.com/gembera
 * License: MIT
 */

#ifndef __G_COROUTINE_H__
#define __G_COROUTINE_H__

#include "glib.h"

typedef enum {
  COROUTINE_STATUS_WAITING = 0,
  COROUTINE_STATUS_YIELDED = 1,
  COROUTINE_STATUS_EXITED = 2,
  COROUTINE_STATUS_ENDED = 3
} GCoroutineStatus;

typedef struct _GCoroutineContext GCoroutineContext;
typedef struct _GCoroutine GCoroutine;
// in GCoroutineHandler, do not use any local variables
typedef GCoroutineStatus (*GCoroutineHandler)(GCoroutine *co);

struct _GCoroutineContext {
  gbool standalone;
  GCoroutineContext *parent;
  GPtrArray *coroutines;
  GPtrArray *children;
  GMap *shared;
};

typedef struct {
  gulong start;
  guint interval;
} GCoroutineTimer;

struct _GCoroutine {
  GCoroutineContext *context;
  GCoroutineHandler handler;
  GCoroutineStatus status;
  GCoroutineTimer sleep_timer;
  gpointer user_data;
  GFreeCallback user_data_free_callback;
  guint16 _case_line;
};

typedef struct {
  guint count;
} GCoroutineSemaphore;

#define g_coroutine_context_new() g_coroutine_context_new_with(NULL)
GCoroutineContext *g_coroutine_context_new_with(GCoroutineContext *parent);
void g_coroutine_context_loop(GCoroutineContext *self);
gint g_coroutine_context_alive_count(GCoroutineContext *self);
void g_coroutine_context_free(GCoroutineContext *self);

void g_coroutine_start(GCoroutine *self);
void g_coroutine_stop(GCoroutine *self);
#define g_coroutine_is_alive(self) ((self)->status < COROUTINE_STATUS_EXITED)

#define g_coroutine_new(context, handler)                                      \
  g_coroutine_new_with(context, handler, NULL, NULL)
GCoroutine *g_coroutine_new_with(GCoroutineContext *context,
                                 GCoroutineHandler handler, gpointer user_data,
                                 GFreeCallback user_data_free_callback);
void g_coroutine_free(GCoroutine *self);

// when compile with VC++, please changed the compiler setting from "Program
// Database for Edit and Continue" to "Program Database"

#define LC_INIT(s) s = 0;

#define LC_RESUME(s)                                                           \
  switch (s) {                                                                 \
  case 0:

#define LC_SET(s)                                                              \
  s = __LINE__;                                                                \
  case __LINE__:

#define LC_END(s) }

#define co_init(co) LC_INIT((co)->_case_line)
#define co_begin(co)                                                           \
  {                                                                            \
    GCoroutineStatus CO_YIELD_FLAG = COROUTINE_STATUS_YIELDED;                 \
    LC_RESUME((co)->_case_line)

#define co_end(co)                                                             \
  LC_END((co)->_case_line);                                                    \
  CO_YIELD_FLAG = COROUTINE_STATUS_WAITING;                                    \
  co_init(co);                                                                 \
  return COROUTINE_STATUS_ENDED;                                               \
  }

#define co_timer_start(t, ms)                                                  \
  (t).start = g_tick_count();                                                  \
  (t).interval = ms;
#define co_timer_expired(t) (g_tick_count() > (t).start + (t).interval)
#define co_sleep(co, ms)                                                       \
  co_timer_start((co)->sleep_timer, ms);                                       \
  co_wait_until(co, co_timer_expired((co)->sleep_timer));

#define co_wait_until(co, condition)                                           \
  do {                                                                         \
    LC_SET((co)->_case_line);                                                  \
    if (!(condition)) {                                                        \
      return COROUTINE_STATUS_WAITING;                                         \
    }                                                                          \
  } while (0)

#define co_wait_while(co, cond) co_wait_until((co), !(cond))

#define co_restart(co)                                                         \
  do {                                                                         \
    co_init(co);                                                               \
    return COROUTINE_STATUS_WAITING;                                           \
  } while (0)

#define co_exit(co)                                                            \
  do {                                                                         \
    co_init(co);                                                               \
    return COROUTINE_STATUS_EXITED;                                            \
  } while (0)

#define co_yield                                                               \
  (co) do {                                                                    \
    CO_YIELD_FLAG = COROUTINE_STATUS_WAITING;                                  \
    LC_SET((co)->_case_line);                                                  \
    if (CO_YIELD_FLAG == COROUTINE_STATUS_WAITING) {                           \
      return COROUTINE_STATUS_YIELDED;                                         \
    }                                                                          \
  }                                                                            \
  while (0)

#define co_yield_until(co, cond)                                               \
  do {                                                                         \
    CO_YIELD_FLAG = COROUTINE_STATUS_WAITING;                                  \
    LC_SET((co)->_case_line);                                                  \
    if ((CO_YIELD_FLAG == COROUTINE_STATUS_WAITING) || !(cond)) {              \
      return COROUTINE_STATUS_YIELDED;                                         \
    }                                                                          \
  } while (0)

GCoroutineSemaphore *g_coroutine_semaphore_new(guint count);

#define co_semaphore_wait(co, s)                                               \
  do {                                                                         \
    co_wait_until(co, (s)->count > 0);                                         \
    --(s)->count;                                                              \
  } while (0)

#define co_semaphore_signal(co, s) ++(s)->count

#endif