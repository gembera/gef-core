/*
 * Copyright (c) 2023 Gembera Team
 * https://github.com/gembera
 * License: MIT
 */

#ifndef _G_STREAM_H_
#define _G_STREAM_H_
#include "gobject.h"

#define GSTREAM(p) ((GStream *)(p))
#define GSTREAMCLASS(p) ((GStreamClass *)(p))
typedef struct _GStream GStream;
typedef struct _GStreamClass GStreamClass;

#ifdef SEEK_END
#undef SEEK_END
#endif

typedef enum
{
	SEEK_BEGIN = 0,
	SEEK_CURRENT = 1,
	SEEK_END = 2
}GSeekOrigin;

struct _GStream
{
	GObject base;
};
struct _GStreamClass
{
	GObjectClass base;
	gint (*get_length)(gpointer self);
	void (*set_length)(gpointer self, gint len);
	gint (*get_position)(gpointer self);
	gint (*read)(gpointer self, void * buffer, gint length);
	gint (*write)(gpointer self, void * buffer, gint length);
	void (*seek)(gpointer self, gint offset, GSeekOrigin origin);
	void (*flush)(gpointer self);
	gbool (*is_eof)(gpointer self);
};

#define g_stream_virtual_get_length(self) GSTREAMCLASS(GOBJECT(self)->g_class)->get_length(self)
#define g_stream_virtual_set_length(self, len) GSTREAMCLASS(GOBJECT(self)->g_class)->set_length(self, len)
#define g_stream_virtual_get_position(self) GSTREAMCLASS(GOBJECT(self)->g_class)->get_position(self)
#define g_stream_virtual_read(self, buf, len) GSTREAMCLASS(GOBJECT(self)->g_class)->read(self, buf, len)
#define g_stream_virtual_write(self, buf, len) GSTREAMCLASS(GOBJECT(self)->g_class)->write(self, buf, len)
#define g_stream_virtual_seek(self, offset, origin) GSTREAMCLASS(GOBJECT(self)->g_class)->seek(self, offset, origin)
#define g_stream_virtual_flush(self) GSTREAMCLASS(GOBJECT(self)->g_class)->flush(self)
#define g_stream_virtual_is_eof(self) GSTREAMCLASS(GOBJECT(self)->g_class)->is_eof(self)

void g_stream_write_byte(gpointer self, gint b);
gint g_stream_read_byte(gpointer self);
#define g_stream_write_string(self, str) g_stream_virtual_write(self, str, g_len(str));

gpointer g_class_GStream(void);

void g_stream_init(gpointer self);
void g_stream_finalize(gpointer self);
void g_stream_class_init(gpointer clazz);

void g_stream_read_all_content(gpointer self, gpointer* buffer, gint*buflen, gbool appendzero);
gint32 g_stream_read_int32(gpointer self, gint32* data);
gfloat g_stream_read_float(gpointer self);
gint16 g_stream_read_int16(gpointer self);
gstring g_stream_read_string_with_length(gpointer self);
void g_stream_write_int32(gpointer self, gint32 data);
void g_stream_write_float(gpointer self, gfloat data);
void g_stream_write_int16(gpointer self, gint16 data);
void g_stream_write_string_with_length(gpointer self, gstring text);
#endif