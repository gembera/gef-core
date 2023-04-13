#include "gevent.h"
#include "glib.h"
#include <assert.h>

typedef struct _Person {
  gchar *name;
  gint age;
} Person;

static void free_callback(gpointer pointer, gpointer user_data) {
  gint *count = (gint *)user_data;
  (*count)++;
}
static void click_count(GEvent *event, gpointer args, gpointer user_data) {
  gint count = 0;
  g_auto_push_with(free_callback, &count);
  gchar *str = g_auto_of(g_dup("hello"), gchar);
  printf("%s", str);
  g_auto(g_new(Person));
  GArray *persons = (GArray *)g_auto_with(g_array_new_of(Person),
                                          (GFreeCallback)g_array_free);
  g_auto_pop();
  assert(count == 3);
}
static void bad_func(gint *count) {
  g_auto_push_with(free_callback, count);
  g_auto(g_dup("bad func called"));
}
static void click_toggle(GEvent *event, gpointer args, gpointer user_data) {
  gint count = 0;
  gint mark = g_auto_push_with(free_callback, &count);
  g_auto(g_new(Person));
  g_auto_with(g_ptr_array_new(), (GFreeCallback)g_ptr_array_free);
  bad_func(&count);
  g_auto_pop_to(mark);
  assert(count == 3);
}

int test_auto(int, char *[]) {
  // g_mem_record(g_mem_record_default_callback);
  g_mem_record_begin();

  GEvent *click =
      (GEvent *)g_auto_with(g_event_new(), (GFreeCallback)g_event_free);
  g_event_add_listener(click, click_count, NULL);
  g_event_add_listener(click, click_toggle, NULL);
  g_event_fire(click, NULL);

  g_auto_free();
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
