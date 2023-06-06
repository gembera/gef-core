#include "glib.h"
#include <assert.h>

static void print_and_count_callback(GList *self, GListNode *item,
                                     gcpointer user_data) {
  gstr str = (gstr )item->data;
  gint *index = (gint *)user_data;
  printf("\n%d:%s\n", *index, str);
  (*index)++;
}

int test_list(int argc, char *argv[]) {
  g_mem_record(g_mem_record_default_callback);
  g_mem_record_begin();

  gint i, i1, i2;
  gstr str1 = "one";
  gstr str2 = "two";
  gstr str3 = "three";
  gstr str4 = "four";
  gstr str5 = "five";
  gstr str6 = g_dup("dynamic");
  GList *list1 = g_list_new();
  g_list_append(list1, str1);
  g_list_append(list1, str2);
  g_list_append(list1, str3);
  g_list_prepend(list1, str4);
  g_list_prepend(list1, str5);
  g_list_prepend(list1, str6);
  // 6, 5, 4, 1, 2, 3
  i = 0;
  g_list_visit(list1, print_and_count_callback, &i);
  assert(i == 6);
  assert(g_list_size(list1) == 6);
  assert(g_list_get(list1, 1)->data == str5);
  assert(g_list_get(list1, 4)->data == str2);

  g_list_remove(list1, str4);
  g_list_remove(list1, str6);
  g_list_remove(list1, str3);
  // 5, 1, 2
  assert(g_list_size(list1) == 3);
  assert(g_list_index_of(list1, str1) == 1);

  g_free(str6);
  g_list_free(list1);

  GList *list2 = g_list_new_with(g_free_callback);
  g_list_append(list2, g_dup("one"));
  g_list_append(list2, g_dup("two"));
  g_list_append(list2, g_dup("three"));
  g_list_free(list2); // it will free list node data too

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
