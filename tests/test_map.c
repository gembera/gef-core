#include "glib.h"
#include "gmemorystream.h"
#include <assert.h>

typedef struct _Student {
  gstr name;
  guint score;
} Student;
void g_student_free(gpointer p) {
  Student *s = (Student *)p;
  if (s) {
    if (s->name)
      g_free(s->name);
    g_free(s);
  }
}
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
  GMap *map2 = g_map_new_with(NULL, NULL, g_student_free, NULL);
  Student *sb = g_new(Student);
  sb->name = g_dup("name b");
  sb->score = 80;
  g_map_set(map2, keyb, sb);
  g_map_set(map2, keya, g_new(Student));
  g_map_set(map2, keyd, g_new(Student));
  Student *sa = g_new(Student);
  sa->name = g_dup("name a");
  sa->score = 99;
  g_map_set(map2, keya, sa);
  Student *s2 = (Student *)g_map_get(map2, keyb);
  assert(s2 && s2->score == sb->score && g_equal(s2->name, sb->name));
  g_map_set(map2, keyb, g_new(Student));
  Student *s1 = (Student *)g_map_get(map2, keya);
  assert(s1 && s1->score == sa->score && g_equal(s1->name, sa->name));
  s2 = (Student *)g_map_get(map2, keyb);
  assert(s2 && s2->score == 0 && s2->name == NULL);
  assert(g_map_has(map2, keyd));
  assert(g_map_size(map2) == 3);
  g_map_remove(map2, keya);
  assert(g_map_size(map2) == 2);
  g_map_free(map2);

  // map3 owns both key and value memory
  GMap *map3 = g_map_new(g_student_free);
  g_map_set(map3, "a", g_new(Student));
  g_map_set(map3, "b", g_new(Student));
  g_map_set(map3, "c", g_new(Student));
  g_map_set(map3, "a", g_new(Student));
  g_map_set(map3, "b", g_new(Student));
  g_map_set_with(map3, "b", g_object_new(GMemoryStream), g_object_free);
  g_map_free(map3);

  GMap *map4 = g_imap_new(NULL);
  g_imap_set(map4, 2, "second");
  g_imap_set(map4, 3, "third");
  g_imap_set(map4, 1, "first");
  g_imap_set(map4, 4, "fouth");
  g_imap_set(map4, 2, "modified second");
  gcstr val3 = g_imap_get(map4, 3);
  assert(g_equal(val3, "third"));
  assert(g_imap_has(map4, 1));
  assert(g_imap_size(map4) == 4);
  g_imap_remove(map4, 1);
  assert(g_imap_size(map4) == 3);
  assert(g_equal(g_imap_get(map4, 2), "modified second"));
  g_imap_free(map4);

  g_auto_free();
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