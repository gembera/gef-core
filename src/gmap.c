/*
 * Copyright (c) 2023 Gembera Team
 * https://github.com/gembera
 * License: MIT
 */

#include "glib.h"

static void g_map_lookup(GMap *self, gconstpointer key, gint *left,
                         gint *right) {
  *left = -1;
  *right = -1;
  g_return_if_fail(self);
  gint size, l, r, m;
  GMapPair node;
  size = g_map_size(self);
  if (size == 0)
    return;
  l = 0;
  r = size - 1;
  while (l <= r) {
    m = (l + r) / 2;
    node = g_array_get(self->data, GMapPair, m);
    gint comp = self->key_compare_handler(key, node.key);
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

GMap *g_map_new(GCompareHandler key_compare_func) {
  GMap *map = g_new(GMap);
  g_return_val_if_fail(map, NULL);
  map->key_compare_handler = key_compare_func;
  map->data = g_array_new_of(GMapPair);
  return map;
}

void g_map_free(GMap *self) {
  g_return_if_fail(self);
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
  for (gint i = 0; i < size; i++) {
    GMapPair *node = g_array(self->data, GMapPair) + i;
    func(self, node->key, node->value, user_data);
  }
}
void g_map_remove_all(GMap *self) {
  g_return_if_fail(self);
  g_array_set_size(self->data, 0);
}

void g_map_remove(GMap *self, gconstpointer key) {
  g_return_if_fail(self);
  gint l, r;
  g_map_lookup(self, key, &l, &r);
  if (l == r && l != -1)
    g_array_remove(self->data, l);
}

GMapPair *g_map_get(GMap *self, gconstpointer key) {
  g_return_val_if_fail(self, NULL);
  gint l, r;
  g_map_lookup(self, key, &l, &r);
  if (l == r && l != -1)
    return g_array(self->data, GMapPair) + l;
  return NULL;
}

GMapPair *g_map_search(GMap *self, GMapSearchHandler func, gpointer user_data) {
  g_return_val_if_fail(self, NULL);
  guint size = g_map_size(self);
  for (gint i = 0; i < size; i++) {
    GMapPair *node = g_array(self->data, GMapPair) + i;
    if (func(self, node->key, node->value, user_data))
      return node;
  }
}

void g_map_set(GMap *self, gpointer key, gpointer value) {
  g_return_if_fail(self != NULL);
  gint l, r;
  g_map_lookup(self, key, &l, &r);
  GMapPair nodenew;
  nodenew.key = key;
  nodenew.value = value;
  if (l == r) {
    if (l == -1) {
      g_array_insert_ref(self->data, 0, &nodenew);
      return;
    }
    g_array(self->data, GMapPair)[l].value = value;
  } else {
    g_array_insert_ref(self->data, r, &nodenew);
  }
}
