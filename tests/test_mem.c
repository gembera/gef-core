#include "glib.h"
#include <assert.h>

int test_mem(int argc, char *argv[]) {
  g_mem_record(g_mem_record_default_callback);
  g_mem_record_begin();
  gpointer data1 = g_malloc0(100);
  assert(data1);
  gpointer data2 = g_malloc(200);
  assert(data2);
  gpointer data3 = g_realloc(data1, 150);
  assert(data3);
  g_free(data2);
  g_free(data3);
  g_mem_record_set_max(10);
  data1 = g_malloc0(100);
  assert(!data1);
  data2 = g_malloc(200);
  assert(!data2);
  data3 = g_realloc(data1, 150);
  assert(!data3);
  gulong allocated = 0;
  gulong freed = 0;
  gulong peak = 0;
  g_mem_profile(&allocated, &freed, &peak);
  g_mem_record_end();
  assert(allocated == freed);
  g_info("allocated memory: %ld  \tfreed memory: %ld  \tpeak memory: %ld\r\n",
         allocated, freed, peak);
  return 0;
}
