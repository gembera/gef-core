#include "glib.h"
#include <assert.h>

int test_slist(int, char *[]) {
  g_mem_record(g_mem_record_default_callback);
  g_mem_record_begin();
  gpointer data1 = g_malloc0(100);
  gpointer data2 = g_malloc(200);
  gpointer data3 = g_realloc(data1, 150);
  g_free(data2);
  g_free(data3);
  gulong allocated = 0;
  gulong freed = 0;
  gulong peak = 0;
  g_mem_profile(&allocated, &freed, &peak);
  printf("\nallocated memory: %d  \nfreed memory: %d\npeak memory: %d\n", allocated, freed, peak);
  assert(allocated == freed);
  g_mem_record_end();
  return 0;
}
