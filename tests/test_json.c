#include "gjson.h"
#include "glib.h"
#include "gmemorystream.h"
#include "gobject.h"
#include "gvalue.h"
#include <assert.h>

#include "../libs/mjson/mjson.h"

gstr g_json_stringify(GValue *val) {}
int test_json(int argc, char *argv[]) {
  g_mem_record(g_mem_record_default_callback);
  g_mem_record_begin();

  const char *s =
      "{\"a\":123,\"b\":[1,2,3.333,{\"c\":1}],\"d\":null, \"t\":true, "
      "\"f\":false, \"o\":{\"c\":\"xxx\"}}";
  GValue *json = g_json_parse(s);
  GValue *val_a = g_json_get(json, "a");
  assert(g_value_int(val_a) == 123);
  GValue *val_b = g_json_get(json, "b");
  gint size = g_json_size(val_b);
  assert(size == 4);
  GValue *val_i = g_json_index(val_b, 2);
  assert(g_value_double(val_i) == 3.333);
  val_i = g_json_index(val_b, 3);
  GValue *val_c = g_json_get(val_i, "c");
  assert(g_value_int(val_c) == 1);
  GValue *val_d = g_json_get(json, "d");
  assert(g_value_is(val_d, G_JSON_NULL));
  GValue *val_t = g_json_get(json, "t");
  assert(g_value_bool(val_t) == TRUE);
  GValue *val_f = g_json_get(json, "f");
  assert(g_value_bool(val_f) == FALSE);
  GValue *val_o = g_json_get(json, "o");
  val_c = g_json_get(val_o, "c");
  assert(g_equal("xxx", g_value_str(val_c)));
  GValue *val_unknow = g_json_get(json, "unknown");
  assert(val_unknow == NULL);

  g_value_free(json);

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
