/*
 * Copyright (c) 2023 Gembera Team
 * https://github.com/gembera
 * License: MIT
 */

#ifndef __G_CHANNEL_H__
#define __G_CHANNEL_H__
#include "glib.h"
#include "gevent.h"
#include "gvalue.h"

typedef struct {
  GArray *items; // Array of GValue
  guint max;
  gbool closed;
  gbool auto_discard; // auto discard the first readable value when the buffer is full
  GEvent * on_read;
  GEvent * on_write;
  GEvent * on_close;
  GEvent * on_error;
  GEvent * on_discard;
} GChannel;

#define G_TYPE_CHANNEL_CLOSED 99
#define g_value_is_channel_closed(val) g_value_is(val, G_TYPE_CHANNEL_CLOSED)

GChannel *g_channel_new( guint max);
gbool g_channel_write(GChannel *self, GValue* item);
gbool g_channel_read(GChannel *self, GValue* item);
gbool g_channel_is_closed(GChannel *self);
void g_channel_close(GChannel *self);
void g_channel_error(GChannel *self, gint err_code, gstr err_msg);
void g_channel_discard(GChannel *self);
guint g_channel_readable_count(GChannel *self);
guint g_channel_writable_count(GChannel *self);
void g_channel_free(GChannel *self);

#endif