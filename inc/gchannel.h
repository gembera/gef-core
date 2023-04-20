/*
 * Copyright (c) 2023 Gembera Team
 * https://github.com/gembera
 * License: MIT
 */

#ifndef _G_CHANNEL_H_
#define _G_CHANNEL_H_
#include "glib.h"
#include "gevent.h"
#include "gvalue.h"

typedef struct {
  GArray *items; // Array of GValue
  guint max;
  gbool closed;
  GEvent * on_read;
  GEvent * on_write;
  GEvent * on_closed;
  GEvent * on_error;
} GChannel;

#define G_TYPE_CHANNEL_CLOSED 99
#define g_value_is_channel_closed(val) g_value_is(val, G_TYPE_CHANNEL_CLOSED)

GChannel *g_channel_new( guint max);
gbool g_channel_write(GChannel *self, GValue* item);
gbool g_channel_read(GChannel *self, GValue* item);
gbool g_channel_is_closed(GChannel *self);
void g_channel_close(GChannel *self);
void g_channel_error(GChannel *self, gint err_code, gstr err_msg);
guint g_channel_readable_size();
guint g_channel_writable_size();
void g_channel_free(GChannel *self);

#endif