#include "glib.h"
#include <assert.h>

typedef struct _Student {
  gchar *name;
  guint score;
} Student;

int test_map(int, char *[]) {
  g_mem_record(g_mem_record_default_callback);
  g_mem_record_begin();

  gchar *keya = "aaa";
  gchar *keyb = "bbb";
  gchar *keyc = "ccc";
  gchar *keyd = "ddd";
  gchar *keye = "eee";

  Student p1 = {"one", 80};
  Student p2 = {"two", 90};
  Student p3 = {"three", 75};
  Student *pp4 = g_new(Student);
  pp4->name = "four";
  pp4->score = 100;

  GMap *map1 = g_map_new_str();
  g_map_set(map1, keya, &p3);
  g_map_set(map1, keyb, &p2);
  g_map_set(map1, keyc, &p1);
  g_map_set(map1, keyd, pp4);
  GMapPair * pair;
  pair = g_map_get(map1, keyc);
  assert(pair && pair->value == &p1);

  g_map_free(map1);
  g_free(pp4);
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