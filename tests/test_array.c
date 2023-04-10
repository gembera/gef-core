#include "glib.h"
#include <assert.h>

int test_array(int, char *[]) {
  g_mem_record_begin();
  gchar *VAL_A = "aaa";
  gchar *VAL_B = "bbb";
  gchar *VAL_C = "ccc";
  gint g_ptr_int = 0;
  // test Array
  GArray *arr = g_array_new_of(gchar *);
  assert(arr != NULL);
  g_array_set_length(arr, 2);
  gchar **data = g_array(arr, gchar *);

  data[0] = VAL_B;
  assert(g_strequal(g_array_get(arr, gchar *, 0), VAL_B));
  assert(data[1] == NULL);

  g_array_prepend_items(arr, &VAL_A, 1);
  // g_array_prepend may cause reallocate, data should be updated
  data = g_array(arr, gchar *);
  assert(g_strequal(data[0], VAL_A));
  assert(g_strequal(g_array_get(arr, gchar *, 1), VAL_B));
  assert(g_array_length(arr) == 3);

  g_array_set(arr, gchar *, 2, VAL_C);
  assert(g_strequal(g_array_get(arr, gchar *, 2), VAL_C));
  assert(data[0] == VAL_A);
  assert(data[1] == VAL_B);
  assert(data[2] == VAL_C);

  g_array_remove(arr, 1);
  assert(g_array_length(arr) == 2);
  assert(g_strequal(data[0], VAL_A));
  assert(g_strequal(data[1], VAL_C));
  g_array_set_capacity(arr, 100);
  g_array_insert(arr, gchar *, 0, VAL_B);
  assert(g_array_length(arr) == 3);
  assert(g_strequal(data[0], VAL_B));
  assert(g_strequal(data[1], VAL_A));
  assert(g_strequal(data[2], VAL_C));
  g_array_free(arr);

  /*
  GPtrArray *parr = g_ptr_array_new();
  assert(parr != NULL);

  GPtrArray *parr1 = g_ptr_array_new();
  assert(parr1 != NULL);

  g_ptr_array_set_size(parr, 1);
  assert(parr->len == 1);

  g_ptr_array_insert(parr, 0, VAL_A);
  assert(strcmp(VAL_A, (char *)parr->pdata[0]));

  g_ptr_array_insert(parr1, 0, VAL_B);
  g_ptr_array_add_all(parr, parr1);
  assert(strcmp(VAL_B, (char *)parr->pdata[2]) == 0);

  g_ptr_int = g_ptr_array_lookup(parr, VAL_A);
  g_ptr_int = g_ptr_array_index_of(parr, VAL_A);
  assert(g_ptr_int == 0);

  g_ptr_array_remove_index(parr, 0);
  assert(parr->pdata[0] == NULL);

  g_ptr_array_add(parr, VAL_A);
  g_ptr_int = g_ptr_array_index_of(parr, VAL_A);
  g_ptr_int = g_ptr_array_remove(parr, VAL_A);
  assert(g_ptr_int);

  g_ptr_array_free(parr);
  g_ptr_array_free(parr1);


    // test ByteArray
    GByteArray *barr = g_byte_array_new();
    barr = g_byte_array_append(barr, VAL_B, 1);
    assert(strstr((char *)barr->data, VAL_B) != NULL);

    g_byte_array_prepend(barr, VAL_A, 1);
    assert(strstr((char *)barr->data, VAL_A) != NULL);

    g_byte_array_set_length(barr, 3);
    assert(barr->len == 3);

    g_byte_array_free(barr);

  */
  gulong allocated = 0;
  gulong freed = 0;
  g_mem_profile(&allocated, &freed);
  assert(allocated == freed);
  g_mem_record_end();
  return 0;
}
