#include "gcoroutine.h"
#include "glib.h"
#include <assert.h>

static GArray *odd_event_case_check = NULL;
static void odd_event_case_record(gcstr label, gint i) {
  g_array_add(odd_event_case_check, gint, i);
  printf("\n%ld > %s : %d\n", g_tick_count(), label, i);
}
static GCoroutineStatus print_odd(GCoroutine *co) {
  gint *i = (gint *)co->user_data;
  gint count = *(gint *)g_map_get(co->context->shared, "count");
  co_begin(co);
  for (; *i < count; (*i)++) {
    odd_event_case_record("odd", *i * 2 + 1);
    co_sleep(co, 100);
  }
  co_end(co);
}

static GCoroutineStatus print_even(GCoroutine *co) {
  gint *i = (gint *)co->user_data;
  gint count = *(gint *)g_map_get(co->context->shared, "count");
  co_begin(co);
  co_sleep(co, 50);
  for (; *i < count; (*i)++) {
    odd_event_case_record("even", *i * 2 + 2);
    co_sleep(co, 100);
  }
  co_end(co);
}

int test_coroutine(int argc, char *argv[]) {
  g_mem_record(g_mem_record_default_callback);
  g_mem_record_begin();

  odd_event_case_check = g_array_new(gint);
  GCoroutineContext *context = g_coroutine_context_new();
  gint count = 5;
  g_map_set(context->shared, "count", &count);
  gint i1 = 0;
  gint i2 = 0;
  GCoroutine *co_odd = g_coroutine_new_with(context, print_odd, &i1, NULL);
  GCoroutine *co_even = g_coroutine_new_with(context, print_even, &i2, NULL);

  g_coroutine_start(co_odd);
  g_coroutine_start(co_even);
  while (g_coroutine_context_alive_count(context)) {
    g_coroutine_context_loop(context);
    g_sleep(10);
  }
  g_coroutine_context_free(context);
  gint size = g_array_size(odd_event_case_check);
  assert(size == count * 2);
  gint *nums = g_array(odd_event_case_check, gint);
  for (gint i = 0; i < size; i++)
    assert(nums[i] == i + 1);
  g_array_free(odd_event_case_check);

  gulong allocated = 0;
  gulong freed = 0;
  gulong peak = 0;
  g_mem_profile(&allocated, &freed, &peak);
  g_mem_record_end();
  printf("\r\nallocated memory: %ld  \r\nfreed memory: %ld  \r\npeak memory: %ld\r\n",
         allocated, freed, peak);
  assert(allocated == freed);
  return 0;
}
