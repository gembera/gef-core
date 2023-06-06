#include "glib.h"
#include <assert.h>

int test_mem(int argc, char *argv[]) {
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
  g_mem_record_end();
  assert(allocated == (100 + 200 + 150));
  assert(freed == (100 + 200 + 150));
  printf("\r\nallocated memory: %ld  \r\nfreed memory: %ld  \r\npeak memory: %ld\r\n", allocated, freed, peak);
  return 0;
}
