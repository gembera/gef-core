/*
 * Copyright (c) 2023 Gembera Team
 * https://github.com/gembera
 * License: MIT
 */

#ifndef _G_COROUTINE_H_
#define _G_COROUTINE_H_

#include "glib.h"

//when compile with VC++, please changed the compiler setting from "Program Database for Edit and Continue" to "Program Database"

#define LC_INIT(s) s = 0;

#define LC_RESUME(s) switch(s) { case 0:

#define LC_SET(s) s = __LINE__; case __LINE__:

#define LC_END(s) }

typedef struct {
	gint start;
	gint interval;
} GCoroutineTimer;

typedef struct {
	guint16 _case_line;
	GCoroutineTimer timer;	
	gpointer data; 
	gpointer local; // just like thread local variables, all variables used in coroutine should be limited in this GValue variable
} GCoroutine;

typedef enum{
	COROUTINE_STATUS_WAITING	= 0,
	COROUTINE_STATUS_YIELDED	= 1,
	COROUTINE_STATUS_EXITED		= 2,
	COROUTINE_STATUS_ENDED		= 3
} GCoroutineStatus;

typedef struct
{
	guint count;
} GCoroutineSemaphore;
// in COROUTINE_FUNC, do not use any local variables
typedef GCoroutineStatus (*COROUTINE_FUNC)(GCoroutine* co);

void g_coroutine_run(GCoroutine* co, COROUTINE_FUNC func);
void g_coroutine_stop(GCoroutine* co);

GCoroutine* g_coroutine_new(void);

#define g_coroutine_init(co) LC_INIT((co)->_case_line)
#define g_coroutine_begin(co) { GCoroutineStatus CO_YIELD_FLAG = COROUTINE_STATUS_YIELDED; LC_RESUME((co)->_case_line)

#define g_coroutine_end(co) LC_END((co)->_case_line); CO_YIELD_FLAG = COROUTINE_STATUS_WAITING; \
                   g_coroutine_init(co); return COROUTINE_STATUS_ENDED; }

#define g_coroutine_timer_start(t, ms) (t).start = g_get_tick_count(); (t).interval = ms;
#define g_coroutine_timer_expired(t) (g_get_tick_count() > (t).start + (t).interval)
#define g_coroutine_sleep(co, ms) g_coroutine_timer_start((co)->timer, ms); g_coroutine_wait_until(co, g_coroutine_timer_expired((co)->timer));

#define g_coroutine_wait_until(co, condition)	        \
  do {						\
    LC_SET((co)->_case_line);				\
    if(!(condition)) {				\
      return COROUTINE_STATUS_WAITING;			\
    }						\
  } while(0)

#define g_coroutine_wait_while(co, cond)  g_coroutine_wait_until((co), !(cond))

#define g_coroutine_wait(co, f) g_coroutine_wait_while((co), g_coroutine_is_alive(f))

#define g_coroutine_wait_child(co, child, f)		\
  do {						\
    g_coroutine_init((child));				\
    g_coroutine_wait((co), (f));		\
  } while(0)

#define g_coroutine_restart(co)				\
  do {						\
    g_coroutine_init(co);				\
    return COROUTINE_STATUS_WAITING;			\
  } while(0)

#define g_coroutine_exit(co)				\
  do {						\
    g_coroutine_init(co);				\
    return COROUTINE_STATUS_EXITED;			\
  } while(0)

#define g_coroutine_is_alive(f) ((f) < COROUTINE_STATUS_EXITED)

#define g_coroutine_yield(co)				\
  do {						\
    CO_YIELD_FLAG = COROUTINE_STATUS_WAITING;				\
    LC_SET((co)->_case_line);				\
    if(CO_YIELD_FLAG == COROUTINE_STATUS_WAITING) {			\
      return COROUTINE_STATUS_YIELDED;			\
    }						\
  } while(0)

#define g_coroutine_yield_until(co, cond)		\
  do {						\
    CO_YIELD_FLAG = COROUTINE_STATUS_WAITING;				\
    LC_SET((co)->_case_line);				\
    if((CO_YIELD_FLAG == COROUTINE_STATUS_WAITING) || !(cond)) {	\
      return COROUTINE_STATUS_YIELDED;			\
    }						\
  } while(0)

GCoroutineSemaphore * g_coroutine_semaphore_new(guint count);

#define g_coroutine_semaphore_wait(co, s)	\
	do {						\
	g_coroutine_wait_until(co, (s)->count > 0);		\
	--(s)->count;				\
	} while(0)

#define g_coroutine_semaphore_signal(co, s) ++(s)->count

#endif