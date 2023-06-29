/*
 * Copyright (c) 2023 Gembera Team
 * https://github.com/gembera
 * License: MIT
 */

#include "gchannel.h"

GChannel *g_channel_new(guint max) {
  GChannel *self = g_new(GChannel);
  g_return_val_if_fail(self, NULL);
  self->items = g_array_new(GValue);
  g_array_set_capacity(self->items, max);
  self->max = max;
  self->closed = FALSE;
  self->on_read = g_event_new_with(self);
  self->on_write = g_event_new_with(self);
  return self;
}
gbool g_channel_write(GChannel *self, GValue *item) {
  g_return_val_if_fail(self, FALSE);
  g_return_val_if_fail(!self->closed, FALSE);
  gbool write_anyway =
      g_value_is_error(item) || g_value_is_channel_closed(item);
  guint size = g_array_size(self->items);
  if (self->max == 0 || self->max > size || write_anyway) {
    g_array_add(self->items, item);
    g_value_ref(item);
    if (g_value_is_channel_closed(item)) {
      self->closed = TRUE;
    }
    g_event_fire(self->on_write, item);
    return TRUE;
  }
  return FALSE;
}
gbool g_channel_read(GChannel *self, GValue *item) {
  g_return_val_if_fail(self, FALSE);
  guint size = g_array_size(self->items);
  if (size > 0) {
    g_value_unref(item);
    g_array_copy(self->items, item, 0, 1);
    g_array_remove(self->items, 0);
    g_event_fire(self->on_read, item);
    return TRUE;
  }
  return FALSE;
}
void g_channel_free(GChannel *self) {
  g_return_if_fail(self);
  guint size = g_array_size(self->items);
  for (guint i = 0; i < size; i++) {
    g_value_unref(g_array_get_ref(self->items, i));
  }
  g_event_free(self->on_read);
  g_event_free(self->on_write);
  g_array_free(self->items);
  g_free(self);
}
gbool g_channel_is_closed(GChannel *self) {
  g_return_val_if_fail(self, FALSE);
  return self->closed;
}
void g_channel_close(GChannel *self) {
  g_return_if_fail(!self->closed);
  GValue *val_closed =
      g_value_set(g_new(GValue), G_TYPE_CHANNEL_CLOSED, NULL, NULL);
  g_channel_write(self, val_closed);
  g_value_free(val_closed);
}
void g_channel_error(GChannel *self, gint err_code, gstr err_msg) {
  g_return_if_fail(!self->closed && err_code < 0);
  GValue *val_error =
      g_value_set(g_new(GValue), err_code, err_msg ? g_dup(err_msg) : NULL,
                  g_free_callback);
  g_channel_write(self, val_error);
  g_value_free(val_error);
}
guint g_channel_readable_count(GChannel *self) {
  g_return_val_if_fail(self, 0);
  return g_array_size(self->items);
}
guint g_channel_writable_count(GChannel *self) {
  g_return_val_if_fail(self, 0);
  guint size = g_array_size(self->items);
  if (self->max == 0)
    return MAXINT - size;
  if (self->max > size)
    return self->max - size;
  return 0;
}