#include "gevent.h"
#include "glib.h"
#include <assert.h>

static void click_count(GEvent *event, gpointer args, gpointer user_data) {
  gint *num = (gint *)args;
  gint *count = (gint *)user_data;
  *count += *num;
}

static void click_toggle(GEvent *event, gpointer args, gpointer user_data) {
  gbool *enabled = (gbool *)user_data;
  *enabled = !*enabled;

  // can not refire an event during firing, no effects
  g_event_fire(event, args);
}

static void click_remove_listener(GEvent *event, gpointer args,
                                  gpointer user_data) {
  gint *token = (gint *)user_data;
  g_event_remove_listener(event, *token);
}

int test_event(int argc, char *argv[]) {
  // g_mem_record(g_mem_record_default_callback);
  g_mem_record_begin();
  gint count = 0;
  gbool enabled = FALSE;
  GEvent *click = g_event_new();
  gint t1 = g_event_add_listener(click, click_count, &count);
  gint t2 = g_event_add_listener(click, click_toggle, &enabled);
  gint i = 1;
  // count , toggle
  g_event_fire(click, &i);
  assert(enabled);
  assert(count == 1);

  i = 6;
  // count , toggle
  g_event_fire(click, &i);
  assert(!enabled);
  assert(count == 7);

  g_event_remove_listener(click, t2);
  assert(g_array_size(click->listeners) == 1);
  i = 10;
  // count
  g_event_fire(click, &i);
  assert(!enabled);
  assert(count == 17);

  gint t3 = g_event_add_listener(click, click_remove_listener, &t1);
  assert(g_array_size(click->listeners) == 2);
  i = 11;
  // count , remove
  g_event_fire(click, &i);
  // remove
  assert(!enabled);
  assert(count == 28);
  assert(g_array_size(click->listeners) == 1);

  t2 = g_event_add_listener(click, click_toggle, &enabled);
  assert(g_array_size(click->listeners) == 2);
  i = 12;
  // remove, toggle
  g_event_fire(click, &i);
  assert(enabled);
  assert(count == 28);
  assert(g_array_size(click->listeners) == 2);

  g_event_free(click);

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
