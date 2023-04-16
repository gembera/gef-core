#include "gcoroutine.h"
#include "glib.h"
#include <assert.h>

static GArray *odd_event_case_check = NULL;
static void odd_event_case_record(gcstr label, gint *i) {
  g_array_add(odd_event_case_check, gint, *i);
  printf("\n%ld > %s : %d\n", g_tick_count(), label, *i);
  *i += 2;
}
static GCoroutineStatus print_odd(GCoroutine *co) {
  gint *i = (gint *)co->user_data;
  co_begin(co);
  odd_event_case_record("odd", i);
  co_sleep(co, 100);
  odd_event_case_record("odd", i);
  co_sleep(co, 100);
  odd_event_case_record("odd", i);
  co_sleep(co, 100);
  co_end(co);
}

static GCoroutineStatus print_even(GCoroutine *co) {
  gint *i = (gint *)co->user_data;
  co_begin(co);
  co_sleep(co, 50);
  odd_event_case_record("even", i);
  co_sleep(co, 100);
  odd_event_case_record("even", i);
  co_sleep(co, 100);
  odd_event_case_record("even", i);
  co_end(co);
}

int test_coroutine(int, char *[]) {
  g_mem_record(g_mem_record_default_callback);
  g_mem_record_begin();

  odd_event_case_check = g_array_new(gint);
  GCoroutineManager *manager = g_coroutine_manager_new();
  gint i1 = 1;
  gint i2 = 2;
  GCoroutine *co_odd = g_coroutine_new_with(manager, print_odd, &i1, NULL);
  GCoroutine *co_even = g_coroutine_new_with(manager, print_even, &i2, NULL);

  g_coroutine_start(co_odd);
  g_coroutine_start(co_even);
  while (g_coroutine_manager_alive_count(manager)) {
    g_coroutine_manager_loop(manager);
    g_sleep(10);
  }
  g_coroutine_manager_free(manager);
  gint size = g_array_size(odd_event_case_check);
  assert(size == 6);
  gint *nums = g_array(odd_event_case_check, gint);
  for (gint i = 0; i < size; i++)
    assert(nums[i] == i + 1);
  g_array_free(odd_event_case_check);

  gulong allocated = 0;
  gulong freed = 0;
  gulong peak = 0;
  g_mem_profile(&allocated, &freed, &peak);
  printf("\nallocated memory: %d  \nfreed memory: %d\npeak memory: %d\n",
         allocated, freed, peak);
  assert(allocated == freed);
  g_mem_record_end();
  return 0;
}
