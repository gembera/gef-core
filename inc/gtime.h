/*
 * Copyright (c) 2023 Gembera Team
 * https://github.com/gembera
 * License: MIT
 */

#ifndef __G_TIME_H__
#define __G_TIME_H__
#include "glib.h"

guint64 g_tick();
guint64 g_utick();
void g_sleep(guint64 time_ms);
void g_usleep(guint64 time_us);

#endif