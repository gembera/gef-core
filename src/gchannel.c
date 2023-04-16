/*
 * Copyright (c) 2023 Gembera Team
 * https://github.com/gembera
 * License: MIT
 */

#include "gchannel.h"

GChannel *g_channel_new_with(guint item_len, guint max,
                             GFreeCallback item_cleanup_callback) {
  g_return_val_if_fail(max > 0, NULL);
  GChannel *self = g_new(GChannel);
  g_return_val_if_fail(self, NULL);
  self->items = g_array_new_with(item_len);
  g_array_set_capacity(self->items, max);
  self->max = max;
  self->item_cleanup_callback = item_cleanup_callback;
  return self;
}
gbool g_channel_send(GChannel *self, gpointer item) {
  g_return_val_if_fail(self, FALSE);
  guint size = g_array_size(self->items);
  if (self->max > size) {
    g_array_add_ref(self->items, item);
    return TRUE;
  }
  return FALSE;
}
gbool g_channel_receive(GChannel *self, gpointer item) {
  g_return_val_if_fail(self, FALSE);
  guint size = g_array_size(self->items);
  if (size > 0) {
    g_array_copy(self->items, item, 0, 1);
    g_array_remove(self->items, 0);
    return TRUE;
  }
  return FALSE;
}
void g_channel_free(GChannel *self) {
  g_return_if_fail(self);
  if (self->item_cleanup_callback) {
    guint size = g_array_size(self->items);
    for (guint i = 0; i < size; i++) {
      self->item_cleanup_callback(g_array_get_ref(self->items, i));
    }
  }
  g_array_free(self->items);
  g_free(self);
}