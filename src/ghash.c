/*
 * Copyright (c) 2023 Gembera Team
 * https://github.com/gembera
 * License: MIT
 */

#include "glib.h"

static GHashNode *g_hash_table_lookup_node(GHashTable *hash_table,
                                           gconstpointer key);
static int g_hash_table_lookup_index(GHashTable *hash_table, gconstpointer key);

GHashTable *g_hash_table_new(GHashFunc hash_func,
                             GCompareFunc key_compare_func) {
  GHashTable *hash_table;

  hash_table = g_new0(GHashTable, 1);
  g_return_val_if_fail(hash_table != NULL, NULL);
  hash_table->hash_func = hash_func ? hash_func : g_direct_hash;
  hash_table->key_compare_func = key_compare_func;
  hash_table->nodes = g_array_new(sizeof(GHashNode));

  return hash_table;
}

static void g_hash_table_deep_destroy_items(gpointer key, gpointer value,
                                            gpointer user_data) {
  // g_free(key);
  g_free(value);
}
void g_hash_table_deep_destroy(GHashTable *hash_table) {
  g_return_if_fail(hash_table != NULL);
  g_hash_table_foreach(hash_table, g_hash_table_deep_destroy_items, NULL);
  g_hash_table_destroy(hash_table);
}

void g_hash_table_clear(GHashTable *hash_table) {
  g_return_if_fail(hash_table != NULL);
  g_array_set_length(hash_table->nodes, 0);
}

void g_hash_table_destroy(GHashTable *hash_table) {
  g_return_if_fail(hash_table != NULL);
  g_array_free(hash_table->nodes);
  g_free(hash_table);
}

void g_hash_table_remove(GHashTable *hash_table, gconstpointer key) {
  gint i = g_hash_table_lookup_index(hash_table, key);
  if (i >= 0)
    g_array_remove(hash_table->nodes, i);
}

gpointer g_hash_table_lookup(GHashTable *hash_table, gconstpointer key) {
  GHashNode *node;
  g_return_val_if_fail(hash_table != NULL, NULL);
  node = g_hash_table_lookup_node(hash_table, key);
  return node ? node->value : NULL;
}

gboolean g_hash_table_lookup_extended(GHashTable *hash_table,
                                      gconstpointer lookup_key,
                                      gpointer *orig_key, gpointer *value) {
  GHashNode *node;

  g_return_val_if_fail(hash_table != NULL, FALSE);

  node = g_hash_table_lookup_node(hash_table, lookup_key);

  if (node) {
    if (orig_key)
      *orig_key = node->key;
    if (value)
      *value = node->value;
    return TRUE;
  } else
    return FALSE;
}

void g_hash_table_foreach(GHashTable *hash_table, GHFunc func,
                          gpointer user_data) {
  gint i;
  gint size;
  g_return_if_fail(hash_table != NULL);
  size = g_hash_table_size(hash_table);
  for (i = 0; i < size; i++) {
    GHashNode *node = &g_array_get(hash_table->nodes, GHashNode, i);
    (*func)(node->key, node->value, user_data);
  }
}

/* Returns the number of elements contained in the hash table. */
gint g_hash_table_size(GHashTable *hash_table) {
  g_return_val_if_fail(hash_table != NULL, 0);

  return g_array_length(hash_table->nodes);
}

static int g_hash_table_lookup_index(GHashTable *hash_table,
                                     gconstpointer key) {
  gint size, l, r, m;
  guint hash;
  GHashNode *node = NULL;

  g_return_val_if_fail(hash_table != NULL, -1);

  size = g_hash_table_size(hash_table);
  hash = (*hash_table->hash_func)(key);

  l = 0;
  r = size - 1;
  while (l <= r) {
    m = (l + r) / 2;
    node = &g_array_get(hash_table->nodes, GHashNode, m);
    if (hash == node->hash)
      return m;
    else if (hash < node->hash) {
      r = m - 1;
    } else {
      l = m + 1;
    }
  }
  return -1;
}

static GHashNode *g_hash_table_lookup_node(GHashTable *hash_table,
                                           gconstpointer key) {
  int i = g_hash_table_lookup_index(hash_table, key);
  return i == -1 ? NULL : &g_array_get(hash_table->nodes, GHashNode, i);
}
void g_hash_table_insert(GHashTable *hash_table, gpointer key, gpointer value) {
  gint size, l, r, m;
  guint hash;
  GHashNode *node = NULL;
  GHashNode nodenew;

  g_return_if_fail(hash_table != NULL);

  size = g_hash_table_size(hash_table);
  hash = (*hash_table->hash_func)(key);

  l = 0;
  r = size - 1;
  while (l <= r) {
    m = (l + r) / 2;
    node = &g_array_get(hash_table->nodes, GHashNode, m);
    if (hash == node->hash) {
      node->value = value;
      return;
    } else if (hash < node->hash) {
      r = m - 1;
    } else {
      l = m + 1;
    }
  }

  nodenew.key = key;
  nodenew.value = value;
  nodenew.hash = hash;

  if (node == NULL)
    g_array_append_items(hash_table->nodes, &nodenew, 1);
  else
    g_array_insert_ref(hash_table->nodes, hash < node->hash ? m : m + 1, &nodenew);
}
