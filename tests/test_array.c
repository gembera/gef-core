#include "glib.h"
#include <assert.h>

typedef struct _Person {
  gchar *name;
  gint age;
} Person;

gboolean search_callback(gconstpointer item, gconstpointer name) {
  return g_strequal(((Person *)item)->name, (gchar *)name);
};

int test_array(int, char *[]) {
  //g_mem_record(g_mem_record_default_callback);
  g_mem_record_begin();
  gint val1 = 111;
  gint val2 = 222;
  gint val3 = 333;
  gint vals[] = {444, 555, 666};
  gchar *VAL_A = "aaa";
  gchar *VAL_B = "bbb";
  gchar *VAL_C = "ccc";
  gint g_ptr_int = 0;
  // test Array
  GArray *arr = g_array_new_of(gint);
  assert(arr != NULL);
  g_array_set_length(arr, 2);
  gint *data = g_array(arr, gint);

  data[0] = val1;
  // 111, 0
  assert(g_array_get(arr, gint, 0) == val1);
  assert(data[1] == 0);

  g_array_prepend_items(arr, vals, 3);
  // 444, 555, 666, 111, 0
  data = g_array(
      arr,
      gint); // g_array_prepend may cause reallocate, data should be updated
  assert(data[0] == 444);
  assert(g_array_get(arr, gint, 1) == 555);
  assert(data[2] == 666);
  assert(data[3] == val1);
  assert(g_array_length(arr) == 5);

  g_array_set(arr, gint, 4, val2);
  // 444, 555, 666, 111, 222
  assert(data[4] == val2);

  g_array_remove(arr, 1);
  // 444, 666, 111, 222
  // g_array_remove should never cause array reallocation
  assert(g_array_length(arr) == 4);
  assert(data[0] == 444);
  g_array_set_capacity(arr, 100);
  g_array_insert(arr, gint, 3, val3);
  // 444, 666, 111, 333, 222
  data = g_array(
      arr,
      gint); // g_array_set_capacity & g_array_insert may cause reallocation
  assert(g_array_length(arr) == 5);
  g_array_add(arr, gint, 999); // g_array_add may cause reallocation, but in
                               // this case the capacity is enough
  assert(g_array_length(arr) == 6);
  assert(data[0] == 444);
  assert(data[1] == 666);
  assert(data[2] == 111);
  assert(data[3] == val3);
  assert(data[4] == val2);
  assert(data[5] == 999);

  g_array_insert(arr, gint, 30,
                 val3); // nothing will happen since 30 > array current length
  assert(g_array_length(arr) == 6);
  g_array_free(arr);

  Person p1 = {"one", 10};
  Person p2 = {"two", 20};
  Person p3 = {"three", 30};

  GPtrArray *parr1 = g_ptr_array_new();
  assert(parr1 != NULL);

  GPtrArray *parr2 = g_ptr_array_new();
  assert(parr2 != NULL);

  g_ptr_array_set_length(parr1, 1);
  assert(parr1->len == 1);
  assert(parr1->data[0] == NULL);

  g_ptr_array_set(parr1, 0, &p1);
  // parr1 = p1
  assert(parr1->data[0] == &p1);

  g_ptr_array_add(parr2, &p2);
  // parr2 = p2
  g_ptr_array_insert(parr2, 0, &p3);
  // parr2 = p3, p2
  g_ptr_array_append_items(parr1, parr2->data, parr2->len);
  // parr1 = p1, p3, p2
  assert(parr1->data[0] == &p1);
  assert(parr1->data[1] == &p3);
  assert(parr1->data[2] == &p2);
  assert(parr2->data[0] == &p3);
  assert(parr2->data[1] == &p2);

  gint i1 = g_ptr_array_index_of(parr1, &p2);
  gint i2 = g_ptr_array_index_of(parr2, &p2);
  assert(i1 == 2);
  assert(i2 == 1);
  g_ptr_array_remove(parr1, i1);
  i1 = g_ptr_array_index_of(parr1, &p2);
  assert(i1 == -1);
  g_ptr_array_add(parr2, &p1);
  assert(g_ptr_array_length(parr2) == 3);
  assert(parr2->data[2] == &p1);
  gint i3 = g_ptr_array_search(parr2, search_callback, "three");
  assert(i3 == 0);

  g_ptr_array_free(parr1);
  g_ptr_array_free(parr2);

  gulong allocated = 0;
  gulong freed = 0;
  g_mem_profile(&allocated, &freed);
  printf("\nallocated memory: %d  \nfreed memory: %d\n", allocated, freed);
  assert(allocated == freed);
  g_mem_record_end();
  return 0;
}
