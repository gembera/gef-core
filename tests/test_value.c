#include "glib.h"
#include "gmemorystream.h"
#include "gobject.h"
#include "gvalue.h"
#include <assert.h>

int test_value(int, char *[]) {
  g_mem_record(g_mem_record_default_callback);
  g_mem_record_begin();

  gstr hello = "hello ";
  gstr gef = "gef";

  GValue *val1 = g_value_new();
  assert(g_value_is(val1, G_TYPE_NULL));
  GValue *val2 = g_value_set(g_value_new(), G_TYPE_STR, hello, NULL);
  assert(g_equal(hello, (gstr)g_value_pointer(val2)));
  GValue *val3 = g_value_set_double(g_value_new(), 1.0);
  g_value_set(val3, G_TYPE_STR, g_dup(gef), g_free_callback);
  assert(g_equal(gef, (gstr)g_value_pointer(val3)));
  g_value_set(val1, G_TYPE_ARRAY, g_array_new(gint),
              (GFreeCallback)g_array_free);
  assert(val1->free_callback);
  assert(*val1->refs == 1);
  g_value_ref(val2, val1);
  assert(*val1->refs == 2);
  assert(*val2->refs == 2);
  assert(val1->free_callback == val2->free_callback);
  assert(val1->data.v_pointer == val2->data.v_pointer);
  g_value_set(val3, G_TYPE_MAP, g_map_new(), (GFreeCallback)g_map_free);
  g_value_free(val1);
  g_value_free(val2);
  g_value_free(val3);

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
