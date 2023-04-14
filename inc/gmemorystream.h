/*
 * Copyright (c) 2023 Gembera Team
 * https://github.com/gembera
 * License: MIT
 */

#ifndef _G_MEMORY_STREAM_H_
#define _G_MEMORY_STREAM_H_
#include "gstream.h"

#define GMEMORYSTREAM(p) ((GMemoryStream *)(p))
#define GMEMORYSTREAMCLASS(p) ((GMemoryStreamClass *)(p))
typedef struct _GMemoryStream GMemoryStream;
typedef struct _GMemoryStreamClass GMemoryStreamClass;

struct _GMemoryStream
{
	GStream base;
	gint position;
	gint length;
	void * buffer;
	gint capacity;
};
struct _GMemoryStreamClass
{
	GStreamClass base;
};
void g_memory_stream_set_length(gpointer self, gint len);
gint g_memory_stream_get_length(gpointer self);
gint g_memory_stream_get_position(gpointer self);
gint g_memory_stream_write(gpointer self, void * buffer, gint length);
gint g_memory_stream_read(gpointer self, void * buffer, gint length);
void g_memory_stream_seek(gpointer self, gint offset, GSeekOrigin origin);
void g_memory_stream_set_capacity(gpointer self, gint capacity);
void g_memory_stream_flush(gpointer self);
gbool g_memory_stream_is_eof(gpointer self);
gpointer g_class_GMemoryStream(void);

void g_memory_stream_init(gpointer self);
void g_memory_stream_finalize(gpointer self);
void g_memory_stream_class_init(gpointer clazz);

#endif