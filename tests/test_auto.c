#include "gevent.h"
#include "glib.h"
#include <assert.h>

typedef struct _Person {
  gstr name;
  gint age;
} Person;

static void free_callback(gpointer pointer, gpointer user_data) {
  gint *count = (gint *)user_data;
  (*count)++;
}
static void click_count(GEvent *event, gpointer args, gpointer user_data) {
  gint count = 0;
  gint si = g_auto_push_with(free_callback, &count);
  assert(si == 1);
  gstr str = (gstr)g_auto(g_dup("hello"));
  printf("%s", str);
  g_auto(g_new(Person));
  GArray *persons = (GArray *)g_auto_with(g_array_new(Person),
                                          (GFreeCallback)g_array_free, -1);
  g_auto_pop();
  assert(count == 3);
}
static void bad_func(gint *count) {
  gint si = g_auto_push_with(free_callback, count);
  assert(si == 2);
  g_auto(g_dup("bad func called"));
  g_auto_with(g_dup("it will be remained at root stack 0"), g_free_callback, 0);
  assert(g_auto_current_stack() == 2);
}
static void click_toggle(GEvent *event, gpointer args, gpointer user_data) {
  gint count = 0;
  gint si = g_auto_push_with(free_callback, &count);
  assert(si == 1);
  g_auto(g_new(Person));
  g_auto_with(g_ptr_array_new(), (GFreeCallback)g_ptr_array_free, -1);
  assert(g_auto_current_stack() == 1);
  bad_func(&count);
  assert(g_auto_current_stack() == 2);
  g_auto_pop_to(si);
  assert(count == 3);
}

int test_auto(int, char *[]) {
  // g_mem_record(g_mem_record_default_callback);
  g_mem_record_begin();

  GEvent *click =
      (GEvent *)g_auto_with(g_event_new(), (GFreeCallback)g_event_free, -1);
  assert(g_auto_current_stack() == 0);
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
