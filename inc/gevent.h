/*
 * Copyright (c) 2023 Gembera Team
 * https://github.com/gembera
 * License: MIT
 */

#ifndef _G_EVENT_H_
#define _G_EVENT_H_
#include "glib.h"

typedef void (*EVENT_CALLBACK)(gpointer self, gpointer data, gpointer args);

typedef struct
{
	gbool consumed;
	gpointer value;
} GEventArgs;

typedef struct _GEvent GEvent;
struct _GEvent
{
	gpointer sender;
	GList *callback_list;
	GList *data_list;
	gbool event_firing;
	GPtrArray *items_to_free;
};

GEvent *g_event_new(gpointer sender);
void g_event_free(GEvent *self);

void g_event_add_listener(GEvent *self, EVENT_CALLBACK callback, gpointer data);
void g_event_remove_listener(GEvent *self, EVENT_CALLBACK callback, gpointer data);
void g_event_fire(GEvent *self, gpointer args);

#endif