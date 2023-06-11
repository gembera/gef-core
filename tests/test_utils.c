#include "glib.h"
#include <assert.h>

gstr check_score(gint score) {
  gstr good = g_dup("Good!");
  g_return_val_if_fail(score >= 0 && score <= 100, g_dup("Out of range!"),
                       g_free(good));
  if (score > 90)
    return good;
  else {
    gstr bad = g_dup("Bad!");
    g_return_val_if_fail(score > 10, g_dup("Unbelievable!"), g_free(good),
                         g_free(bad));
    g_free(good);
    return bad;
  }
}

void check_next(gint *result) {
  gstr next = g_dup("next");
  g_return_if_fail(*result > 0, g_free(next));
  g_free(next);
  (*result)++;
}
int test_utils(int argc, char *argv[]) {
  g_mem_record(g_mem_record_default_callback);
  g_mem_record_begin();

  gstr result = check_score(-10);
  assert(g_equal(result, "Out of range!"));
  g_free(result);

  result = check_score(1000);
  assert(g_equal(result, "Out of range!"));
  g_free(result);

  result = check_score(3);
  assert(g_equal(result, "Unbelievable!"));
  g_free(result);

  result = check_score(50);
  assert(g_equal(result, "Bad!"));
  g_free(result);

  result = check_score(99);
  assert(g_equal(result, "Good!"));
  g_free(result);

  gint num = -10;
  check_next(&num);
  assert(num == -10);
  num = 100;
  check_next(&num);
  assert(num == 101);

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
