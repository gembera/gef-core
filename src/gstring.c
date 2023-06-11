#include "gstring.h"
#include "printf.h"

#define STRING_CHUNK_SIZE 16
static inline void _string_putc(char ch, void *buffer, size_t idx,
                                size_t maxlen) {
  GString *self = (GString *)buffer;
  if (ch == '\0') {
    self->value[self->len] = '\0';
    return;
  }
  gint left = self->alloc - 1 - self->len;
  if (self->max_len) {
    if (left <= 0)
      return;
  } else {
    if (left < 1) {
      gint alloc = self->alloc + STRING_CHUNK_SIZE;
      gstr value = (gstr)g_realloc(self->value, alloc);
      g_return_if_fail(value);
      self->alloc = alloc;
      self->value = value;
    }
  }
  self->value[self->len++] = ch;
}

GString *g_string_new() {
  GString *self = g_new(GString);
  g_return_val_if_fail(self, NULL);
  return self;
}
GString *g_string_new_with(guint max_len) {
  GString *self = g_new(GString);
  g_return_val_if_fail(self && max_len > 0, NULL);
  self->alloc = max_len + 1;
  self->value = g_malloc(self->alloc);
  self->value[0] = '\0';
  self->max_len = max_len;
  return self;
}
GString *g_string_wrap(gstr value, guint alloc) {
  GString *self = g_new(GString);
  g_return_val_if_fail(self && alloc > 0, NULL);
  self->alloc = alloc;
  self->value = value;
  self->value[0] = '\0';
  self->max_len = alloc - 1;
  return self;
}
void g_string_free(GString *self) {
  g_return_if_fail(self);
  if (self->value)
    g_free(self->value);
  g_free(self);
}
gstr g_string_unwrap(GString *self) {
  g_return_val_if_fail(self, NULL);
  gstr value = self->value;
  self->value = NULL;
  g_string_free(self);
  return value;
}
void g_string_reset(GString *self) {
  g_return_if_fail(self);
  if (self->value && self->alloc > 0) {
    self->len = 0;
    self->value[0] = '\0';
  }
}
gint g_string_printf(GString *self, gcstr fmt, ...) {
  g_return_val_if_fail(self, 0);
  va_list va;
  va_start(va, fmt);
  const int ret =
      _vsnprintf(_string_putc, (char *)self,
                 (size_t)(self->max_len ? self->max_len : -1), fmt, va);
  va_end(va);
  return ret;
}
gint g_string_vprintf(GString *self, gcstr fmt, va_list va) {
  g_return_val_if_fail(self, 0);
  return _vsnprintf(_string_putc, (char *)self,
                    (size_t)(self->max_len ? self->max_len : -1), fmt, va);
}
gint g_string_append_with(GString *self, gcstr str, guint len) {
  g_return_val_if_fail(self && str, 0);
  gint i;
  gint clen = self->len;
  for (i = 0; i < len; i++)
    _string_putc(str[i], self, 0, 0);
  _string_putc('\0', self, 0, 0);
  return self->len - clen;
}