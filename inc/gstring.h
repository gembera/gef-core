#ifndef __G_STRING__
#define __G_STRING__
#include "glib.h"

typedef struct {
  gstr value;
  guint len;
  guint alloc;
  gbool fixed;
} GString;

GString *g_string_new();
GString *g_string_new_with(guint max_length);
void g_string_free(GString *self);
void g_string_reset(GString *self);
gint g_string_append(GString *self, gcstr fmt, ...);
gint g_string_append_str(GString *self, gcstr str);
gint g_string_append_int(GString *self, long num);
gint g_string_append_float(GString *self, double num, gint digits);

#endif