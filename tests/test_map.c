#include "glib.h"
#include "gmemorystream.h"
#include <assert.h>

typedef struct _Student {
  gstr name;
  guint score;
} Student;

static void map_visit_callback(GMap *self, gpointer key, gpointer value,
                               gpointer user_data) {
  gint *count = (gint *)user_data;
  if (((Student *)value)->score >= 80)
    (*count)++;
};

int test_map(int argc, char *argv[]) {
  g_mem_record(g_mem_record_default_callback);
  g_mem_record_begin();

  gstr keya = "aaa";
  gstr keyb = "bbb";
  gstr keyc = "ccc";
  gstr keyd = "ddd";
  gstr keye = "eee";

  Student p1 = {"one", 80};
  Student p2 = {"two", 90};
  Student p3 = {"three", 75};
  Student *pp4 = g_new(Student);
  pp4->name = "four";
  pp4->score = 100;
  Student p5 = {"five", 60};

  GMap *map1 = g_map_new(NULL);
  g_map_set(map1, keyb, &p2);
  g_map_set(map1, keya, &p3);
  g_map_set(map1, keyd, pp4);
  g_map_set(map1, keyc, &p1);
  g_map_set(map1, keye, &p5);
  GMapEntry *pair;
  pair = g_map_get_entry(map1, keyc);
  assert(pair && pair->value == &p1);
  assert(g_map_get(map1, keyc) == &p1);
  gint count = 0;
  g_map_visit(map1, map_visit_callback, &count);
  assert(count == 3);
  g_map_free(map1);
  g_free(pp4);

  // map2 owns the value memory, but not key memory
  GMap *map2 = g_map_new_with(NULL, NULL, g_free_callback, NULL);
  g_map_set(map2, keyb, g_new(Student));
  g_map_set(map2, keya, g_new(Student));
  g_map_set(map2, keyd, g_new(Student));
  g_map_set(map2, keya, g_new(Student));
  g_map_set(map2, keyb, g_new(Student));
  g_map_free(map2);

  // map3 owns both key and value memory
  GMap *map3 = g_map_new( g_free_callback);
  g_map_set(map3, "a", g_new(Student));
  g_map_set(map3, "b", g_new(Student));
  g_map_set(map3, "c", g_new(Student));
  g_map_set(map3, "a", g_new(Student));
  g_map_set(map3, "b", g_new(Student));
  //g_map_set_with(map3, "b", g_object_new(GMemoryStream), NULL, g_object_free);
  g_map_free(map3);

  g_auto_free();
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