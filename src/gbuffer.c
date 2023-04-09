/*
 * Copyright (c) 2023 Gembera Team
 * https://github.com/gembera
 * License: MIT
 */

#include "glib.h"

#define DEFAULT_BUFFER_LENGTH 4096

typedef struct _GBufferItem GBufferItem;
struct _GBufferItem
{
	GBufferItem * next;
};

static GBufferItem * g_buffer_free_list = NULL;
static gint g_buffer_default_length = 0;

void g_buffer_set_length(gint len)
{
	if (g_buffer_default_length == 0)
		g_buffer_default_length = len;
}
gint g_buffer_get_length()
{
	if (g_buffer_default_length == 0)
		g_buffer_default_length = DEFAULT_BUFFER_LENGTH;
	return g_buffer_default_length;
}
gpointer g_buffer_get()
{
	gpointer buf = NULL;
	if (g_buffer_free_list != NULL)
	{
		buf = g_buffer_free_list;
		g_buffer_free_list = g_buffer_free_list->next;
	}
	else
	{
		buf = g_malloc(g_buffer_get_length());
	}
	return buf;
}
void g_buffer_release(gpointer buf)
{
	if (g_buffer_free_list == NULL)
	{
		g_buffer_free_list = (GBufferItem *)buf;
		g_buffer_free_list->next = NULL;
	}
	else
	{
		((GBufferItem *)buf)->next = g_buffer_free_list;
		g_buffer_free_list = (GBufferItem *)buf;
	}
}