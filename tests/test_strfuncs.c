#include "glib.h"
#include <assert.h>

int test_strfuncs(int argc, char *argv[]) {
  g_mem_record(g_mem_record_default_callback);
  g_mem_record_begin();

  gcstr hello = "Hello";
  gcstr world = "World";
  gchar buffer[100];
  assert(g_len(hello) == 5);
  g_cpy(buffer, hello);
  assert(g_equal(buffer, hello));
  g_ncpy(buffer, world, 2);
  assert(g_equal(buffer, "Wollo"));
  assert(g_num("12345") == 12345);
  assert(g_parse("FF", 0, 16) == 255);
  g_cpy(buffer, "<123>");
  g_delimit(buffer, "<>", '-');
  assert(g_equal(buffer, "-123-"));
  gstr str = g_dup("GEF");
  assert(g_equal(str, "GEF"));
  g_down(str);
  assert(g_equal(str, "gef"));
  g_up(str);
  assert(g_equal(str, "GEF"));
  g_reverse(str);
  assert(g_equal(str, "FEG"));
  g_free(str);

  str = g_format("%s%s%s", "Why ", "gembera ", "?");
  assert(g_equal(str, "Why gembera ?"));
  assert(g_start_with(str, "Why"));
  assert(!g_start_with(str, "why"));
  assert(g_end_with(str, " ?"));
  assert(!g_end_with(str, "ggg"));
  assert(g_index_of(str, "Why", 0) == 0);
  assert(g_index_of(str, "gembera", 0) == 4);
  assert(g_index_of(str, "xxx", 0) == -1);
  assert(g_last_index_of(str, "hy") == 1);
  assert(g_last_index_of(str, "shy") == -1);
  g_free(str);

  assert(g_cmp("aaa", "bbb") < 0);
  assert(g_cmp("aaa", "AAA") > 0);
  assert(g_cmp("aaa", "aaa") == 0);
  str = g_substring("This is gembera project.", 8, 15);
  assert(g_equal(str, "gembera project"));
  g_free(str);
  assert(g_hex('f') == 15);
  assert(g_hex('x') == -1);
  str = g_replace("Hi GEF!", "Hi", "Hello");
  assert(g_equal(str, "Hello GEF!"));
  str = g_replace_free(str, "GEF", "gembera");
  assert(g_equal(str, "Hello gembera!"));
  str = g_replace_free(str, "e", "");
  assert(g_equal(str, "Hllo gmbra!"));
  g_free(str);
  str = g_trim(" \t \rgembera  \n");
  assert(g_equal(str, "gembera"));
  g_free(str);
  g_format_to(buffer, sizeof(buffer), "%d + %d = %d", 1, 2, 3);
  assert(g_equal(buffer, "1 + 2 = 3"));
  g_format_to(buffer, sizeof(buffer), "%d + %d = %d", 8, 8, 16);
  assert(g_equal(buffer, "8 + 8 = 16"));

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
