#include "glib.h"
#include <assert.h>

static void print_and_count_callback(GList *self, GListNode *item,
                                     gconstpointer user_data) {
  gchar *str = (gchar *)item->data;
  gint *index = (gint *)user_data;
  printf("\n%d:%s\n", *index, str);
  (*index)++;
}

static gbool search_handler(GList *self, GListNode *item,
                            gconstpointer user_data) {
  gchar *v1 = (gchar *)item->data;
  gchar *v2 = (gchar *)user_data;
  return g_equal(v1, v2);
}

int test_list(int, char *[]) {
  g_mem_record(g_mem_record_default_callback);
  g_mem_record_begin();

  gint i, i1, i2;
  gchar *str1 = "one";
  gchar *str2 = "two";
  gchar *str3 = "three";
  gchar *str4 = "four";
  gchar *str5 = "five";
  gchar *str6 = g_dup("dynamic");
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

  GListNode *node = g_list_search(list1, search_handler, "four");
  assert(node && node->data == str4);

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
  printf("\nallocated memory: %d  \nfreed memory: %d\npeak memory: %d\n",
         allocated, freed, peak);
  assert(allocated == freed);
  g_mem_record_end();
  return 0;
}
