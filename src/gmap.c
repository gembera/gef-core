/*
 * Copyright (c) 2023 Gembera Team
 * https://github.com/gembera
 * License: MIT
 */

#include "glib.h"

static void g_map_free_key_value(GMap *self, GMapEntry *entry) {
  GFreeCallback key_free_callback = self->key_free_callback;
  GFreeCallback value_free_callback = self->value_default_free_callback;
  if (entry->value_custom_free_callback) {
    value_free_callback = entry->value_custom_free_callback;
  }
  g_free_with(entry->key, key_free_callback);
  g_free_with(entry->value, value_free_callback);
}
static void g_map_lookup(GMap *self, gcpointer key, gint *left, gint *right) {
  *left = -1;
  *right = -1;
  g_return_if_fail(self);
  gint size, l, r, m;
  GMapEntry *entry;
  size = g_map_size(self);
  if (size == 0)
    return;
  l = 0;
  r = size - 1;
  while (l <= r) {
    m = (l + r) / 2;
    entry = g_array(self->data, GMapEntry) + m;
    gint comp = self->key_compare_handler(key, entry->key);
    if (comp == 0) {
      *left = *right = m;
      return;
    } else if (comp < 0) {
      r = m - 1;
    } else {
      l = m + 1;
    }
  }
  *left = r;
  *right = l;
}

GMap *g_map_new_with(GMapKeyNewHandler key_new_handler,
                     GFreeCallback key_free_callback,
                     GFreeCallback value_free_callback,
                     GCompareHandler key_compare_handler) {
  GMap *map = g_new(GMap);
  g_return_val_if_fail(map, NULL);
  map->key_compare_handler = key_compare_handler == NULL
                                 ? (GCompareHandler)g_cmp
                                 : key_compare_handler;
  map->key_free_callback = key_free_callback;
  map->value_default_free_callback = value_free_callback;
  map->key_new_handler = key_new_handler;
  map->data = g_array_new(GMapEntry);
  return map;
}

void g_map_free(GMap *self) {
  g_return_if_fail(self);
  g_map_remove_all(self);
  g_array_free(self->data);
  g_free(self);
}

guint g_map_size(GMap *self) {
  g_return_val_if_fail(self, 0);
  return g_array_size(self->data);
}

void g_map_visit(GMap *self, GMapVisitCallback func, gpointer user_data) {
  g_return_if_fail(self);
  guint size = g_map_size(self);
  for (guint i = 0; i < size; i++) {
    GMapEntry *entry = g_array(self->data, GMapEntry) + i;
    func(self, entry->key, entry->value, user_data);
  }
}
void g_map_remove_all(GMap *self) {
  g_return_if_fail(self);
  guint size = g_map_size(self);
  for (guint i = 0; i < size; i++) {
    GMapEntry *entry = g_array(self->data, GMapEntry) + i;
    g_map_free_key_value(self, entry);
  }
  g_array_set_size(self->data, 0);
}
void g_map_remove(GMap *self, gcpointer key) {
  g_return_if_fail(self);
  gint l, r;
  g_map_lookup(self, key, &l, &r);
  if (l == r && l != -1) {
    g_map_free_key_value(self, g_array(self->data, GMapEntry) + l);
    g_array_remove(self->data, l);
  }
}
gbool g_map_has(GMap *self, gcpointer key) {
  GMapEntry *entry = g_map_get_entry(self, key);
  return entry != NULL;
}
gpointer g_map_get(GMap *self, gcpointer key) {
  GMapEntry *entry = g_map_get_entry(self, key);
  return entry == NULL ? NULL : entry->value;
}
GMapEntry *g_map_get_entry(GMap *self, gcpointer key) {
  g_return_val_if_fail(self, NULL);
  gint l, r;
  g_map_lookup(self, key, &l, &r);
  if (l == r && l != -1)
    return g_array(self->data, GMapEntry) + l;
  return NULL;
}

void g_map_set_with(GMap *self, gpointer key, gpointer value,
                    GFreeCallback value_free_callback) {
  g_return_if_fail(self != NULL);
  if (self->key_new_handler) {
    key = self->key_new_handler(key);
  }
  GMapEntry entrynew;
  entrynew.key = key;
  entrynew.value = value;
  entrynew.value_custom_free_callback = value_free_callback;
  gint l, r;
  g_map_lookup(self, key, &l, &r);
  if (l == r) {
    if (l == -1) {
      g_array_insert_ref(self->data, 0, &entrynew);
      return;
    }
    GMapEntry *entry = g_array(self->data, GMapEntry) + l;
    g_map_free_key_value(self, entry);
    entry->key = key;
    entry->value = value;
    entry->value_custom_free_callback = value_free_callback;
  } else {
    g_array_insert_ref(self->data, r, &entrynew);
  }
}
