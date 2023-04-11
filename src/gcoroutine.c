/*
 * Copyright (c) 2023 Gembera Team
 * https://github.com/gembera
 * License: MIT
 */

#include "gcoroutine.h"

typedef struct  
{
	COROUTINE_FUNC func;
	GCoroutine* co;
} GCoroutineRunner;

static gbool coroutine_initialized = FALSE;
static GSList * coroutine_funcs = NULL;

gbool g_coroutine_is_ready(void);
void g_coroutine_initialize(void);

GCoroutine* g_coroutine_new()
{
	GCoroutine * co = g_new0(GCoroutine, 1);
	return co;
}
/*
void g_coroutine_free(GCoroutine* co)
{
	g_return_if_fail(co != NULL);
	g_coroutine_stop(co);
	g_free(co);
}
*/
GCoroutineSemaphore * g_coroutine_semaphore_new(guint count)
{
	GCoroutineSemaphore * cs = g_new0(GCoroutineSemaphore, 1);
	cs->count = count;
	return cs;
}

static void _on_coroutine_process(gint tid)
{
	GSList * func = coroutine_funcs;
	while(func != NULL)
	{
		GCoroutineRunner * runner = (GCoroutineRunner *)func->data;
		func = g_slist_next(func);
		if (!g_coroutine_is_alive(runner->func(runner->co)))			
		{
			coroutine_funcs = g_slist_remove(coroutine_funcs, runner);
			g_free(runner->co);
			g_free(runner);
		}
	}
	/*
	if (coroutine_funcs == NULL)
	{
		vm_delete_timer(coroutine_timer_id);
		coroutine_timer_id = -1;
	}
	*/
}
void g_coroutine_stop(GCoroutine* co)
{
	GSList * func = coroutine_funcs;
	while(func != NULL)
	{
		GCoroutineRunner * runner = (GCoroutineRunner *)func->data;
		func = g_slist_next(func);
		if (runner->co == co)
		{
			coroutine_funcs = g_slist_remove(coroutine_funcs, runner);
			g_free(runner->co);
			g_free(runner);
			break;
		}
	}
}
void g_coroutine_run(GCoroutine* co, COROUTINE_FUNC func)
{
	GCoroutineRunner * runner = NULL;
	g_return_if_fail(co != NULL && func != NULL);
	g_coroutine_initialize();
	runner = g_new(GCoroutineRunner, 1);
	runner->func = func;
	runner->co = co;
	coroutine_funcs = g_slist_append(coroutine_funcs, runner);
}

gbool g_coroutine_is_ready()
{
	return coroutine_initialized;
}
void g_coroutine_initialize(void)
{
	//g_log_debug("g_coroutine_initialize %d", coroutine_initialized);
	if (!coroutine_initialized)
	{
		/*
		VMINT coroutine_timer_id = vm_create_timer(20, _on_coroutine_process);
		g_log_debug("g_coroutine_initialize : %d", coroutine_timer_id);
		*/
		coroutine_initialized = TRUE;
	}
}