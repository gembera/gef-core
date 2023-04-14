/*
 * Copyright (c) 2023 Gembera Team
 * https://github.com/gembera
 * License: MIT
 */

#include "gfilestream.h"

#include <stdio.h>
#include <unistd.h>

static int open_file_count = 0;
//#define GET_MODE(mode) (mode == FILE_MODE_READ_ONLY ? MODE_READ : (mode ==
// FILE_MODE_READ_WRITE ? MODE_WRITE : MODE_CREATE_ALWAYS_WRITE))
#define GET_MODE(mode)                                                         \
  (mode == FILE_MODE_READ_ONLY                                                 \
       ? "rb"                                                                  \
       : (mode == FILE_MODE_READ_WRITE ? "rb+" : "wb+"))

void g_file_stream_reopen(gpointer self) {
  GFileStream *s = GFILESTREAM(self);
  g_return_if_fail(self != NULL && !g_file_stream_is_open(self));
  s->file_handle = (ghandle)freopen(
      s->file_name, s->file_mode == FILE_MODE_READ_ONLY ? "r" : "rw",
      s->file_handle);
  if (g_file_stream_is_open(s))
    open_file_count++;
  // g_log_debug("reopen file %s : %x (count = %d)\n", s->file_name,
  // GPOINTER_TO_INT(s->file_handle), open_file_count);
}
gint g_file_stream_open(gpointer self, gstr name, GFileMode mode) {
  GFileStream *s = GFILESTREAM(self);
  if (!(self != NULL && !g_file_stream_is_open(self))) {
    g_log_error("g_file_stream_open error: %x %s %d\n", g_pointer_to_num(self),
                name, mode);
  }
  g_return_val_if_fail(self != NULL && !g_file_stream_is_open(self), -1);
  if (s->file_name)
    g_free(s->file_name);
  // s->file_name = g_file_get_normalize_path(name);
  s->file_name = g_dup(name);
  s->file_mode = mode;
  s->file_handle = (ghandle)fopen(s->file_name, GET_MODE(s->file_mode));
  if (s->file_handle == NULL) {
    g_log_error("open file error: %s %s\n", s->file_name,
                GET_MODE(s->file_mode));
    return -1;
  }
  if (g_file_stream_is_open(s))
    open_file_count++;
  // g_log_debug("open file %s : %x (count = %d)\n", s->file_name,
  // GPOINTER_TO_INT(s->file_handle), open_file_count);

  return 0;
}
gbool g_file_stream_is_open(gpointer self) {
  GFileStream *s = GFILESTREAM(self);
  g_return_val_if_fail(self != NULL, FALSE);
  return s->file_handle != NULL;
}
void g_file_stream_set_length(gpointer self, glong len) {}
glong g_file_stream_get_length(gpointer self) {
  glong size = 0;
  GFileStream *s = GFILESTREAM(self);
  g_return_val_if_fail(g_file_stream_is_open(self), 0);
  glong offset = ftell((FILE *)s->file_handle);

  fseek((FILE *)s->file_handle, 0L, SEEK_END);
  size = ftell((FILE *)s->file_handle);

  fseek((FILE *)s->file_handle, offset, SEEK_SET); // 返回
  // g_log_debug("offset=%d, size=%d, mode=%d\n", offset, size, s->file_mode);
  return (glong)size;
}
glong g_file_stream_get_position(gpointer self) {
  GFileStream *s = GFILESTREAM(self);
  g_return_val_if_fail(g_file_stream_is_open(self), 0);
  return (glong)ftell((FILE *)s->file_handle);
}
glong g_file_stream_write(gpointer self, gcstr buffer, glong length) {
  glong written = 0;
  GFileStream *s = GFILESTREAM(self);
  if (!g_file_stream_is_open(self)) {
    g_log_error("g_file_stream_is_open(self) error: %d %s\n", s->file_handle,
                s->file_name);
  }
  g_return_val_if_fail(g_file_stream_is_open(self), -1);
  written = fwrite(buffer, 1, length, (FILE *)s->file_handle);
  return (glong)written;
}
glong g_file_stream_read(gpointer self, gstr buffer, glong length) {
  glong nread = 0;
  GFileStream *s = GFILESTREAM(self);
  g_return_val_if_fail(g_file_stream_is_open(self), 0);
  nread = fread(buffer, 1, length, (FILE *)s->file_handle);
  // g_log_debug("g_file_stream_read:%d\n", nread);
  return (glong)nread;
}
void g_file_stream_seek(gpointer self, glong offset, GSeekOrigin origin) {
  GFileStream *s = GFILESTREAM(self);
  g_return_if_fail(g_file_stream_is_open(self));
  switch (origin) {
  case SEEK_BEGIN:
    fseek((FILE *)s->file_handle, offset, SEEK_SET);
    break;
  case SEEK_CURRENT:
    fseek((FILE *)s->file_handle, offset, SEEK_CUR);
    break;
  case SEEK_END:
    fseek((FILE *)s->file_handle, offset, SEEK_END);
    break;
  }
}
void g_file_stream_close(gpointer self) {
  if (g_file_stream_is_open(self)) {
    open_file_count--;
    fclose((FILE *)GFILESTREAM(self)->file_handle);
    // g_log_debug("close file %s : %x (count = %d)\n",
    // GFILESTREAM(self)->file_name, GFILESTREAM(self)->file_handle,
    // open_file_count);
    GFILESTREAM(self)->file_handle = (ghandle)-1;
  }
}
void g_file_stream_flush(gpointer self) {
  GFileStream *s = GFILESTREAM(self);
  g_return_if_fail(g_file_stream_is_open(self));
  fflush((FILE *)s->file_handle);
}
gbool g_file_stream_is_eof(gpointer self) {
  GFileStream *s = GFILESTREAM(self);
  g_return_val_if_fail(g_file_stream_is_open(self), TRUE);
  int eof = feof((FILE *)s->file_handle);
  return eof;
}

void g_file_stream_init(gpointer self) {
  g_stream_init(self);
  GFILESTREAM(self)->file_handle = NULL;
  GFILESTREAM(self)->file_name = NULL;
}
void g_file_stream_finalize(gpointer self) {
  if (g_file_stream_is_open(self))
    g_file_stream_close(self);
  if (GFILESTREAM(self)->file_name)
    g_free(GFILESTREAM(self)->file_name);
  GFILESTREAM(self)->file_name = NULL;
  g_stream_finalize(self);
}
void g_file_stream_class_init(gpointer c) {
  g_stream_class_init(c);
  GSTREAMCLASS(c)->get_length = g_file_stream_get_length;
  GSTREAMCLASS(c)->set_length = g_file_stream_set_length;
  GSTREAMCLASS(c)->get_position = g_file_stream_get_position;
  GSTREAMCLASS(c)->read = g_file_stream_read;
  GSTREAMCLASS(c)->write = g_file_stream_write;
  GSTREAMCLASS(c)->seek = g_file_stream_seek;
  GSTREAMCLASS(c)->flush = g_file_stream_flush;
  GSTREAMCLASS(c)->is_eof = g_file_stream_is_eof;
}

gpointer g_class_GFileStream() {
  gpointer clazz = g_class(CLASS_NAME(GFileStream));
  return clazz != NULL
             ? clazz
             : REGISTER_CLASS(GFileStream, GStream, g_file_stream_init,
                              g_file_stream_finalize, g_file_stream_class_init,
                              NULL);
}
