/*
 * Copyright (c) 2023 Gembera Team
 * https://github.com/gembera
 * License: MIT
 */

#ifndef __G_TIME_H__
#define __G_TIME_H__
#include "glib.h"

typedef struct {
  int milliseconds;     /* milliseconds after the second [0-999] */
  int seconds;          /* seconds after the minute [0-59] */
  int minutes;          /* minutes after the hour [0-59] */
  int hours;            /* hours since midnight [0-23] */
  int day;              /* day of the month [1-31] */
  int month;            /* months since January [0-11] */
  int year;             /* years */
  long timezone_offset; /* offset from UTC in minutes */
} GDate;

guint64 g_tick();
guint64 g_utick();
void g_sleep(guint64 time_ms);
void g_usleep(guint64 time_us);
GDate g_date_now();

#endif