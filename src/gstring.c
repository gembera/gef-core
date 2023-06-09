#include "gstring.h"
#include "mjson.h"
#define STRING_RESERVE_SIZE 16
static int g_string_append_buffer(const char *ptr, int len, void *fn_data) {
  GString *self = (GString *)fn_data;
  gint i;
  gint left = self->alloc - 1 - self->len;
  if (self->fixed) {
    if (left < len)
      len = left;
  } else {
    if (left < len) {
      gint new_size = self->alloc + len + STRING_RESERVE_SIZE;
      gstr s = (gstr)g_realloc(self->value, new_size);
      g_return_val_if_fail(s, 0);
      self->alloc = new_size;
      self->value = s;
    }
  }
  memcpy(self->value + self->len, ptr, (size_t)len);
  self->len += len;
  self->value[self->len] = '\0';
  return len;
}

GString *g_string_new() {
  GString *self = g_new(GString);
  g_return_val_if_fail(self, NULL);
  self->value = NULL;
  self->alloc = 0;
  self->len = 0;
  self->fixed = FALSE;
  return self;
}
GString *g_string_new_with(guint max_length) {
  GString *self = g_new(GString);
  g_return_val_if_fail(self && max_length > 0, NULL);
  self->alloc = max_length + 1;
  self->value = g_malloc(self->alloc);
  self->len = 0;
  self->fixed = TRUE;
  return self;
}
void g_string_free(GString *self) {
  g_return_if_fail(self);
  g_free(self->value);
  g_free(self);
}
void g_string_reset(GString *self) {
  g_return_if_fail(self);
  if (self->value && self->alloc > 0) {
    self->len = 0;
    self->value[0] = '\0';
  }
}
gint g_string_append(GString *self, gcstr fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  gint len = mjson_vprintf(g_string_append_buffer, self, fmt, &ap);
  va_end(ap);
  return len;
}
gint g_string_append_str(GString *self, gcstr str) {
  return g_string_append_buffer(str, g_len(str), self);
}
gint g_string_append_int(GString *self, long num) {
  return mjson_print_long(g_string_append_buffer, self, num, TRUE);
}
gint g_string_append_float(GString *self, double num, gint digits) {
  return mjson_print_dbl(g_string_append_buffer, self, num, digits);
}