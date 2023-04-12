/*
 * Copyright (c) 2023 Gembera Team
 * https://github.com/gembera
 * License: MIT
 */

#include "gevent.h"
/*
GEvent * g_event_new(gpointer sender)
{
	GEvent * self = g_new(GEvent);
	self->sender = sender;
	self->callback_list = NULL;
	self->data_list = NULL;
	self->event_firing = FALSE;
	self->items_to_free = NULL;
	return (GEvent *)self;
}
void g_event_free(GEvent * self)
{
	if (self == NULL) return;
	
	if (self->callback_list != NULL)
		g_list_free(self->callback_list);
	if (self->data_list != NULL)
		g_list_free(self->data_list);

	g_free(self);
}

void g_event_add_listener(GEvent * self, EVENT_CALLBACK callback, gpointer data)
{
	GList *callback_list, *data_list;
	g_return_if_fail(self != NULL);

	callback_list = self->callback_list;
	data_list = self->data_list;
	for(; callback_list != NULL; callback_list = g_list_next(callback_list), data_list = g_list_next(data_list))
	{
		if (callback_list->data == (gpointer)callback && data_list->data == data) return;
	}
	self->callback_list = g_list_append(self->callback_list, (gpointer)callback);
	self->data_list = g_list_append(self->data_list, data);
}
void g_event_remove_listener(GEvent * self, EVENT_CALLBACK callback, gpointer data)
{
	GList *callback_list, *data_list;
	g_return_if_fail(self != NULL);

	callback_list = self->callback_list;
	data_list = self->data_list;
	for(; callback_list != NULL; callback_list = g_list_next(callback_list), data_list = g_list_next(data_list))
	{
		if (callback_list->data == (gpointer)callback && data_list->data == data)
		{
			self->callback_list = g_list_remove_link(self->callback_list, callback_list);
			self->data_list = g_list_remove_link(self->data_list, data_list);
			if (self->event_firing)
			{
				if (self->items_to_free == NULL) self->items_to_free = g_ptr_array_new();
				g_ptr_array_add(self->items_to_free, callback_list);
				g_ptr_array_add(self->items_to_free, data_list);		
			}
			else
			{
				g_list_free_1(callback_list);
				g_list_free_1(data_list);
			}
			break;
		}
	}
}
void g_event_fire(GEvent * self, gpointer args)
{
	int i;
	GList *callback_list, *data_list;
	g_return_if_fail(self != NULL);

	callback_list = self->callback_list;
	data_list = self->data_list;
	self->event_firing = TRUE;
	for(; callback_list != NULL; callback_list = g_list_next(callback_list), data_list = g_list_next(data_list))
	{
		((EVENT_CALLBACK)callback_list->data)(self->sender, data_list->data, args);
	}
	self->event_firing = FALSE;
	if (self->items_to_free != NULL && g_ptr_array_size(self->items_to_free) > 0)
	{
		for(i = 0; i < g_ptr_array_size(self->items_to_free); i++)
		{
			g_list_free_1((GList*)g_ptr_array_get(self->items_to_free, i));
		}
		g_ptr_array_set_size(self->items_to_free, 0);
	}
}
*/