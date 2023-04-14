/*
 * Copyright (c) 2023 Gembera Team
 * https://github.com/gembera
 * License: MIT
 */

#include "glib.h"
#include <time.h>
#include <unistd.h>

gulong g_tick_count() {
  struct timespec tv;
  clock_gettime(CLOCK_MONOTONIC, &tv);
  return (gulong)tv.tv_sec * 1000 + (gulong)tv.tv_nsec / 1000000;
}

void g_sleep(gulong time_ms) { usleep(time_ms * 1000); }

static gint rand_init = FALSE;
gint g_rand(gint max) {
  if (!rand_init) {
    rand_init = TRUE;
    srand((unsigned int)g_tick_count());
  }
  return rand() % max;
}

static char *g_log___file__;
static int g_log___line__;
static char *g_log___func__;
static char *g_log_level;
int g_log_enabled(char *file, int line, char *func, char *level) {
  g_log___file__ = file;
  g_log___line__ = line;
  g_log___func__ = func;
  g_log_level = level;
  // return _vm_log_module(g_log___file__, g_log___line__);
  return 0;
}
void g_log(char *fmt, ...) {
  /*

  va_list args, args2;

  va_start(args, fmt);
  va_start(args2, fmt);

  g_vsprintf(fmt, &args, &args2);

  va_end(args);
  va_end(args2);

  g_print("\n%s : %s\n\t in %s (%s #%d)", g_log_level, global_string_buf,
          g_log___func__, g_log___file__, g_log___line__);
  */
}