/*
 * Copyright (c) 2023 Gembera Team
 * https://github.com/gembera
 * License: MIT
 */

#include "gevent.h"
typedef struct {
  GEventCallback callback;
  gint token;
  gpointer user_data;
  GFreeCallback user_data_free_callback;
} GEventListener;

GEvent *g_event_new_with(gpointer sender) {
  GEvent *self = g_new(GEvent);
  g_return_val_if_fail(self, NULL);
  self->sender = sender;
  self->listeners = g_array_new(GEventListener);
  self->firing_index = -1;
  self->last_token = 1;
  return (GEvent *)self;
}
void g_event_free(GEvent *self) {
  g_return_if_fail(self);
  guint size = g_array_size(self->listeners);
  for (gint i = 0; i < size; i++) {
    GEventListener *listener = g_array(self->listeners, GEventListener) + i;
    g_free_with(listener->user_data, listener->user_data_free_callback);
  }
  g_array_free(self->listeners);
  g_free(self);
}

gint g_event_add_listener_with(GEvent *self, GEventCallback callback,
                               gpointer user_data,
                               GFreeCallback free_callback) {
  g_return_val_if_fail(self, -1);
  GEventListener listener;
  self->last_token++;
  listener.callback = callback;
  listener.token = self->last_token;
  listener.user_data = user_data;
  listener.user_data_free_callback = free_callback;
  g_array_add(self->listeners, GEventListener, listener);
  return listener.token;
}
void g_event_remove_listener(GEvent *self, gint token) {
  g_return_if_fail(self);
  gint size = g_array_size(self->listeners);
  gint index = -1;
  for (gint i = 0; i < size; i++) {
    GEventListener *listener = g_array(self->listeners, GEventListener) + i;
    if (listener->token == token) {
      index = i;
      break;
    }
  }
  if (index != -1) {
    if (index <= self->firing_index) {
      self->firing_index--;
    }
    GEventListener *listener = g_array(self->listeners, GEventListener) + index;
    g_free_with(listener->user_data, listener->user_data_free_callback);
    g_array_remove(self->listeners, index);
  }
}
void g_event_fire(GEvent *self, gpointer args) {
  g_return_if_fail(self);
  g_return_if_fail(self->firing_index == -1);
  guint size = g_array_size(self->listeners);
  for (self->firing_index = 0; self->firing_index < size;
       self->firing_index++) {
    GEventListener *listener =
        g_array(self->listeners, GEventListener) + self->firing_index;
    listener->callback(self, args, listener->user_data);
  }
  self->firing_index = -1;
}
