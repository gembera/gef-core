/*
 * Copyright (c) 2023 Gembera Team
 * https://github.com/gembera
 * License: MIT
 */

#include "glib.h"
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
  gstr level_text = "INFO";
  switch (level) {
  case DEBUG:
    level_text = "DEBUG";
    break;
  case WARN:
    level_text = "WARN";
    break;
  case ERROR:
    level_text = "ERROR";
    break;
  }
  va_list ap;
  va_start(ap, msg);
  gstr buf = _log_buffer;
  gint len = LOG_BUFFER_LEN - 1;
  snprintf_(buf, len, "\r\n%s : ", level_text);
  guint slen = g_len(buf);
  buf += slen;
  len -= slen;
  vsnprintf_(buf, len, msg, ap);
  slen = g_len(buf);
  buf += slen;
  len -= slen;
  if (len > 0 && file)
    snprintf_(buf, len, "\t in %s (%d)", file, line);
  va_end(ap);
  if (_log_callback) {
    _log_callback(_log_buffer, _log_file);
  } else {
    fprintf(stderr, _log_buffer);
  }
}
