#include "gtime.h"
#include <assert.h>

int test_time(int argc, char *argv[]) {
  g_mem_record(g_mem_record_default_callback);
  g_mem_record_begin();

  guint64 st = g_tick();
  g_sleep(100);
  guint64 en = g_tick();
  g_info("st: %llu, en: %llu, delta: %llu ms", st, en, en - st);
  assert(en - st >= 100);

  st = g_utick();
  g_usleep(1000);
  en = g_utick();
  g_info("st: %llu, en: %llu, delta: %llu us", st, en, en - st);
  assert(en - st >= 1000);

  gulong allocated = 0;
  gulong freed = 0;
  gulong peak = 0;
  g_mem_profile(&allocated, &freed, &peak);
  g_mem_record_end();
  g_info("allocated memory: %ld  \tfreed memory: %ld  \tpeak memory: %ld\r\n",
         allocated, freed, peak);
  assert(allocated == freed);
  return 0;
}
