/*
 * Copyright (c) 2023 Gembera Team
 * https://github.com/gembera
 * License: MIT
 */

#include "glib.h"
#include "gtime.h"
#include "printf.h"
#include <time.h>
#include <unistd.h>

#ifndef HAVE_PRINTF_PUTCHAR

void _putchar(char character) { putchar(character); }

#endif
#define LOG_BUFFER_LEN 1024
static gchar _log_buffer[LOG_BUFFER_LEN];
static GLogCallback _log_callback = NULL;
static gpointer _log_file = NULL;
static GLogLevel _min_level = DEBUG;

void g_log_init(GLogLevel min_level, GLogCallback callback,
                gpointer user_data) {
  _min_level = min_level;
  _log_callback = callback;
  _log_file = user_data;
}

void _g_log(gcstr file, guint line, GLogLevel level, gcstr msg, ...) {
  if (level < _min_level)
    return;
  GDate now = g_date_now();

  gstr level_text = "INFO";
  gstr level_color = "\033[1;34m";
  switch (level) {
  case DEBUG:
    level_text = "DEBUG";
    level_color = "\033[1;32m";
    break;
  case WARN:
    level_text = "WARN";
    level_color = "\033[1;33m";
    break;
  case ERROR:
    level_text = "ERROR";
    level_color = "\033[1;31m";
    break;
  }
  va_list ap;
  va_start(ap, msg);
  gstr buf = _log_buffer;
  gint len = LOG_BUFFER_LEN - 1;
  if (_log_callback)
    snprintf_(buf, len, "\r\n%s\t[%d-%02d-%02d %02d:%02d:%02d.%03d] ",
              level_text, now.month + 1, now.year, now.day, now.hours,
              now.minutes, now.seconds, now.milliseconds);
  else
    snprintf_(buf, len, "\r\n%s%s%s\t[%d-%02d-%02d %02d:%02d:%02d.%03d] ",
              level_color, level_text, "\033[1;0m", now.year, now.month + 1,
              now.day, now.hours, now.minutes, now.seconds, now.milliseconds);

  guint slen = g_len(buf);
  buf += slen;
  len -= slen;
  vsnprintf_(buf, len, msg, ap);
  slen = g_len(buf);
  buf += slen;
  len -= slen;
  if (len > 0 && file) {
    if (_log_callback)
      snprintf_(buf, len, "\t @ %s (%d)", file, line);
    else
      snprintf_(buf, len, "\t %s@%s %s (%d)", level_color, "\033[1;0m", file,
                line);
  }
  va_end(ap);
  if (_log_callback) {
    _log_callback(_log_buffer, _log_file);
  } else {
    fprintf(stderr, "%s", _log_buffer);
  }
}
