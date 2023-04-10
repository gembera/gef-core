#include "glib.h"
#include <assert.h>

void mem_record_callback(gulong index, gpointer memnew, gpointer memfree,
                         gulong allocated, gulong freed, const char *__file__,
                         const int __line__) {
  printf("\n%ld\t%lx\t%lx\t%ld\t%ld\t%s(%d)", index, memnew, memfree,
         allocated, freed, __file__, __line__);
}
int test_mem(int, char *[]) {
  g_mem_record(mem_record_callback);
  g_mem_record_begin();
  gpointer data1 = g_malloc0(100);
  gpointer data2 = g_malloc(200);
  gpointer data3 = g_realloc(data1, 150);
  g_free(data2);
  g_free(data3);
  gulong allocated = 0;
  gulong freed = 0;
  g_mem_profile(&allocated, &freed);
  assert(allocated == (100 + 200 + 150));
  assert(freed == (100 + 200 + 150));
  g_mem_record_end();
  return 0;
}
