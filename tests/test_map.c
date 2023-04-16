#include "glib.h"
#include <assert.h>

typedef struct _Student {
  gstr name;
  guint score;
} Student;

int test_map(int, char *[]) {
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
  Student p5 = {"five", 75};

  GMap *map1 = g_map_new();
  g_map_set(map1, keyb, &p2);
  g_map_set(map1, keya, &p3);
  g_map_set(map1, keyd, pp4);
  g_map_set(map1, keyc, &p1);
  g_map_set(map1, keye, &p5);
  GMapEntry *pair;
  pair = g_map_get(map1, keyc);
  assert(pair && pair->value == &p1);

  g_map_free(map1);
  g_free(pp4);

  // map2 owns the value memory, but not key memory
  GMap *map2 = g_map_new_with(NULL, g_free_callback, NULL);
  g_map_set(map2, keyb, g_new(Student));
  g_map_set(map2, keya, g_new(Student));
  g_map_set(map2, keyd, g_new(Student));
  g_map_set(map2, keya, g_new(Student));
  g_map_set(map2, keyb, g_new(Student));
  g_map_free(map2);

  // map3 owns both key and value memory
  GMap *map3 = g_map_new_with(g_free_callback, g_free_callback, NULL);
  g_map_set(map3, g_dup("a"), g_new(Student));
  g_map_set(map3, g_dup("b"), g_new(Student));
  g_map_set(map3, g_dup("c"), g_new(Student));
  g_map_set(map3, g_dup("a"), g_new(Student));
  g_map_set(map3, g_dup("b"), g_new(Student));
  g_map_free(map3);

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