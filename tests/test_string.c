#include "glib.h"
#include "gstring.h"
#include <assert.h>

int test_string(int argc, char *argv[]) {
  g_mem_record(g_mem_record_default_callback);
  g_mem_record_begin();

  GString *s1 = g_string_new();
  g_string_append_int(s1, 123);
  assert(g_equal(s1->value, "123"));
  g_string_reset(s1);
  g_string_append_str(s1, "Hello");
  assert(g_equal(s1->value, "Hello"));
  g_string_append_str(s1, " World");
  assert(g_equal(s1->value, "Hello World"));
  g_string_reset(s1);
  g_string_append_float(s1, 123.456789, 10);
  assert(g_equal(s1->value, "123.456789"));
  g_string_reset(s1);
  g_string_append_float(s1, 123.456789, 4);
  assert(g_equal(s1->value, "123.5"));
  g_string_reset(s1);
  g_string_append_float(s1, 123.456789, 2);
  assert(g_equal(s1->value, "1.2e+02"));
  g_string_reset(s1);
  g_string_append(s1, "{name:'%s', stars: %d, double: %g}", "Gembera", 5, 123.456789);
  assert(g_equal(s1->value, "{name:'Gembera', stars: 5, double: 123.457}"));
  g_string_free(s1);

  s1 = g_string_new_with(6);
  g_string_append_int(s1, 123);
  assert(g_equal(s1->value, "123"));
  g_string_reset(s1);
  g_string_append_str(s1, "Hello");
  assert(g_equal(s1->value, "Hello"));
  g_string_append_str(s1, " World");
  assert(g_equal(s1->value, "Hello "));
  g_string_reset(s1);
  g_string_append_float(s1, 123.456789, 10);
  assert(g_equal(s1->value, "123.45"));
  g_string_reset(s1);
  g_string_append_float(s1, 123.456789, 4);
  assert(g_equal(s1->value, "123.5"));
  g_string_reset(s1);
  g_string_append_float(s1, 123.456789, 2);
  assert(g_equal(s1->value, "1.2e+0"));
  g_string_reset(s1);
  g_string_append(s1, "{name:'%s', stars: %d, double: %g}", "Gembera", 5, 123.456789);
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
