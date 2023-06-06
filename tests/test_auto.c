#include "gevent.h"
#include "glib.h"
#include <assert.h>
#define AC1 "auto_container1"
#define AC2 "auto_container2"
typedef struct _Person {
  gstr name;
  gint age;
} Person;

static void free_callback(gpointer pointer, gpointer user_data) {
  gint *count = (gint *)user_data;
  (*count)++;
}
static void click_count(GEvent *event, gpointer args, gpointer user_data) {
  gstr str = (gstr)g_auto(g_dup("hello"), AC1);
  printf("%s", str);
  g_auto(g_new(Person), AC1);
  GArray *persons = (GArray *)g_auto_with(g_array_new(Person),
                                          (GFreeCallback)g_array_free, AC1);
  g_auto_container_free(AC1);
}
static void bad_func() {
  g_auto(g_dup("bad func called"), AC2);
  g_auto_with(g_dup("it will be remained at root stack 0"), g_free_callback,
              AC2);
}
static void click_toggle(GEvent *event, gpointer args, gpointer user_data) {
  g_auto(g_new(Person), AC2);
  g_auto_with(g_ptr_array_new(), (GFreeCallback)g_ptr_array_free, AC2);
  bad_func();
  g_auto_container_free(AC2);
}

int test_auto(int argc, char *argv[]) {
  g_mem_record(g_mem_record_default_callback);
  g_mem_record_begin();

  GEvent *click =
      (GEvent *)g_auto_with(g_event_new(), (GFreeCallback)g_event_free, NULL);
  g_event_add_listener(click, click_count, NULL);
  g_event_add_listener(click, click_toggle, NULL);
  g_event_fire(click, NULL);

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
