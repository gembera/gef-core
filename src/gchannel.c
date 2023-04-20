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
  self->on_closed = g_event_new_with(self);
  self->on_error = g_event_new_with(self);
  return self;
}
gbool g_channel_write(GChannel *self, GValue *item) {
  g_return_val_if_fail(self, FALSE);
  g_return_val_if_fail(!self->closed, FALSE);
  guint size = g_array_size(self->items);
  if (self->max == 0 || self->max > size || g_value_is_error(item) ||
      g_value_is(item, G_TYPE_CHANNEL_CLOSED)) {
    g_array_add_ref(self->items, item);
    if (item->refs)
      (*item->refs)++;
    g_event_fire(self->on_write, item);
    return TRUE;
  }
  return FALSE;
}
gbool g_channel_read(GChannel *self, GValue *item) {
  g_return_val_if_fail(self, FALSE);
  guint size = g_array_size(self->items);
  if (size > 0) {
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
  g_event_free(self->on_closed);
  g_event_free(self->on_error);
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
  g_event_fire(self->on_closed, val_closed);
  self->closed = TRUE;
  g_value_free(val_closed);
}
void g_channel_error(GChannel *self, gint err_code, gstr err_msg) {
  g_return_if_fail(!self->closed && err_code < 0);
  GValue *val_error =
      g_value_set(g_new(GValue), err_code, err_msg ? g_dup(err_msg) : NULL,
                  g_free_callback);
  g_channel_write(self, val_error);
  g_event_fire(self->on_error, val_error);
  g_value_free(val_error);
}