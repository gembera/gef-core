#include "glib.h"
#include <assert.h>

static void print_and_count_callback(GSList *self, GSListNode *item,
                                     gconstpointer user_data) {
  gchar *str = (gchar *)item->data;
  gint *index = (gint *)user_data;
  printf("\n%d:%s\n", *index, str);
  (*index)++;
}

static gbool search_handler(GSList *self, GSListNode *item,
                            gconstpointer user_data) {
  gchar *v1 = (gchar *)item->data;
  gchar *v2 = (gchar *)user_data;
  return g_strequal(v1, v2);
}

int test_slist(int, char *[]) {
  g_mem_record(g_mem_record_default_callback);
  g_mem_record_begin();

  gint i, i1, i2;
  gchar *str1 = "one";
  gchar *str2 = "two";
  gchar *str3 = "three";
  gchar *str4 = "four";
  gchar *str5 = "five";
  gchar *str6 = g_strdup("dynamic");
  GSList *list1 = g_slist_new();
  g_slist_append(list1, str1);
  g_slist_append(list1, str2);
  g_slist_append(list1, str3);
  g_slist_prepend(list1, str4);
  g_slist_prepend(list1, str5);
  g_slist_prepend(list1, str6);
  // 6, 5, 4, 1, 2, 3
  i = 0;
  g_slist_visit(list1, print_and_count_callback, &i);
  assert(i == 6);
  assert(g_slist_size(list1) == 6);
  assert(g_slist_get(list1, 1)->data == str5);
  assert(g_slist_get(list1, 4)->data == str2);

  GSListNode *node = g_slist_search(list1, search_handler, "four");
  assert(node && node->data == str4);

  g_slist_remove(list1, str4);
  g_slist_remove(list1, str6);
  g_slist_remove(list1, str3);
  // 5, 1, 2
  assert(g_slist_size(list1) == 3);
  assert(g_slist_index_of(list1, str1) == 1);

  g_free(str6);
  g_slist_free(list1);

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
