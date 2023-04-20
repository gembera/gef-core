/*
 * Copyright (c) 2023 Gembera Team
 * https://github.com/gembera
 * License: MIT
 */

#include "gstream.h"

gint32 g_stream_read_int32(gpointer self, gint32 *data) {
  return g_stream_read(self, (void *)data, 4);
}
gfloat g_stream_read_float(gpointer self) {
  gfloat data;
  g_stream_read(self, (void *)&data, 4);
  return data;
}
gint16 g_stream_read_int16(gpointer self) {
  gint16 data;
  g_stream_read(self, (void *)&data, 2);
  return data;
}
gstr g_stream_read_string_with_length(gpointer self) {
  gint16 fnlen = 0;
  gstr fn = NULL;
  fnlen = g_stream_read_int16(self);
  if (fnlen < 0)
    return fn;
  fn = g_malloc(fnlen + 1);
  fn[fnlen] = '\0';
  g_stream_read(self, fn, fnlen);
  return fn;
}

void g_stream_write_int32(gpointer self, gint32 data) {
  g_stream_write(self, (void *)&data, 4);
}
void g_stream_write_float(gpointer self, gfloat data) {
  g_stream_write(self, (void *)&data, 4);
}
void g_stream_write_int16(gpointer self, gint16 data) {
  g_stream_write(self, (void *)&data, 2);
}
void g_stream_write_string_with_length(gpointer self, gstr text) {
  gint16 len = g_len(text);
  g_stream_write_int16(self, len);
  g_stream_write(self, text, len);
}
void g_stream_read_all_content(gpointer self, gstr *retbuffer,
                               glong *retbuflen, gbool appendzero) {
  gstr buffer = NULL;
  glong buflen = 0;
  buflen = g_stream_get_length(self);
  buffer = (guint8 *)g_malloc(buflen + (appendzero ? 1 : 0));
  g_stream_seek(self, 0, SEEK_BEGIN);
  g_stream_read(self, buffer, buflen);
  if (appendzero)
    ((gstr)buffer)[buflen] = '\0';
  *retbuffer = buffer;
  *retbuflen = buflen;
}

void g_stream_write_byte(gpointer self, gint bi) {
  guint8 b = (guint8)(bi & 0xFF);
  g_stream_write(self, &b, 1);
}
gint g_stream_read_byte(gpointer self) {
  guint8 b = 0;
  g_stream_read(self, &b, 1);
  return b;
}

void g_stream_init(gpointer self) { g_object_init(self); }
void g_stream_finalize(gpointer self) { g_object_finalize(self); }
void g_stream_class_init(gpointer c) { g_object_class_init(c); }

gpointer g_class_GStream() {
  gpointer clazz = g_class(CLASS_NAME(GStream));
  return clazz != NULL
             ? clazz
             : REGISTER_CLASS(GStream, GObject, g_stream_init,
                              g_stream_finalize, g_stream_class_init, NULL);
}