/*
 * Copyright (c) 2023 Gembera Team
 * https://github.com/gembera
 * License: MIT
 */

#include "gobject.h"

static GHashTable *name_to_class_table = NULL;

GObject *g_new_object(GObjectClass *clazz)
{
	GObject *object;

	g_return_val_if_fail(clazz != NULL, NULL);

	object = g_malloc(clazz->instance_size);
	memset(object, 0, clazz->instance_size);
	object->g_class = clazz;
	// object->properties = g_hash_table_new(g_str_hash, g_str_equal);
	if (clazz->init != NULL)
		clazz->init(object);

	return object;
}

void g_delete_object(gpointer s)
{
	GObject *self = (GObject *)s;
	if (s == NULL)
		return;
	g_return_if_fail(self->g_class != NULL);

	if (self->g_class->finalize != NULL)
		self->g_class->finalize(self);

	g_free(self);
}

void g_object_init(gpointer self)
{
}
void g_object_finalize(gpointer self)
{
}
void g_object_class_init(gpointer clazz)
{
}

#define INIT_CLASS_SYSTEM            \
	if (name_to_class_table == NULL) \
		g_init_class_system();
void g_init_class_system()
{
	if (name_to_class_table == NULL)
	{
		name_to_class_table = g_hash_table_new(g_str_hash, g_str_equal);
	}
}
void name_to_class_table_destroy(gpointer key, gpointer value, gpointer user_data)
{
	GObjectClass *clazz = (GObjectClass *)value;
	if (clazz->g_class_finalize != NULL)
		clazz->g_class_finalize(clazz);
	g_free(clazz->g_class_name);
	g_free(clazz);
}
void g_finalize_class_system()
{
	if (name_to_class_table != NULL)
	{
		g_hash_table_foreach(name_to_class_table, name_to_class_table_destroy, NULL);
		g_hash_table_destroy(name_to_class_table);
	}
}
GObjectClass *g_register_class(const gchar *classname, GObjectClass *baseclazz, gint class_size, GObjectClassInitFunc class_init, GObjectClassFinalizeFunc class_finalize,
							   gint instance_size, GObjectInitFunc init, GObjectFinalizeFunc finalize)
{
	GObjectClass *clazz;
	g_return_val_if_fail(classname != NULL, NULL);

	clazz = g_get_class(classname);
	if (clazz == NULL)
	{
		clazz = (GObjectClass *)g_malloc(class_size);
		memset(clazz, 0, class_size);
		clazz->g_base_class = baseclazz;
		clazz->g_class_finalize = class_finalize;
		clazz->g_class_name = g_strdup(classname);
		clazz->instance_size = instance_size;
		clazz->init = init;
		clazz->finalize = finalize;
		if (class_init != NULL)
			class_init(clazz);
		g_hash_table_insert(name_to_class_table, clazz->g_class_name, clazz);
	}
	return clazz;
}

gboolean g_is_type_of(GObjectClass *thisclazz, GObjectClass *targetclazz)
{
	while (thisclazz != NULL && thisclazz != targetclazz)
		thisclazz = thisclazz->g_base_class;
	return thisclazz == targetclazz;
}
gboolean g_is_instance_of(gpointer self, GObjectClass *targetclazz)
{
	return g_is_type_of(GOBJECT(self)->g_class, targetclazz);
}
GObjectClass *g_get_class_GObject()
{
	gpointer clazz = g_get_class(CLASS_NAME(GObject));
	return clazz != NULL ? clazz : g_register_class("GObjectClass", NULL, sizeof(GObjectClass), NULL, NULL, sizeof(GObject), NULL, NULL);
}
GObjectClass *g_get_class(const gchar *classname)
{
	INIT_CLASS_SYSTEM
	return (GObjectClass *)g_hash_table_lookup(name_to_class_table, classname);
}
const gchar *g_get_class_name(GObjectClass *clazz)
{
	return clazz == NULL ? NULL : clazz->g_class_name;
}

void static_clean_gobject()
{
	g_finalize_class_system();
}