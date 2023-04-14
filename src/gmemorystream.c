/*
 * Copyright (c) 2023 Gembera Team
 * https://github.com/gembera
 * License: MIT
 */

#include "gmemorystream.h"
glong g_memory_stream_get_length(gpointer self) {
  return GMEMORYSTREAM(self)->length;
}
void g_memory_stream_set_capacity(gpointer self, glong capacity) {
  GMemoryStream *s = GMEMORYSTREAM(self);
  if (capacity <= s->length)
    return;
  s->buffer = g_realloc(s->buffer, capacity);
  if (s->buffer == NULL) {
    g_log_error("g_memory_stream_set_capacity : Memory overflow!");
  } else {
    s->capacity = capacity;
  }
}
void g_memory_stream_set_length(gpointer self, glong len) {
  GMemoryStream *s = GMEMORYSTREAM(self);
  if (len > s->capacity)
    g_memory_stream_set_capacity(s, len);
  s->length = len;
}
glong g_memory_stream_get_position(gpointer self) {
  GMemoryStream *s = GMEMORYSTREAM(self);
  return s->position;
}
glong g_memory_stream_write(gpointer self, gcstr buffer, glong length) {
  GMemoryStream *s = GMEMORYSTREAM(self);
  if (length == 0)
    return 0;
  if (s->position + length > s->capacity) {
    gint capacity =
        s->capacity != 0 && s->capacity <= 1024 ? s->capacity * 2 : s->capacity;
    if (s->position + length > capacity)
      capacity = s->position + length;
    g_memory_stream_set_capacity(s, capacity);
  }
  memcpy((gstr)s->buffer + s->position, buffer, length);
  s->position += length;
  if (s->position >= s->length)
    s->length = s->position;
  return length;
}
glong g_memory_stream_read(gpointer self, gstr buffer, glong length) {
  glong available;
  GMemoryStream *s = GMEMORYSTREAM(self);
  if (buffer == NULL)
    return 0;
  available = s->length - s->position;
  if (length > available)
    length = available;
  memcpy(buffer, (gstr)s->buffer + s->position, length);
  s->position += length;
  return length;
}
void g_memory_stream_flush(gpointer self) {}
void g_memory_stream_seek(gpointer self, glong offset, GSeekOrigin origin) {
  GMemoryStream *s = GMEMORYSTREAM(self);
  glong pos = 0;
  switch (origin) {
  case SEEK_BEGIN:
    pos = offset;
    break;
  case SEEK_CURRENT:
    pos = s->position + offset;
    break;
  case SEEK_END:
    pos = s->length + offset;
    break;
  }
  s->position = pos;
}
gbool g_memory_stream_is_eof(gpointer self) {
  return g_memory_stream_get_position(self) >= g_memory_stream_get_length(self);
}
void g_memory_stream_init(gpointer self) {
  GMemoryStream *s = GMEMORYSTREAM(self);
  g_stream_init(self);
  s->capacity = s->length = s->position = 0;
  s->buffer = NULL;
}
void g_memory_stream_finalize(gpointer self) {
  GMemoryStream *s = GMEMORYSTREAM(self);
  if (s->buffer != NULL) {
    g_free(s->buffer);
    s->buffer = NULL;
  }
  g_stream_finalize(self);
}
void g_memory_stream_class_init(gpointer c) {
  g_stream_class_init(c);
  GSTREAMCLASS(c)->get_length = g_memory_stream_get_length;
  GSTREAMCLASS(c)->set_length = g_memory_stream_set_length;
  GSTREAMCLASS(c)->get_position = g_memory_stream_get_position;
  GSTREAMCLASS(c)->read = g_memory_stream_read;
  GSTREAMCLASS(c)->write = g_memory_stream_write;
  GSTREAMCLASS(c)->seek = g_memory_stream_seek;
  GSTREAMCLASS(c)->flush = g_memory_stream_flush;
  GSTREAMCLASS(c)->is_eof = g_memory_stream_is_eof;
}

gpointer g_class_GMemoryStream() {
  gpointer clazz = g_class(CLASS_NAME(GMemoryStream));
  return clazz != NULL
             ? clazz
             : REGISTER_CLASS(GMemoryStream, GStream, g_memory_stream_init,
                              g_memory_stream_finalize,
                              g_memory_stream_class_init, NULL);
}