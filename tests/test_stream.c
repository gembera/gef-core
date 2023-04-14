#include "glib.h"
#include "gobject.h"
#include "gmemorystream.h"
#include <assert.h>

int test_stream(int, char *[]) {
  g_mem_record(g_mem_record_default_callback);
  g_mem_record_begin();
  gchar* hello = "hello ";
  gchar* gef = "gef";
  GMemoryStream * smem = g_object_new_of(GMemoryStream);
  g_memory_stream_write(smem, hello, g_len(hello));
  g_object_free(smem);

  g_auto_free();

  gulong allocated = 0;
  gulong freed = 0;
  gulong peak = 0;
  g_mem_profile(&allocated, &freed, &peak);
  printf("\nallocated memory: %d  \nfreed memory: %d\npeak memory: %d\n", allocated, freed, peak);
  assert(allocated == freed);
  g_mem_record_end();
  return 0;
}
