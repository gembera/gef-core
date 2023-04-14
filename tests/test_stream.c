#include "glib.h"
#include "gmemorystream.h"
#include "gobject.h"
#include <assert.h>

int test_stream(int, char *[]) {
  g_mem_record(g_mem_record_default_callback);
  g_mem_record_begin();

  gcstr hello = "hello ";
  gcstr gef = "gef";
  GMemoryStream *smem = g_object_new_of(GMemoryStream);
  g_stream_write_string(smem, hello);
  g_stream_write_string(smem, gef);
  g_stream_seek(smem, 0, SEEK_BEGIN);
  glong len;
  gstr buffer;
  g_stream_read_all_content(smem, &buffer, &len, TRUE);
  assert(g_equal(buffer, "hello gef"));
  g_free(buffer);
  g_object_free(smem);

  g_auto_free();

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
