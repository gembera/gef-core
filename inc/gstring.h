#ifndef __G_STRING__
#define __G_STRING__
#include "glib.h"

typedef struct {
  gstr value;
  guint alloc;
  guint len;
  guint max_len;
} GString;

GString *g_string_new();
GString *g_string_new_with(guint max_len);
void g_string_free(GString *self);
GString *g_string_wrap(gstr value, guint alloc);
gstr g_string_unwrap(GString *self);
void g_string_reset(GString *self);
#define g_string_append(self, str) g_string_append_with(self, str, g_len(str))
gint g_string_append_with(GString *self, gcstr str, guint len);
gint g_string_printf(GString *self, gcstr fmt, ...);
gint g_string_vprintf(GString *self, gcstr fmt, va_list va);

#endif