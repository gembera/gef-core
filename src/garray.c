/*
 * Copyright (c) 2023 Gembera Team
 * https://github.com/gembera
 * License: MIT
 */

#include "glib.h"

static void g_array_maybe_expand(GArray *self, guint len) {
  guint old_alloc;
  if ((self->len + len) > self->alloc) {
    old_alloc = self->alloc;
    self->alloc = self->len + len;
    self->data = g_realloc(self->data, self->alloc);
    memset(self->data + old_alloc, 0, self->alloc - old_alloc);
  }
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

void g_array_append_items(GArray *self, gpointer data, guint count) {
  g_return_if_fail(self);
  g_return_if_fail(count > 0);
  guint size = self->item_len;
  g_array_maybe_expand(self, size * count);
  memcpy(self->data + self->len, data, size * count);
  self->len += size * count;
}

void g_array_prepend_items(GArray *self, gpointer data, guint count) {
  g_return_if_fail(self);
  g_return_if_fail(count > 0);
  guint size = self->item_len;
  g_array_maybe_expand(self, size * count);
  g_memmove(self->data + size * count, self->data, self->len);
  memcpy(self->data, data, size * count);
  self->len += size * count;
}

void g_array_set_size(GArray *self, guint length) {
  g_return_if_fail(self);
  guint size = self->item_len;
  g_array_maybe_expand(self, length * size - self->len);
  self->len = length * size;
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
void g_array_set_ref(GArray *self, guint index, gpointer ref) {
  g_return_if_fail(self);
  guint size = g_array_size(self);
  g_return_if_fail(index >= 0 && index < size && ref);
  guint item_len = self->item_len;
  memcpy(self->data + index * item_len, ref, item_len);
}
void g_array_insert_ref(GArray *self, guint index, gpointer ref) {
  g_return_if_fail(self);
  guint size = g_array_size(self);
  g_return_if_fail(index >= 0 && index <= size);
  guint item_len = self->item_len;
  g_array_maybe_expand(self, item_len);
  self->len += item_len;

  if (self->data && index < self->len / item_len - 1)
    g_memmove(self->data + (index + 1) * item_len,
              self->data + index * item_len,
              self->len - (1 + index) * item_len);

  memcpy(self->data + index * item_len, ref, item_len);
}
guint g_array_size(GArray *self) {
  g_return_val_if_fail(self, 0);
  return self->len / self->item_len;
}
void g_array_set_capacity(GArray *self, guint capacity) {
  g_return_if_fail(self);
  guint item_len = self->item_len;
  if (capacity * item_len > self->len)
    g_array_maybe_expand(self, capacity * item_len - self->len);
}

void g_array_visit(GArray *self, GArrayVisitCallback func, gpointer user_data) {
  g_return_if_fail(self);
  g_return_if_fail(func);
  for (guint i = 0; i < self->len; i += self->item_len) {
    func(self, i, (gpointer)((guint8 *)self->data) + i, user_data);
  }
}

static void g_ptr_array_maybe_expand(GPtrArray *self, guint size) {
  guint old_alloc;
  if ((self->size + size) > self->alloc) {
    old_alloc = self->alloc;
    self->alloc = self->size + size;
    if (self->data)
      self->data = g_realloc(self->data, sizeof(gpointer) * self->alloc);
    else
      self->data = g_new_many(gpointer, self->alloc);
    memset(self->data + old_alloc, 0, self->alloc - old_alloc);
  }
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

void g_ptr_array_set_capacity(GPtrArray *self, guint capacity) {
  g_return_if_fail(self);
  if (capacity > self->size)
    g_ptr_array_maybe_expand(self, (capacity - self->size));
}
void g_ptr_array_set_size(GPtrArray *self, guint size) {
  g_return_if_fail(self);
  g_ptr_array_set_capacity(self, size);
  self->size = size;
}

void g_ptr_array_remove(GPtrArray *self, guint index) {
  g_return_if_fail(self);
  g_return_if_fail(index >= 0 && index < self->size);
  for (gint i = index; i < self->size - 1; i++)
    self->data[i] = self->data[i + 1];
  self->data[self->size - 1] = NULL;
  self->size -= 1;
}
void g_ptr_array_insert(GPtrArray *self, guint index, gpointer item) {
  g_return_if_fail(self);
  g_return_if_fail(index >= 0 && index <= self->size);
  g_ptr_array_maybe_expand(self, 1);
  if (index == self->size) {
    self->data[self->size++] = item;
  } else {
    self->size++;
    for (gint i = self->size - 1; i > index; i--)
      self->data[i] = self->data[i - 1];
    self->data[index] = item;
  }
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

void g_ptr_array_append_items(GPtrArray *self, gpointer *data, guint count) {
  g_return_if_fail(self);
  g_return_if_fail(count > 0);
  guint size = sizeof(gpointer);
  g_ptr_array_maybe_expand(self, count);
  memcpy(self->data + self->size, data, size * count);
  self->size += count;
}

void g_ptr_array_prepend_items(GPtrArray *self, gpointer *data, guint count) {
  g_return_if_fail(self);
  g_return_if_fail(count > 0);
  guint size = sizeof(gpointer);
  g_ptr_array_maybe_expand(self, count);
  g_memmove(self->data + count, self->data, size * self->size);
  memcpy(self->data, data, size * count);
  self->size += count;
}