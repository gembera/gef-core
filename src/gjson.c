#include "gjson.h"
#include "gstring.h"
#include "mjson.h"

GValue *g_json_new() {
  GValue *self = g_value_new();
  g_return_val_if_fail(self, NULL);
  g_value_set(self, G_JSON_OBJECT,
              g_map_new_with((GMapKeyNewHandler)g_dup, g_free_callback,
                             (GFreeCallback)g_value_free, NULL),
              (GFreeCallback)g_map_free);
  return self;
}
GValue *g_json_new_array() {
  GValue *self = g_value_new();
  g_return_val_if_fail(self, NULL);
  g_value_set(self, G_JSON_ARRAY,
              g_ptr_array_new_with((GFreeCallback)g_value_free),
              (GFreeCallback)g_ptr_array_free);
  return self;
}
GValue *g_json_get(GValue *self, gcstr key) {
  g_return_val_if_fail(g_value_is(self, G_JSON_OBJECT), NULL);
  return (GValue *)g_map_get((GMap *)g_value_pointer(self), key);
}
gbool g_json_has(GValue *self, gcstr key) {
  g_return_val_if_fail(g_value_is(self, G_JSON_OBJECT), FALSE);
  return g_map_has((GMap *)g_value_pointer(self), key);
}
void g_json_set(GValue *self, gcstr key, GValue *val) {
  g_return_if_fail(g_value_is(self, G_JSON_OBJECT));
  g_map_set((GMap *)g_value_pointer(self), (gpointer)key, val);
}
void g_json_set_int(GValue *self, gcstr key, gint val) {
  g_json_set(self, key, g_value_set_int(g_value_new(), val));
}
void g_json_set_double(GValue *self, gcstr key, gdouble val) {
  g_json_set(self, key, g_value_set_double(g_value_new(), val));
}
void g_json_set_str(GValue *self, gcstr key, gstr val) {
  g_json_set(self, key, g_value_set_str(g_value_new(), val));
}
GMapEntry *g_json_get_entry(GValue *self, gcstr key) {
  g_return_val_if_fail(g_value_is(self, G_JSON_OBJECT), NULL);
  return g_map_get_entry((GMap *)g_value_pointer(self), key);
}
void g_json_remove(GValue *self, gcstr key) {
  g_return_if_fail(g_value_is(self, G_JSON_OBJECT));
  g_map_remove((GMap *)g_value_pointer(self), (gpointer)key);
}
void g_json_remove_all(GValue *self) {
  g_return_if_fail(g_value_is(self, G_JSON_OBJECT));
  g_map_remove_all((GMap *)g_value_pointer(self));
}
guint g_json_size(GValue *self) {
  g_return_val_if_fail(g_value_is(self, G_JSON_OBJECT), 0);
  return g_map_size((GMap *)g_value_pointer(self));
}
void g_json_visit(GValue *self, GMapVisitCallback func, gpointer user_data) {
  g_return_if_fail(g_value_is(self, G_JSON_OBJECT));
  g_map_visit((GMap *)g_value_pointer(self), func, user_data);
}

GValue *g_json_array_get(GValue *self, guint index) {
  g_return_val_if_fail(g_value_is(self, G_JSON_ARRAY), NULL);
  return (GValue *)g_ptr_array_get((GPtrArray *)g_value_pointer(self), index);
}
guint g_json_array_size(GValue *self) {
  g_return_val_if_fail(g_value_is(self, G_JSON_ARRAY), 0);
  return g_ptr_array_size((GPtrArray *)g_value_pointer(self));
}

void g_json_array_set(GValue *self, guint index, GValue *item) {
  g_return_if_fail(g_value_is(self, G_JSON_ARRAY));
  g_ptr_array_set((GPtrArray *)g_value_pointer(self), index, item);
}

void g_json_array_add(GValue *self, GValue *item) {
  g_return_if_fail(g_value_is(self, G_JSON_ARRAY));
  g_ptr_array_add((GPtrArray *)g_value_pointer(self), item);
}

GValue *g_json_array_add_int(GValue *self, gint item) {
  g_return_val_if_fail(g_value_is(self, G_JSON_ARRAY), NULL);
  GValue *val = g_value_set_int(g_value_new(), item);
  g_json_array_add(self, val);
  return val;
}
GValue *g_json_array_add_double(GValue *self, gdouble item) {
  g_return_val_if_fail(g_value_is(self, G_JSON_ARRAY), NULL);
  GValue *val = g_value_set_double(g_value_new(), item);
  g_json_array_add(self, val);
  return val;
}
GValue *g_json_array_add_str(GValue *self, gstr item) {
  g_return_val_if_fail(g_value_is(self, G_JSON_ARRAY), NULL);
  GValue *val = g_value_set_str(g_value_new(), item);
  g_json_array_add(self, val);
  return val;
}

GValue *g_json_array_insert_int(GValue *self, guint index, gint item) {
  g_return_val_if_fail(g_value_is(self, G_JSON_ARRAY), NULL);
  GValue *val = g_value_set_int(g_value_new(), item);
  g_json_array_insert(self, index, val);
  return val;
}
GValue *g_json_array_insert_double(GValue *self, guint index, gdouble item) {
  g_return_val_if_fail(g_value_is(self, G_JSON_ARRAY), NULL);
  GValue *val = g_value_set_double(g_value_new(), item);
  g_json_array_insert(self, index, val);
  return val;
}
GValue *g_json_array_insert_str(GValue *self, guint index, gstr item) {
  g_return_val_if_fail(g_value_is(self, G_JSON_ARRAY), NULL);
  GValue *val = g_value_set_str(g_value_new(), item);
  g_json_array_insert(self, index, val);
  return val;
}
void g_json_array_remove(GValue *self, guint index) {
  g_return_if_fail(g_value_is(self, G_JSON_ARRAY));
  g_ptr_array_remove((GPtrArray *)g_value_pointer(self), index);
}
void g_json_array_insert(GValue *self, guint index, GValue *item) {
  g_return_if_fail(g_value_is(self, G_JSON_ARRAY));
  g_ptr_array_insert((GPtrArray *)g_value_pointer(self), index, item);
}
gint g_json_array_index_of(GValue *self, GValue *item) {
  g_return_val_if_fail(g_value_is(self, G_JSON_ARRAY), -1);
  return g_ptr_array_index_of((GPtrArray *)g_value_pointer(self), item);
}
void g_json_array_visit(GValue *self, GPtrArrayVisitCallback func,
                        gpointer user_data) {
  g_return_if_fail(g_value_is(self, G_JSON_ARRAY));
  g_ptr_array_visit((GPtrArray *)g_value_pointer(self), func, user_data);
}
struct JsonParseData {
  gstr last_key;
  gint last_alloc;
  GValue *root;
  GPtrArray *stack;
};

static void json_parse_add_value(GValue *val_parent, gstr key, GValue *val) {
  if (g_value_is(val_parent, G_TYPE_PTR_ARRAY)) {
    g_json_array_add(val_parent, val);
  } else {
    g_return_if_fail(key);
    g_json_set(val_parent, key, val);
  }
}

static int json_parse_cb(int ev, const char *s, int off, int len, void *ud) {
  struct JsonParseData *d = (struct JsonParseData *)ud;
  gint size = g_ptr_array_size(d->stack);
  GValue *val_parent = size ? g_ptr_array_get(d->stack, size - 1) : NULL;
  GValue *val_new = NULL;
  switch (ev) {
  case '{':
  case '[':
    if (ev == '{') {
      val_new = g_json_new();
    } else {
      val_new = g_json_new_array();
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
    if (!d->last_key || d->last_alloc <= len) {
      d->last_alloc = len + 1;
      d->last_key = g_realloc(d->last_key, d->last_alloc);
    }
    mjson_get_string(s + off, len, "$", d->last_key, len + 1);
    break;
  case MJSON_TOK_NUMBER:
    val_new = g_value_new();
    double num;
    mjson_get_number(s + off, len, "$", &num);
    if ((gint)num == num) {
      g_value_set_int(val_new, num);
    } else {
      g_value_set_double(val_new, num);
    }
    json_parse_add_value(val_parent, d->last_key, val_new);
    break;
  case MJSON_TOK_STRING:
    val_new = g_value_new();
    gstr str = g_malloc0(len + 1);
    mjson_get_string(s + off, len, "$", str, len + 1);
    g_value_set_str(val_new, str);
    json_parse_add_value(val_parent, d->last_key, val_new);
    g_free(str);
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
  struct JsonParseData d = {NULL, 0, NULL, NULL};
  d.stack = g_ptr_array_new();
  int result = mjson(s, g_len(s), json_parse_cb, &d);
  g_ptr_array_free(d.stack);
  g_free(d.last_key);
  g_return_val_if_fail(result >= 0, NULL, g_value_free(d.root));
  return d.root;
}

static int _json_esc(int c, int esc) {
  const char *p, *esc1 = "\b\f\n\r\t\\\"", *esc2 = "bfnrt\\\"";
  for (p = esc ? esc1 : esc2; *p != '\0'; p++) {
    if (*p == c)
      return esc ? esc2[p - esc1] : esc1[p - esc2];
  }
  return 0;
}

static int _json_print_str(GString *str, const char *s) {
  int len = g_len(s);
  int i, n = g_string_append_with(str, "\"", 1);
  for (i = 0; i < len; i++) {
    char c = (char)(unsigned char)_json_esc(s[i], 1);
    if (c) {
      n += g_string_append_with(str, "\\", 1);
      n += g_string_append_with(str, &c, 1);
    } else {
      n += g_string_append_with(str, &s[i], 1);
    }
  }
  return n + g_string_append_with(str, "\"", 1);
}

static void _json_to_string(GString *str, GValue *val, gint level) {
  int i, len;
  g_return_if_fail(val);
  switch (val->type) {
  case G_TYPE_NULL:
    g_string_append(str, "null");
    break;
  case G_TYPE_STR:
    _json_print_str(str, g_value_str(val));
    break;
  case G_TYPE_BOOL:
    g_string_append(str, g_value_bool(val) ? "true" : "false");
    break;
  case G_TYPE_INT:
    g_string_printf(str, "%d", g_value_int(val));
    break;
  case G_TYPE_DOUBLE:
    g_string_printf(str, "%f", g_value_double(val));
    break;
  case G_TYPE_PTR_ARRAY: {
    GPtrArray *ar = (GPtrArray *)g_value_pointer(val);
    len = g_ptr_array_size(ar);
    g_string_append(str, "[");
    for (i = 0; i < len; i++) {
      _json_to_string(str, (GValue *)g_ptr_array_get(ar, i), level + 1);
      if (i < len - 1)
        g_string_append(str, ",");
    }
    g_string_append(str, "]");
  } break;
  case G_TYPE_MAP: {
    GMap *map = (GMap *)g_value_pointer(val);
    guint size = g_map_size(map);
    g_string_append(str, "{");
    for (i = 0; i < size; i++) {
      GMapEntry *entry = g_array(map->data, GMapEntry) + i;
      _json_print_str(str, (gstr)entry->key);
      g_string_append(str, ":");
      _json_to_string(str, (GValue *)entry->value, level + 1);
      if (i < size - 1)
        g_string_append(str, ",");
    }
    g_string_append(str, "}");
  } break;
  }
}

gstr g_json_stringify(GValue *self) {
  GString *str = g_string_new();
  _json_to_string(str, self, 0);
  return g_string_unwrap(str);
}
