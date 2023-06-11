/*
 * Copyright (c) 2023 Gembera Team
 * https://github.com/gembera
 * License: MIT
 */

#include "glib.h"
#include <time.h>
#include <unistd.h>

#ifndef HAVE_PRINTF_PUTCHAR

void _putchar(char character) { putchar(character); }

#endif

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