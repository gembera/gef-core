/*
 * Copyright (c) 2023 Gembera Team
 * https://github.com/gembera
 * License: MIT
 */

#ifndef __G_MEMORY_STREAM_H__
#define __G_MEMORY_STREAM_H__
#include "gstream.h"

#define GMEMORYSTREAM(p) ((GMemoryStream *)(p))
#define GMEMORYSTREAMCLASS(p) ((GMemoryStreamClass *)(p))
typedef struct _GMemoryStream GMemoryStream;
typedef struct _GMemoryStreamClass GMemoryStreamClass;

struct _GMemoryStream {
  GStream base;
  glong position;
  glong length;
  gstr buffer;
  glong capacity;
};
struct _GMemoryStreamClass {
  GStreamClass base;
};
void g_memory_stream_set_length(gpointer self, glong len);
glong g_memory_stream_get_length(gpointer self);
glong g_memory_stream_get_position(gpointer self);
glong g_memory_stream_write(gpointer self, gcstr buffer, glong length);
glong g_memory_stream_read(gpointer self, gstr buffer, glong length);
void g_memory_stream_seek(gpointer self, glong offset, GSeekOrigin origin);
void g_memory_stream_set_capacity(gpointer self, glong capacity);
void g_memory_stream_flush(gpointer self);
gbool g_memory_stream_is_eof(gpointer self);
gpointer g_class_GMemoryStream(void);

void g_memory_stream_init(gpointer self);
void g_memory_stream_finalize(gpointer self);
void g_memory_stream_class_init(gpointer clazz);

#endif