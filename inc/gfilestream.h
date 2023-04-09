/*
 * Copyright (c) 2023 Gembera Team
 * https://github.com/gembera
 * License: MIT
 */

#ifndef _G_FILE_STREAM_H_
#define _G_FILE_STREAM_H_
#include "gstream.h"

#ifdef __cplusplus
extern "C" {
#pragma }
#endif /* __cplusplus */

#define GFILESTREAM(p) ((GFileStream *)(p))
#define GFILESTREAMCLASS(p) ((GFileStreamClass *)(p))
typedef struct _GFileStream GFileStream;
typedef struct _GFileStreamClass GFileStreamClass;
typedef enum
{
	FILE_MODE_READ_ONLY,
	FILE_MODE_READ_WRITE,
	FILE_MODE_CREATE_NEW
}GFileMode;
struct _GFileStream
{
	GStream base;
	ghandle file_handle;
	gstring file_name;
	GFileMode file_mode;
};
struct _GFileStreamClass
{
	GStreamClass base;
};

gint g_file_stream_open(gpointer self, gstring name, GFileMode mode);
void g_file_stream_reopen(gpointer self);
void g_file_stream_close(gpointer self);
void g_file_stream_set_length(gpointer self, gint len);
gint g_file_stream_get_length(gpointer self);
gint g_file_stream_get_position(gpointer self);
gint g_file_stream_write(gpointer self, void * buffer, gint length);
gint g_file_stream_read(gpointer self, void * buffer, gint length);
void g_file_stream_seek(gpointer self, gint offset, GSeekOrigin origin);
void g_file_stream_flush(gpointer self);
gboolean g_file_stream_is_eof(gpointer self);
gboolean g_file_stream_is_open(gpointer self);

gpointer g_get_class_GFileStream(void);

void g_file_stream_init(gpointer self);
void g_file_stream_finalize(gpointer self);
void g_file_stream_class_init(gpointer clazz);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif