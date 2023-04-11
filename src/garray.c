/*
 * Copyright (c) 2023 Gembera Team
 * https://github.com/gembera
 * License: MIT
 */

#include "glib.h"

typedef struct _GRealArray {
  guint8 *data;
  guint len;
  guint alloc;
  guint item_size;
} GRealArray;

static void g_array_maybe_expand(GRealArray *array, gint len);

GArray *g_array_new(gint item_size) {
  GRealArray *array;
  g_return_val_if_fail(item_size > 0, NULL);

  array = g_new(GRealArray, 1);
  g_return_val_if_fail(array != NULL, NULL);

  array->data = NULL;
  array->len = 0;
  array->alloc = 0;
  array->item_size = item_size;

  return (GArray *)array;
}

void g_array_free(GArray *self) {
  g_free(self->data);
  g_free(self);
}

void g_array_append_items(GArray *self, gpointer data, gint count) {
  g_return_if_fail(self != NULL);
  g_return_if_fail(count > 0);
  GRealArray *array = (GRealArray *)self;
  gint size = array->item_size;
  g_array_maybe_expand(array, size * count);
  memcpy(array->data + array->len, data, size * count);
  array->len += size * count;
}

void g_array_prepend_items(GArray *self, gpointer data, gint count) {
  g_return_if_fail(self != NULL);
  g_return_if_fail(count > 0);
  GRealArray *array = (GRealArray *)self;
  gint size = array->item_size;
  g_array_maybe_expand(array, size * count);
  g_memmove(array->data + size * count, array->data, array->len);
  memcpy(array->data, data, size * count);
  array->len += size * count;
}

void g_array_set_length(GArray *self, gint length) {
  g_return_if_fail(self != NULL);
  GRealArray *array = (GRealArray *)self;
  gint size = array->item_size;
  g_array_maybe_expand(array, length * size - array->len);
  array->len = length * size;
}

void g_array_remove(GArray *self, gint index) {
  g_return_if_fail(self != NULL);
  gint length = g_array_length(self);
  g_return_if_fail(index >= 0 && index < length);
  GRealArray *array = (GRealArray *)self;
  gint size = array->item_size;
  if (array->data && index < array->len / size - 1)
    g_memmove(array->data + index * size, array->data + (index + 1) * size,
              array->len - (1 + index) * size);
  array->len -= size;
}

void g_array_insert_ref(GArray *self, gint index, gpointer data) {
  g_return_if_fail(self != NULL);
  gint length = g_array_length(self);
  g_return_if_fail(index >= 0 && index <= length);
  GRealArray *array = (GRealArray *)self;
  gint size = array->item_size;
  g_array_maybe_expand(array, size);
  array->len += size;

  if (array->data && index < array->len / size - 1)
    g_memmove(array->data + (index + 1) * size, array->data + index * size,
              array->len - (1 + index) * size);

  memcpy(array->data + index * size, data, size);
}
gint g_array_length(GArray *self) {
  g_return_val_if_fail(self != NULL, 0);
  GRealArray *array = (GRealArray *)self;
  return array->len / array->item_size;
}
void g_array_set_capacity(GArray *self, gint capacity) {
  g_return_if_fail(self != NULL);
  GRealArray *array = (GRealArray *)self;
  gint size = array->item_size;
  if (capacity * size > array->len)
    g_array_maybe_expand(array, capacity * size - array->len);
}
static void g_array_maybe_expand(GRealArray *array, gint len) {
  guint old_alloc;
  if ((array->len + len) > array->alloc) {
    old_alloc = array->alloc;
    array->alloc = array->len + len;
    array->data = g_realloc(array->data, array->alloc);
    memset(array->data + old_alloc, 0, array->alloc - old_alloc);
  }
}

typedef struct _GRealPtrArray {
  gpointer *data;
  gint len;
  gint alloc;
} GRealPtrArray;

static void g_ptr_array_maybe_expand(GRealPtrArray *array, gint len);

GPtrArray *g_ptr_array_new() {
  GRealPtrArray *array;
  array = g_new0(GRealPtrArray, 1);
  return (GPtrArray *)array;
}
void g_ptr_array_free(GPtrArray *self) {
  g_return_if_fail(self);
  g_free(self->data);
  g_free(self);
}

static void g_ptr_array_maybe_expand(GRealPtrArray *array, gint len) {
  guint old_alloc;
  if ((array->len + len) > array->alloc) {
    old_alloc = array->alloc;
    array->alloc = array->len + len;
    if (array->data)
      array->data = g_realloc(array->data, sizeof(gpointer) * array->alloc);
    else
      array->data = g_new0(gpointer, array->alloc);
    memset(array->data + old_alloc, 0, array->alloc - old_alloc);
  }
}

void g_ptr_array_set_capacity(GPtrArray *self, gint length) {
  GRealPtrArray *array = (GRealPtrArray *)self;
  g_return_if_fail(array);
  if (length > array->len)
    g_ptr_array_maybe_expand(array, (length - array->len));
}
void g_ptr_array_set_length(GPtrArray *self, gint length) {
  GRealPtrArray *array = (GRealPtrArray *)self;
  g_ptr_array_set_capacity(self, length);
  array->len = length;
}

void g_ptr_array_remove(GPtrArray *self, gint index) {
  int i;
  GRealPtrArray *array = (GRealPtrArray *)self;

  g_return_if_fail(array);
  g_return_if_fail(index >= 0 && index < array->len);

  for (i = index; i < array->len - 1; i++)
    array->data[i] = array->data[i + 1];

  array->data[array->len - 1] = NULL;

  array->len -= 1;
}
void g_ptr_array_insert(GPtrArray *self, gint index, gpointer item) {
  int i;
  GRealPtrArray *array = (GRealPtrArray *)self;

  g_return_if_fail(array);
  g_return_if_fail(index >= 0 && index <= array->len);

  g_ptr_array_maybe_expand(array, 1);

  if (index == array->len) {
    array->data[array->len++] = item;
  } else {
    array->len++;

    for (i = array->len - 1; i > index; i--)
      array->data[i] = array->data[i - 1];

    array->data[index] = item;
  }
}
gint g_ptr_array_search(GPtrArray *self, GSearchHandler func, gpointer item) {
  gint i;
  for (i = 0; i < g_ptr_array_length(self); i++) {
    if (func(g_ptr_array_get(self, i), item))
      return i;
  }
  return -1;
}
gint g_ptr_array_index_of(GPtrArray *self, gpointer item) {
  gint i;
  for (i = 0; i < g_ptr_array_length(self); i++) {
    if (item == g_ptr_array_get(self, i))
      return i;
  }
  return -1;
}

void g_ptr_array_append_items(GPtrArray *self, gpointer *data, gint count) {
  g_return_if_fail(self != NULL);
  g_return_if_fail(count > 0);
  GRealPtrArray *array = (GRealPtrArray *)self;
  gint size = sizeof(gpointer);
  g_ptr_array_maybe_expand(array, count);
  memcpy(array->data + array->len, data, size * count);
  array->len += count;
}

void g_ptr_array_prepend_items(GPtrArray *self, gpointer *data, gint count) {
  g_return_if_fail(self != NULL);
  g_return_if_fail(count > 0);
  GRealPtrArray *array = (GRealPtrArray *)self;
  gint size = sizeof(gpointer);
  g_ptr_array_maybe_expand(array, count);
  g_memmove((guint8 *)(array->data + count), (guint8 *)array->data,
            size * array->len);
  memcpy(array->data, data, size * count);
  array->len += count;
}