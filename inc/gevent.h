/*
 * Copyright (c) 2023 Gembera Team
 * https://github.com/gembera
 * License: MIT
 */

#ifndef _G_EVENT_H_
#define _G_EVENT_H_
#include "glib.h"

typedef struct {
  gpointer sender;
  gint last_token;
  gint firing_index;
  GArray *listeners;
} GEvent;

typedef void (*GEventCallback)(GEvent *event, gpointer args,
                               gpointer user_data);

#define g_event_new() g_event_new_with(NULL)
GEvent *g_event_new_with(gpointer sender);
void g_event_free(GEvent *self);

#define g_event_add_listener(self, callback, user_data)                        \
  g_event_add_listener_with(self, callback, user_data, NULL)
gint g_event_add_listener_with(GEvent *self, GEventCallback callback,
                               gpointer user_data, GFreeCallback free_callback);
void g_event_remove_listener(GEvent *self, gint token);
void g_event_fire(GEvent *self, gpointer args);

#endif