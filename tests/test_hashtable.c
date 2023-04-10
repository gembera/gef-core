#include "glib.h"
#include <assert.h>

#define KEY_A "key-a"
#define KEY_B "key-b"

int test_hashtable(int, char *[]) {
  gpointer hvalue = NULL;
  gpointer orig_key;
  gpointer orig_value;
  gboolean ret;
  gint h_size;

  GHashTable *ht = g_hash_table_new_with_string_key();
  assert(ht != NULL);

  g_hash_table_insert(ht, KEY_A, "1");
  g_hash_table_insert(ht, KEY_B, "2");
  hvalue = g_hash_table_lookup(ht, KEY_A);
  assert(g_strequal((char *)hvalue, "1"));

  ret = g_hash_table_lookup_extended(ht, KEY_A, &orig_key, &orig_value);
  assert(ret == TRUE);

  hvalue = g_hash_table_lookup(ht, KEY_B);
  assert(g_strequal((char *)hvalue, "2"));

  g_hash_table_remove(ht, KEY_B);

  h_size = g_hash_table_size(ht);
  assert(h_size == 1);

  g_hash_table_destroy(ht);

  return 0;
}