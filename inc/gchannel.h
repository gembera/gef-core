/*
 * Copyright (c) 2023 Gembera Team
 * https://github.com/gembera
 * License: MIT
 */

#ifndef _G_CHANNEL_H_
#define _G_CHANNEL_H_
#include "glib.h"
#include "gevent.h"

#define CH_STATUS_ERROR
typedef struct {
  GArray *items;
  guint max;
  GFreeCallback item_cleanup_callback;
  GEvent * on_read;
  GEvent * on_write;
  GEvent * on_closed;
  GEvent * on_error;
} GChannel;

#define g_channel_new(type, max) g_channel_new_with(sizeof(type), max, NULL)
GChannel *g_channel_new_with(guint item_len, guint max,
                             GFreeCallback item_cleanup_callback);
gbool g_channel_write(GChannel *self, gpointer item);
gbool g_channel_read(GChannel *self, gpointer item);
guint g_channel_readable_size();
guint g_channel_writable_size();
void g_channel_free(GChannel *self);

#endif