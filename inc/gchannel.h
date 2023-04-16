/*
 * Copyright (c) 2023 Gembera Team
 * https://github.com/gembera
 * License: MIT
 */

#ifndef _G_CHANNEL_H_
#define _G_CHANNEL_H_
#include "glib.h"

typedef struct {
  GPtrArray *buffer;
  guint max;
} GChannel;

#define g_channel_new(type) g_channel_new_with(sizeof(type), 1)
GChannel *g_channel_new_with(guint item_size, guint max, GFreeCallback);
void g_channel_free(GChannel *self);

#endif