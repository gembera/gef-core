/*
 * Copyright (c) 2023 Gembera Team
 * https://github.com/gembera
 * License: MIT
 */

#ifndef _G_CHANNEL_H_
#define _G_CHANNEL_H_
#include "glib.h"

typedef struct {
  GArray *items;
  guint max;
  GFreeCallback item_cleanup_callback;
} GChannel;

#define g_channel_new(type, max) g_channel_new_with(sizeof(type), max, NULL)
GChannel *g_channel_new_with(guint item_len, guint max,
                             GFreeCallback item_cleanup_callback);
gbool g_channel_send(GChannel *self, gpointer item);
gbool g_channel_receive(GChannel *self, gpointer item);
void g_channel_free(GChannel *self);

#endif