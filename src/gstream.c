/*
 * Copyright (c) 2023 Gembera Team
 * https://github.com/gembera
 * License: MIT
 */

#include "gstream.h"
#include "gfilestream.h"

gint32 g_stream_read_int32(gpointer self, gint32* data)
{
	return g_stream_virtual_read(self, (void*)data, 4);
}
gfloat g_stream_read_float(gpointer self)
{
	gfloat data;
	g_stream_virtual_read(self, (void*)&data, 4);
	return data;
}
gint16 g_stream_read_int16(gpointer self)
{
	gint16 data;
	g_stream_virtual_read(self, (void*)&data, 2);
	return data;
}
gstring g_stream_read_string_with_length(gpointer self)
{
	gint16 fnlen = 0;
	gstring fn = NULL;
	fnlen = g_stream_read_int16(self);
	if (fnlen < 0) return fn;
	fn = g_malloc(fnlen + 1);
	fn[fnlen] = '\0';
	g_stream_virtual_read(self, fn, fnlen);
	return fn;
}

void g_stream_write_int32(gpointer self, gint32 data)
{
	g_stream_virtual_write(self, (void*)&data, 4);
}
void g_stream_write_float(gpointer self, gfloat data)
{
	g_stream_virtual_write(self, (void*)&data, 4);
}
void g_stream_write_int16(gpointer self, gint16 data)
{
	g_stream_virtual_write(self, (void*)&data, 2);
}
void g_stream_write_string_with_length(gpointer self, gstring text)
{
	gint16 len = strlen(text);
	g_stream_write_int16(self, len);
	g_stream_virtual_write(self, text, len);
}
void g_stream_read_all_content(gpointer self, gpointer* retbuffer, gint* retbuflen, gbool appendzero)
{
	gpointer buffer = NULL;
	gint buflen = 0;
	buflen = g_stream_virtual_get_length(self);
	buffer = (guint8 *)g_malloc(buflen + (appendzero ? 1 : 0));
	g_stream_virtual_seek(self, 0, SEEK_BEGIN);
	g_stream_virtual_read(self, buffer, buflen);
	if (appendzero)
		((gchar*)buffer)[buflen] = '\0';
	*retbuffer = buffer;
	*retbuflen = buflen;
}

gint g_stream_get_length(gstring uri)
{
	gstring path = NULL;
	gint length = 0;
	// if (g_strstartwith(uri, PREFIX_RES))
	// {
	// 	GResourceStream * rs = (GResourceStream *)g_new_object(CLASS(GResourceStream));
	// 	path = g_strsubstring(uri, strlen(PREFIX_RES), -1);
	// 	g_resource_stream_open(rs, path);
	// 	if (!g_resource_stream_is_open(rs))
	// 	{
	// 		g_log_error("g_stream_get_length fails to open %s", path);
	// 	}
	// 	else
	// 	{
	// 		length = g_resource_stream_get_length(rs);
	// 		g_delete_object(rs);
	// 	}
	// }
	// else 
	if (g_strstartwith(uri, PREFIX_FILE))
	{
		GFileStream * fs = (GFileStream *)g_new_object(CLASS(GFileStream));
		path = g_strsubstring(uri, strlen(PREFIX_FILE), -1);
		g_file_stream_open(fs, path, FILE_MODE_READ_ONLY);
		if (!g_file_stream_is_open(fs))
		{
			g_log_error("g_stream_get_length fails to open %s", path);
		}
		else
		{
			length = g_file_stream_get_length(fs);
			g_delete_object(fs);
		}
	}
	// else if (g_strstartwith(uri, PREFIX_PACKAGE))
	// {
	// 	GPackage * package;
	// 	gint pos;
	// 	path = g_strsubstring(uri, strlen(PREFIX_PACKAGE), -1);
	// 	pos = g_strindexof(path, "/", 0);
	// 	path[pos] = '\0';
	// 	package = (GPackage *)g_package_get(path);
	// 	if (package != NULL)
	// 	{
	// 		GPackageItem * item = g_package_get_item(package, path + pos + 1);			
	// 		if (item != NULL && item->length > 0)
	// 		{
	// 			length = item->length;	
	// 		}
	// 	}
	// 	path[pos] = '/';
	// }
	else
	{
		GFileStream * fs = (GFileStream *)g_new_object(CLASS(GFileStream));
		g_file_stream_open(fs, uri, FILE_MODE_READ_ONLY);
		length = g_file_stream_get_length(fs);
		g_delete_object(fs);
	}
	if (path) g_free(path);
	return length;

}
void g_stream_read_all(gstring uri, gpointer* retbuffer, gint* retbuflen, gbool appendzero)
{
	gpointer buffer = NULL;
	gint buflen = 0;
	gstring path = NULL;
	// if (g_strstartwith(uri, PREFIX_RES))
	// {
	// 	GResourceStream * rs = (GResourceStream *)g_new_object(CLASS(GResourceStream));
	// 	path = g_strsubstring(uri, strlen(PREFIX_RES), -1);
	// 	g_resource_stream_open(rs, path);
	// 	if (!g_resource_stream_is_open(rs))
	// 	{
	// 		g_log_error("g_stream_read_all fails to open %s", path);
	// 	}
	// 	else
	// 	{
	// 		buflen = g_resource_stream_get_length(rs);
	// 		g_log_debug("g_stream_read_all %s, length=%d", path, buflen);
	// 		buffer = (guint8 *)g_malloc(buflen + (appendzero ? 1 : 0));
	// 		g_resource_stream_read(rs, buffer, buflen);
	// 		g_delete_object(rs);
	// 	}
	// }
	// else 
	if (g_strstartwith(uri, PREFIX_FILE))
	{
		GFileStream * fs = (GFileStream *)g_new_object(CLASS(GFileStream));
		path = g_strsubstring(uri, strlen(PREFIX_FILE), -1);
		g_file_stream_open(fs, path, FILE_MODE_READ_ONLY);
		if (!g_file_stream_is_open(fs))
		{
			g_log_error("g_stream_read_all fails to open %s", path);
		}
		else
		{
			buflen = g_file_stream_get_length(fs);
			g_log_debug("g_stream_read_all %s, length=%d", path, buflen);
			buffer = (guint8 *)g_malloc(buflen + (appendzero ? 1 : 0));
			g_file_stream_read(fs, buffer, buflen);
			g_delete_object(fs);
		}
	}
	// else if (g_strstartwith(uri, PREFIX_PACKAGE))
	// {
	// 	GPackage * package;
	// 	gint pos;
	// 	path = g_strsubstring(uri, strlen(PREFIX_PACKAGE), -1);
	// 	pos = g_strindexof(path, "/", 0);
	// 	path[pos] = '\0';
	// 	package = (GPackage *)g_package_get(path);
	// 	if (package != NULL)
	// 	{
	// 		GPackageItem * item = g_package_get_item(package, path + pos + 1);
	// 		if (item != NULL && item->length > 0)
	// 		{
	// 			buflen = item->length;
	// 			buffer = g_malloc(buflen + (appendzero ? 1 : 0));
	// 			g_package_item_read(item, buffer);			
	// 		}
	// 	}
	// 	path[pos] = '/';
	// }
	else
	{
		GFileStream * fs = (GFileStream *)g_new_object(CLASS(GFileStream));
		g_file_stream_open(fs, uri, FILE_MODE_READ_ONLY);
		buflen = g_file_stream_get_length(fs);
		buffer = (guint8 *)g_malloc(buflen + (appendzero ? 1 : 0));
		g_file_stream_read(fs, buffer, buflen);
		g_delete_object(fs);
	}
	if (path) g_free(path);

	if (buffer != NULL && appendzero)
		((gchar*)buffer)[buflen] = '\0';
	*retbuffer = buffer;
	*retbuflen = buffer == NULL ? 0 : buflen;
}
void g_stream_write_byte(gpointer self, gint bi)
{
	guint8 b = (guint8)(bi & 0xFF);
	g_stream_virtual_write(self, &b, 1);
}
gint g_stream_read_byte(gpointer self)
{
	guint8 b = 0;
	g_stream_virtual_read(self, &b, 1);
	return b;
}

void g_stream_init(gpointer self)
{
	g_object_init(self);
}
void g_stream_finalize(gpointer self)
{
	g_object_finalize(self);
}
void g_stream_class_init(gpointer c)
{
	g_object_class_init(c);
}

gpointer g_get_class_GStream()
{
	gpointer clazz = g_get_class(CLASS_NAME(GStream));
	return clazz != NULL ? clazz :
		REGISTER_CLASS(GStream, GObject, g_stream_init, g_stream_finalize, g_stream_class_init, NULL);
}