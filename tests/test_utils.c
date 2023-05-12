#include "glib.h"
#include <assert.h>

int test_utils(int, char *[]) {
  g_mem_record(g_mem_record_default_callback);
  g_mem_record_begin();

  gulong st = g_tick_count();
  g_sleep(1000);
  gulong en = g_tick_count();
  printf("st: %ld, en: %ld, delta: %ld", st, en, en - st);
  assert(en - st >= 1000);

  gulong allocated = 0;
  gulong freed = 0;
  gulong peak = 0;
  g_mem_profile(&allocated, &freed, &peak);
  g_mem_record_end();
  printf("\nallocated memory: %ld  \nfreed memory: %ld\npeak memory: %ld\n",
         allocated, freed, peak);
  assert(allocated == freed);
  return 0;
}
