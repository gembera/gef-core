#include "glib.h"
#include <assert.h>

typedef struct _Person {
  gstr name;
  gint age;
} Person;

static void array_visit_callback(GArray *self, guint index, gcpointer item,
                                 gcpointer user_data) {
  gint v = *(gint *)item;
  gint *count = (gint *)user_data;
  if (v > 300)
    (*count)++;
};

static void ptr_array_visit_callback(GPtrArray *self, guint index,
                                     gcpointer item, gcpointer user_data) {
  gint *age_gt_20_count = (gint *)user_data;
  if (((Person *)item)->age > 20)
    (*age_gt_20_count)++;
};

int test_array(int argc, char *argv[]) {
  // g_mem_record(g_mem_record_default_callback);
  g_mem_record_begin();
  gint i, i1, i2, i3, i4;
  gint val1 = 111;
  gint val2 = 222;
  gint val3 = 333;
  gint vals[] = {444, 555, 666};
  gint g_ptr_int = 0;
  // test Array
  GArray *arr = g_array_new(gint);
  assert(arr != NULL);
  g_array_set_size(arr, 2);
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
  assert(*(gint *)g_array_get_ref(arr, 2) == 666);
  assert(data[2] == 666);
  assert(data[3] == val1);
  assert(g_array_size(arr) == 5);
  gint two_items[2];
  g_array_copy(arr, two_items, 2, 2);
  assert(two_items[0] == 666);
  assert(two_items[1] == val1);

  g_array_set(arr, gint, 4, val2);
  // 444, 555, 666, 111, 222
  assert(data[4] == val2);

  i = 0;
  g_array_visit(arr, array_visit_callback, &i);
  assert(i == 3);

  g_array_remove(arr, 1);
  // 444, 666, 111, 222
  // g_array_remove should never cause array reallocation
  assert(g_array_size(arr) == 4);
  assert(data[0] == 444);
  g_array_set_capacity(arr, 100);
  g_array_insert(arr, gint, 3, val3);
  // 444, 666, 111, 333, 222
  data = g_array(
      arr,
      gint); // g_array_set_capacity & g_array_insert may cause reallocation
  assert(g_array_size(arr) == 5);
  g_array_add(arr, gint, 999); // g_array_add may cause reallocation, but in
                               // this case the capacity is enough
  assert(g_array_size(arr) == 6);
  assert(data[0] == 444);
  assert(data[1] == 666);
  assert(data[2] == 111);
  assert(data[3] == val3);
  assert(data[4] == val2);
  assert(data[5] == 999);

  g_array_insert(arr, gint, 30,
                 val3); // nothing will happen since 30 > array current length
  assert(g_array_size(arr) == 6);
  g_array_free(arr);

  Person p1 = {"one", 10};
  Person p2 = {"two", 20};
  Person p3 = {"three", 30};
  Person *pp4 = g_new(Person);
  pp4->name = "four";
  pp4->age = 40;

  GPtrArray *parr1 = g_ptr_array_new();
  assert(parr1 != NULL);

  GPtrArray *parr2 = g_ptr_array_new();
  assert(parr2 != NULL);

  g_ptr_array_set_size(parr1, 1);
  assert(parr1->size == 1);
  assert(parr1->data[0] == NULL);

  g_ptr_array_set(parr1, 0, &p1);
  // parr1 = p1
  assert(parr1->data[0] == &p1);

  g_ptr_array_add(parr2, &p2);
  // parr2 = p2
  g_ptr_array_insert(parr2, 0, &p3);
  // parr2 = p3, p2
  g_ptr_array_append_items(parr1, parr2->data, parr2->size);
  // parr1 = p1, p3, p2
  assert(parr1->data[0] == &p1);
  assert(parr1->data[1] == &p3);
  assert(parr1->data[2] == &p2);
  assert(parr2->data[0] == &p3);
  assert(parr2->data[1] == &p2);

  i1 = g_ptr_array_index_of(parr1, &p2);
  i2 = g_ptr_array_index_of(parr2, &p2);
  assert(i1 == 2);
  assert(i2 == 1);

  g_ptr_array_remove(parr1, i1);
  i1 = g_ptr_array_index_of(parr1, &p2);
  assert(i1 == -1);

  g_ptr_array_add(parr2, &p1);
  assert(g_ptr_array_size(parr2) == 3);
  assert(parr2->data[2] == &p1);

  g_ptr_array_add(parr2, pp4);
  assert(g_ptr_array_size(parr2) == 4);
  assert(parr2->data[3] == pp4);

  i4 = 0;
  g_ptr_array_visit(parr2, ptr_array_visit_callback, &i4);
  assert(i4 == 2);

  g_free(pp4);
  g_ptr_array_free(parr1);
  g_ptr_array_free(parr2);

  GPtrArray *parr3 = g_ptr_array_new_with(g_free_callback);
  g_ptr_array_add(parr3, g_new(Person));
  g_ptr_array_add(parr3, g_new(Person));
  g_ptr_array_add(parr3, g_new(Person));
  g_ptr_array_add(parr3, g_new(Person));
  g_ptr_array_add(parr3, g_new(Person));
  g_ptr_array_free(parr3); // it should free all items too

  gulong allocated = 0;
  gulong freed = 0;
  gulong peak = 0;
  g_mem_profile(&allocated, &freed, &peak);
  g_mem_record_end();
  printf("\r\nallocated memory: %ld  \r\nfreed memory: %ld  \r\npeak memory: "
         "%ld\r\n",
         allocated, freed, peak);
  assert(allocated == freed);
  return 0;
}
