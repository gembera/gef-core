#include "gjson.h"
#include "mjson.h"

struct JsonParseData {
  gstr last_key;
  GValue *root;
  GPtrArray *stack;
};

static void json_parse_add_value(GValue *val_parent, gstr key, GValue *val) {
  if (g_value_is(val_parent, G_TYPE_PTR_ARRAY)) {
    g_ptr_array_add((GPtrArray *)g_value_pointer(val_parent), val);
  } else {
    g_return_if_fail(key);
    g_map_set((GMap *)g_value_pointer(val_parent), key, val);
  }
}
static int json_parse_cb(int ev, const char *s, int off, int len, void *ud) {
  struct JsonParseData *d = (struct JsonParseData *)ud;
  const char *sub = s + off;
  gint size = g_ptr_array_size(d->stack);
  GValue *val_parent = size ? g_ptr_array_get(d->stack, size - 1) : NULL;
  GValue *val_new = NULL;
  switch (ev) {
  case '{':
  case '[':
    val_new = g_value_new();
    if (ev == '{') {
      g_value_set(val_new, G_JSON_OBJECT,
                  g_map_new_with(NULL, g_free_callback,
                                 (GFreeCallback)g_value_free, NULL),
                  (GFreeCallback)g_map_free);
    } else {
      g_value_set(val_new, G_JSON_ARRAY,
                  g_ptr_array_new_with((GFreeCallback)g_value_free),
                  (GFreeCallback)g_ptr_array_free);
    }
    if (val_parent) {
      json_parse_add_value(val_parent, d->last_key, val_new);
    } else {
      d->root = val_new;
    }
    g_ptr_array_add(d->stack, val_new);
    break;
  case '}':
  case ']':
    g_return_val_if_fail(size, -1);
    g_ptr_array_remove(d->stack, size - 1);
    break;
  case MJSON_TOK_KEY:
    d->last_key = g_malloc0(len + 1);
    mjson_get_string(s + off, len, "$", d->last_key, len + 1);
    break;
  case MJSON_TOK_NUMBER:
    val_new = g_value_new();
    double num;
    mjson_get_number(s + off, len, "$", &num);
    g_value_set_double(val_new, num);
    json_parse_add_value(val_parent, d->last_key, val_new);
    break;
  case MJSON_TOK_STRING:
    val_new = g_value_new();
    gstr str = g_malloc0(len + 1);
    mjson_get_string(s + off, len, "$", str, len + 1);
    g_value_set_str(val_new, str);
    json_parse_add_value(val_parent, d->last_key, val_new);
    break;
  case MJSON_TOK_FALSE:
  case MJSON_TOK_TRUE:
    val_new = g_value_new();
    g_value_set_bool(val_new, ev == MJSON_TOK_TRUE);
    json_parse_add_value(val_parent, d->last_key, val_new);
    break;
  case MJSON_TOK_NULL:
    val_new = g_value_new();
    json_parse_add_value(val_parent, d->last_key, val_new);
    break;
  }
  return 0;
}
GValue *g_json_parse(gcstr s) {
  struct JsonParseData d = {NULL, NULL};
  d.stack = g_ptr_array_new();
  mjson(s, g_len(s), json_parse_cb, &d);
  g_ptr_array_free(d.stack);
  return d.root;
}
GValue *g_json_get(GValue *self, gcstr key) {
  g_return_val_if_fail(g_value_is(self, G_JSON_OBJECT), NULL);
  return (GValue *)g_map_get((GMap *)g_value_pointer(self), key);
}
GValue *g_json_index(GValue *self, gint index) {
  g_return_val_if_fail(g_value_is(self, G_JSON_ARRAY), NULL);
  return (GValue *)g_ptr_array_get((GPtrArray *)g_value_pointer(self), index);
}