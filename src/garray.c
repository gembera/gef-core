/*
 * Copyright (c) 2023 Gembera Team
 * https://github.com/gembera
 * License: MIT
 */

#include "glib.h"

static gbool g_array_maybe_expand(GArray *self, guint len) {
  if ((self->len + len) > self->alloc) {
    gpointer newdata = g_realloc(self->data, self->len + len);
    g_return_val_if_fail(newdata, FALSE);
    guint old_alloc = self->alloc;
    self->alloc = self->len + len;
    self->data = newdata;
    memset(self->data + old_alloc, 0, self->alloc - old_alloc);
  }
  return TRUE;
}

GArray *g_array_new_with(guint item_len) {
  g_return_val_if_fail(item_len > 0, NULL);
  GArray *self = g_new(GArray);
  g_return_val_if_fail(self, NULL);
  self->data = NULL;
  self->len = 0;
  self->alloc = 0;
  self->item_len = item_len;
  return self;
}

void g_array_free(GArray *self) {
  g_return_if_fail(self);
  g_free(self->data);
  g_free(self);
}

gbool g_array_append_items(GArray *self, gpointer data, guint count) {
  g_return_val_if_fail(self, FALSE);
  g_return_val_if_fail(count > 0, FALSE);
  guint size = self->item_len;
  g_return_val_if_fail(g_array_maybe_expand(self, size * count), FALSE);
  memcpy(self->data + self->len, data, size * count);
  self->len += size * count;
  return TRUE;
}

gbool g_array_prepend_items(GArray *self, gpointer data, guint count) {
  g_return_val_if_fail(self, FALSE);
  g_return_val_if_fail(count > 0, FALSE);
  guint size = self->item_len;
  g_return_val_if_fail(g_array_maybe_expand(self, size * count), FALSE);
  g_memmove(self->data + size * count, self->data, self->len);
  memcpy(self->data, data, size * count);
  self->len += size * count;
  return TRUE;
}

gbool g_array_set_size(GArray *self, guint length) {
  g_return_val_if_fail(self, FALSE);
  guint size = self->item_len;
  g_return_val_if_fail(g_array_maybe_expand(self, length * size - self->len),
                       FALSE);
  self->len = length * size;
  return TRUE;
}

void g_array_remove(GArray *self, guint index) {
  g_return_if_fail(self);
  guint size = g_array_size(self);
  g_return_if_fail(index >= 0 && index < size);
  guint item_len = self->item_len;
  if (self->data && index < self->len / item_len - 1)
    g_memmove(self->data + index * item_len,
              self->data + (index + 1) * item_len,
              self->len - (1 + index) * item_len);
  self->len -= item_len;
}
void g_array_copy(GArray *self, gpointer data, guint index, guint count) {
  g_return_if_fail(self);
  guint size = g_array_size(self);
  g_return_if_fail(index >= 0 && index < size && count > 0 &&
                   index + count <= size);
  guint item_len = self->item_len;
  memcpy(data, self->data + index * item_len, item_len * count);
}
gpointer g_array_get_ref(GArray *self, guint index) {
  g_return_val_if_fail(self, NULL);
  guint size = g_array_size(self);
  g_return_val_if_fail(index >= 0 && index < size, NULL);
  guint item_len = self->item_len;
  return self->data + index * item_len;
}
void g_array_set(GArray *self, guint index, gpointer ref) {
  g_return_if_fail(self);
  guint size = g_array_size(self);
  g_return_if_fail(index >= 0 && index < size && ref);
  guint item_len = self->item_len;
  memcpy(self->data + index * item_len, ref, item_len);
}
gbool g_array_insert(GArray *self, guint index, gpointer ref) {
  g_return_val_if_fail(self, FALSE);
  guint size = g_array_size(self);
  g_return_val_if_fail(index >= 0 && index <= size, FALSE);
  guint item_len = self->item_len;
  g_return_val_if_fail(g_array_maybe_expand(self, item_len), FALSE);
  self->len += item_len;

  if (self->data && index < self->len / item_len - 1)
    g_memmove(self->data + (index + 1) * item_len,
              self->data + index * item_len,
              self->len - (1 + index) * item_len);

  memcpy(self->data + index * item_len, ref, item_len);
  return TRUE;
}
guint g_array_size(GArray *self) {
  g_return_val_if_fail(self, 0);
  return self->len / self->item_len;
}
gbool g_array_set_capacity(GArray *self, guint capacity) {
  g_return_val_if_fail(self, FALSE);
  guint item_len = self->item_len;
  if (capacity * item_len > self->len)
    g_return_val_if_fail(
        g_array_maybe_expand(self, capacity * item_len - self->len), FALSE);
  return TRUE;
}

void g_array_visit(GArray *self, GArrayVisitCallback func, gpointer user_data) {
  g_return_if_fail(self);
  g_return_if_fail(func);
  for (guint i = 0; i < self->len; i += self->item_len) {
    func(self, i, (gpointer)((guint8 *)self->data) + i, user_data);
  }
}

static gbool g_ptr_array_maybe_expand(GPtrArray *self, guint size) {
  if ((self->size + size) > self->alloc) {
    gpointer newdata;
    if (self->data)
      newdata = g_realloc(self->data, sizeof(gpointer) * (self->size + size));
    else
      newdata = g_new_many(gpointer, self->size + size);
    g_return_val_if_fail(newdata, FALSE);
    guint old_alloc = self->alloc;
    self->alloc = self->size + size;
    self->data = newdata;
    memset(self->data + old_alloc, 0, self->alloc - old_alloc);
  }
  return TRUE;
}

GPtrArray *g_ptr_array_new_with(GFreeCallback free_callback) {
  GPtrArray *self = g_new(GPtrArray);
  g_return_val_if_fail(self, NULL);
  self->free_callback = free_callback;
  return self;
}

void g_ptr_array_free(GPtrArray *self) {
  g_return_if_fail(self);
  if (self->free_callback) {
    for (gint i = 0; i < g_ptr_array_size(self); i++) {
      gpointer data = g_ptr_array_get(self, i);
      g_free_with(data, self->free_callback);
    }
  }
  g_free(self->data);
  g_free(self);
}

gbool g_ptr_array_set_capacity(GPtrArray *self, guint capacity) {
  g_return_val_if_fail(self, FALSE);
  if (capacity > self->size)
    g_return_val_if_fail(
        g_ptr_array_maybe_expand(self, (capacity - self->size)), FALSE);
  return TRUE;
}
gbool g_ptr_array_set_size(GPtrArray *self, guint size) {
  g_return_val_if_fail(self, FALSE);
  g_return_val_if_fail(g_ptr_array_set_capacity(self, size), FALSE);
  self->size = size;
  return TRUE;
}

void g_ptr_array_remove(GPtrArray *self, guint index) {
  g_return_if_fail(self);
  g_return_if_fail(index >= 0 && index < self->size);
  for (gint i = index; i < self->size - 1; i++)
    self->data[i] = self->data[i + 1];
  self->data[self->size - 1] = NULL;
  self->size -= 1;
}
gbool g_ptr_array_insert(GPtrArray *self, guint index, gpointer item) {
  g_return_val_if_fail(self, FALSE);
  g_return_val_if_fail(index >= 0 && index <= self->size, FALSE);
  g_return_val_if_fail(g_ptr_array_maybe_expand(self, 1), FALSE);
  if (index == self->size) {
    self->data[self->size++] = item;
  } else {
    self->size++;
    for (gint i = self->size - 1; i > index; i--)
      self->data[i] = self->data[i - 1];
    self->data[index] = item;
  }
  return TRUE;
}

void g_ptr_array_visit(GPtrArray *self, GPtrArrayVisitCallback func,
                       gpointer user_data) {
  g_return_if_fail(self);
  g_return_if_fail(func);
  for (gint i = 0; i < g_ptr_array_size(self); i++) {
    func(self, i, g_ptr_array_get(self, i), user_data);
  }
}

gint g_ptr_array_index_of(GPtrArray *self, gpointer item) {
  g_return_val_if_fail(self, -1);
  for (gint i = 0; i < g_ptr_array_size(self); i++) {
    if (item == g_ptr_array_get(self, i))
      return i;
  }
  return -1;
}

gbool g_ptr_array_append_items(GPtrArray *self, gpointer *data, guint count) {
  g_return_val_if_fail(self, FALSE);
  g_return_val_if_fail(count > 0, FALSE);
  guint size = sizeof(gpointer);
  g_return_val_if_fail(g_ptr_array_maybe_expand(self, count), FALSE);
  memcpy(self->data + self->size, data, size * count);
  self->size += count;
  return TRUE;
}

gbool g_ptr_array_prepend_items(GPtrArray *self, gpointer *data, guint count) {
  g_return_val_if_fail(self, FALSE);
  g_return_val_if_fail(count > 0, FALSE);
  guint size = sizeof(gpointer);
  g_return_val_if_fail(g_ptr_array_maybe_expand(self, count), FALSE);
  g_memmove(self->data + count, self->data, size * self->size);
  memcpy(self->data, data, size * count);
  self->size += count;
  return TRUE;
}