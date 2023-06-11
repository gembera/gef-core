#include "glib.h"
#include "gstring.h"
#include <assert.h>

int test_string(int argc, char *argv[]) {
  g_mem_record(g_mem_record_default_callback);
  g_mem_record_begin();

  GString *s1 = g_string_new();
  g_string_printf(s1, "%d", 123);
  assert(g_equal(s1->value, "123"));
  g_string_reset(s1);
  g_string_append(s1, "Hello");
  assert(g_equal(s1->value, "Hello"));
  g_string_append(s1, " World ");
  assert(g_equal(s1->value, "Hello World "));
  g_string_append_with(s1, " from Gembera" + 6, 7);
  assert(g_equal(s1->value, "Hello World Gembera"));
  g_string_reset(s1);
  g_string_printf(s1, "%f", 123.456789);
  assert(g_equal(s1->value, "123.456789"));
  g_string_reset(s1);
  g_string_printf(s1, "%f", 123.4);
  assert(g_equal(s1->value, "123.4"));
  g_string_reset(s1);
  g_string_printf(s1, "%f", 3.333);
  assert(g_equal(s1->value, "3.333"));
  g_string_reset(s1);
  g_string_printf(s1, "%f", 123.0);
  assert(g_equal(s1->value, "123"));
  g_string_reset(s1);
  g_string_printf(s1, "%3.1f", 123.456789);
  assert(g_equal(s1->value, "123.5"));
  g_string_reset(s1);
  g_string_printf(s1, "%e", 123.456789);
  assert(g_equal(s1->value, "1.234568e+02"));
  g_string_reset(s1);
  g_string_printf(s1, "{name:'%s', stars: %d, double: %g}", "Gembera", 5, 123.456789);
  assert(g_equal(s1->value, "{name:'Gembera', stars: 5, double: 123.457}"));
  g_string_free(s1);


  s1 = g_string_new_with(6);
  g_string_printf(s1, "%d", 123);
  assert(g_equal(s1->value, "123"));
  g_string_reset(s1);
  g_string_append(s1, "Hello");
  assert(g_equal(s1->value, "Hello"));
  g_string_append(s1, " World");
  assert(g_equal(s1->value, "Hello "));
  g_string_reset(s1);
  g_string_printf(s1, "%f", 123.456789);
  assert(g_equal(s1->value, "123.45"));
  g_string_reset(s1);
  g_string_printf(s1, "%3.1f", 123.456789);
  assert(g_equal(s1->value, "123.5"));
  g_string_reset(s1);
  g_string_printf(s1, "%e", 123.456789);
  assert(g_equal(s1->value, "1.2345"));
  g_string_reset(s1);
  g_string_printf(s1, "{name:'%s', stars: %d, double: %g}", "Gembera", 5, 123.456789);
  assert(g_equal(s1->value, "{name:"));
  g_string_free(s1);

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
