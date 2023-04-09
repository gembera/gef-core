/*
 * Copyright (c) 2023 Gembera Team
 * https://github.com/gembera
 * License: MIT
 */

#include "glib.h"

typedef struct _GRealArray GRealArray;

struct _GRealArray {
  guint8 *data;
  guint len;
  guint alloc;
};

static void g_array_maybe_expand(GRealArray *array, gint len);

GArray *g_array_new(void) {
  GRealArray *array;

  array = g_new(GRealArray, 1);

  array->data = NULL;
  array->len = 0;
  array->alloc = 0;

  return (GArray *)array;
}

void g_array_free(GArray *array) {
  // if (free_segment)
  g_free(array->data);

  g_free(array);
}

GArray *g_rarray_append(GArray *array, gpointer data, gint size) {
  g_array_maybe_expand((GRealArray *)array, size);

  memcpy(array->data + array->len, data, size);

  array->len += size;

  return array;
}

GArray *g_rarray_prepend(GArray *array, gpointer data, gint size) {
  g_array_maybe_expand((GRealArray *)array, size);

  g_memmove(array->data + size, array->data, array->len);
  memcpy(array->data, data, size);

  array->len += size;

  return array;
}

GArray *g_rarray_set_length(GArray *array, gint length, gint size) {
  g_array_maybe_expand((GRealArray *)array, length * size - array->len);
  if (array->data)
    memset(array->data + length * size, 0, size);
  array->len = length * size;
  return array;
}

GArray *g_rarray_remove_index(GArray *array, gint size, gint index) {
  if (array->data && index < array->len / size - 1)
    g_memmove(array->data + index * size, array->data + (index + 1) * size,
              array->len - (1 + index) * size);
  array->len -= size;
  return array;
}

GArray *g_rarray_insert_val(GArray *array, gint index, gpointer data,
                            gint size) {
  g_array_maybe_expand((GRealArray *)array, size);
  array->len += size;

  if (array->data && index < array->len / size - 1)
    g_memmove(array->data + (index + 1) * size, array->data + index * size,
              array->len - (1 + index) * size);

  memcpy(array->data + index * size, data, size);
  return array;
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

/* Pointer Array
 */

typedef struct _GRealPtrArray GRealPtrArray;

struct _GRealPtrArray {
  gpointer *pdata;
  gint len;
  gint alloc;
  GCompareFunc comp;
};

static void g_ptr_array_maybe_expand(GRealPtrArray *array, gint len);

GPtrArray *g_ptr_array_new() { return g_ptr_array_new_sorted(NULL); }
GPtrArray *g_ptr_array_new_sorted(GCompareFunc func) {
  GRealPtrArray *array;

  array = g_new0(GRealPtrArray, 1);
  array->comp = func;

  return (GPtrArray *)array;
}
void g_ptr_array_free(GPtrArray *array) {
  g_return_if_fail(array);

  // if (free_segment)
  g_free(array->pdata);

  g_free(array);
}

static void g_ptr_array_maybe_expand(GRealPtrArray *array, gint len) {
  guint old_alloc;

  if ((array->len + len) > array->alloc) {
    old_alloc = array->alloc;

    // array->alloc = g_nearest_pow (array->len + len);
    // array->alloc = MAX (array->alloc, MIN_ARRAY_SIZE);
    array->alloc = array->len + len;
    if (array->pdata)
      array->pdata = g_realloc(array->pdata, sizeof(gpointer) * array->alloc);
    else
      array->pdata = g_new0(gpointer, array->alloc);

    memset(array->pdata + old_alloc, 0, array->alloc - old_alloc);
  }
}

void g_ptr_array_set_capacity(GPtrArray *farray, gint length) {

  GRealPtrArray *array = (GRealPtrArray *)farray;

  g_return_if_fail(array);

  if (length > array->len)
    g_ptr_array_maybe_expand(array, (length - array->len));
}
void g_ptr_array_set_size(GPtrArray *farray, gint length) {
  GRealPtrArray *array = (GRealPtrArray *)farray;
  g_ptr_array_set_capacity(farray, length);
  array->len = length;
}

void g_ptr_array_remove_index(GPtrArray *farray, gint index) {
  int i;
  GRealPtrArray *array = (GRealPtrArray *)farray;

  g_return_if_fail(array);
  g_return_if_fail(index >= 0 && index < array->len);

  for (i = index; i < array->len - 1; i++)
    array->pdata[i] = array->pdata[i + 1];

  array->pdata[array->len - 1] = NULL;

  array->len -= 1;
}
void g_ptr_array_insert(GPtrArray *farray, gint index, gpointer item) {
  int i;
  GRealPtrArray *array = (GRealPtrArray *)farray;

  g_return_if_fail(array);
  g_return_if_fail(index >= 0 && index <= array->len);

  g_ptr_array_maybe_expand(array, 1);

  if (index == array->len) {
    array->pdata[array->len++] = item;
  } else {
    array->len++;

    for (i = array->len - 1; i > index; i--)
      array->pdata[i] = array->pdata[i - 1];

    array->pdata[index] = item;
  }
}
void g_ptr_array_add_all(GPtrArray *array, GPtrArray *artoadd) {
  gint i, len;
  g_return_if_fail(array != NULL && artoadd != NULL);
  len = g_ptr_array_length(artoadd);
  for (i = 0; i < len; i++) {
    g_ptr_array_add(array, g_ptr_array_index(artoadd, i));
  }
}

gint g_ptr_array_lookup(GPtrArray *array, void *item) {
  GCompareFunc comp = ((GRealPtrArray *)array)->comp;
  return g_ptr_array_lookup_ex(array, item, comp);
}
gint g_ptr_array_lookup_ex(GPtrArray *ar, void *item, GCompareFunc func) {
  GCompareFunc comp = func != NULL ? func : ((GRealPtrArray *)ar)->comp;
  if (comp != NULL) {
    gint size, l, r, m, c;
    gpointer data;
    size = g_ptr_array_length(ar);

    l = 0;
    r = size - 1;
    while (l <= r) {
      m = (l + r) / 2;
      data = g_ptr_array_index(ar, m);
      c = comp(item, data);
      if (c == 0)
        return m;
      else if (c < 0) {
        r = m - 1;
      } else {
        l = m + 1;
      }
    }
    return -1;
  }
  return -1;
}
gint g_ptr_array_index_of(GPtrArray *ar, void *item) {
  gint i;
  for (i = 0; i < g_ptr_array_length(ar); i++) {
    if (item == g_ptr_array_index(ar, i))
      return i;
  }
  return -1;
}
gboolean g_ptr_array_remove(GPtrArray *farray, gpointer data) {
  GRealPtrArray *array = (GRealPtrArray *)farray;
  int i;

  g_return_val_if_fail(array, FALSE);

  for (i = 0; i < array->len; i += 1) {
    if (array->pdata[i] == data) {
      g_ptr_array_remove_index(farray, i);
      return TRUE;
    }
  }

  return FALSE;
}

gint g_ptr_array_add(GPtrArray *farray, gpointer item) {
  GRealPtrArray *array = (GRealPtrArray *)farray;

  GCompareFunc comp = ((GRealPtrArray *)farray)->comp;

  g_return_val_if_fail(array, -1);

  g_ptr_array_maybe_expand(array, 1);

  if (comp == NULL) {
    array->pdata[array->len++] = item;
    return array->len - 1;
  } else {

    gint size, l, r, m = -1, c;
    gpointer data;
    size = g_ptr_array_length(farray);

    if (size > 0) {
      m = size - 1;
      data = g_ptr_array_index(farray, m);
      c = comp(item, data);
      if (c < 0) {
        m = 0;
        data = g_ptr_array_index(farray, m);
        c = comp(item, data);
        if (c > 0) {
          l = 0;
          r = size - 1;
          while (l <= r) {
            m = (l + r) / 2;
            data = g_ptr_array_index(farray, m);
            c = comp(item, data);
            if (c == 0) {
              break;
            } else if (c < 0) {
              r = m - 1;
            } else {
              l = m + 1;
            }
          }
        }
      }
    }

    if (size == 0) {
      array->pdata[array->len++] = item;
      return array->len - 1;
    } else {
      gint index = c < 0 ? m : m + 1;
      g_ptr_array_insert(farray, index, item);
      return index;
    }
  }
}

/* Byte arrays
 */

GByteArray *g_byte_array_new(void) { return (GByteArray *)g_array_new(); }

void g_byte_array_free(GByteArray *array) { g_array_free((GArray *)array); }

GByteArray *g_byte_array_append(GByteArray *array, const guint8 *data,
                                guint len) {
  g_rarray_append((GArray *)array, (guint8 *)data, len);

  return array;
}

GByteArray *g_byte_array_prepend(GByteArray *array, const guint8 *data,
                                 guint len) {
  g_rarray_prepend((GArray *)array, (guint8 *)data, len);

  return array;
}

GByteArray *g_byte_array_set_length(GByteArray *array, gint length) {
  g_rarray_set_length((GArray *)array, length, 1);

  return array;
}
