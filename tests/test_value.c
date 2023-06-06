#include "glib.h"
#include "gmemorystream.h"
#include "gobject.h"
#include "gvalue.h"
#include <assert.h>

int test_value(int argc, char *argv[]) {
  g_mem_record(g_mem_record_default_callback);
  g_mem_record_begin();

  gstr hello = "hello ";
  gstr gef = "gef";
  gstr err_message = "Something wrong";
  GValue *val1 = g_value_new();
  assert(g_value_is(val1, G_TYPE_NULL));
  g_value_set(val1, G_TYPE_ERROR, g_dup(err_message), g_free_callback);
  assert(g_value_is(val1, G_TYPE_ERROR));
  assert(*val1->refs == 1);
  assert(g_equal(err_message, g_value_pointer(val1)));
  GValue *val2 = g_value_set(g_value_new(), G_TYPE_STR, hello, NULL);
  assert(g_equal(hello, (gstr)g_value_pointer(val2)));
  GValue *val3 = g_value_set_double(g_value_new(), 16.0);
  assert(g_value_bool(val3) == TRUE);
  assert(g_value_int(val3) == 16);
  assert(g_value_long(val3) == 16);
  assert(g_value_double(val3) == 16);
  g_value_set_bool(val3, FALSE);
  assert(g_value_bool(val3) == FALSE);
  assert(g_value_int(val3) == 0);
  assert(g_value_long(val3) == 0);
  assert(g_value_double(val3) == 0);
  g_value_set_int(val3, 2020);
  assert(g_value_bool(val3) == TRUE);
  assert(g_value_int(val3) == 2020);
  assert(g_value_long(val3) == 2020);
  assert(g_value_double(val3) == 2020);
  g_value_set_long(val3, 6666666);
  assert(g_value_bool(val3) == TRUE);
  assert(g_value_int(val3) == 6666666);
  assert(g_value_long(val3) == 6666666);
  assert(g_value_double(val3) == 6666666);
  g_value_set_double(val3, 888.888);
  assert(g_value_bool(val3) == TRUE);
  assert(g_value_int(val3) == 888);
  assert(g_value_long(val3) == 888);
  assert(g_value_double(val3) == 888.888);
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
  g_value_set(val3, G_TYPE_MAP, g_map_new(NULL), (GFreeCallback)g_map_free);
  g_value_free(val1);
  g_value_free(val2);
  g_value_free(val3);

  g_auto_free();

  gulong allocated = 0;
  gulong freed = 0;
  gulong peak = 0;
  g_mem_profile(&allocated, &freed, &peak);
  g_mem_record_end();
  printf("\r\nallocated memory: %ld  \r\nfreed memory: %ld  \r\npeak memory: %ld\r\n",
         allocated, freed, peak);
  assert(allocated == freed);
  return 0;
}
