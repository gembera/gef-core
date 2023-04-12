/*
 * Copyright (c) 2023 Gembera Team
 * https://github.com/gembera
 * License: MIT
 */

#include "gvalue.h"
#include "gobject.h"

/* --- GValue functions --- */
GValue *       
g_value_new()
{
	GValue * val;

	val = g_new (GValue);
	val->g_type = G_TYPE_NULL;
	return val;
}
void  
g_value_copy(GValue * v1, GValue * v2)
{
	g_return_if_fail(v1 != NULL && v2 != NULL);
	g_value_destroy_data(v1);
	*v1 = *v2;
	if (G_IS_VALUE_STRING(v2))
		v1->data.v_pointer = g_value_dup_string(v2);
	else if (G_IS_VALUE_POINTER(v2))
		v1->g_type = G_TYPE_REFERENCE;
}
GValue *       
g_value_new_with_value(GValue * value)
{
	GValue *val = g_value_new();
	g_value_copy(val, value);
	return val;
}
void g_value_destroy_hash_table_item(gpointer key, gpointer value, gpointer user_data)
{
	GValue * v = (GValue*)user_data;
	if (v->g_type == G_TYPE_HASH_TABLE)	g_free(key);
	g_value_destroy((GValue *)value);
}
void
g_value_destroy_data(GValue * value){
	g_return_if_fail(value != NULL);
	if (G_IS_VALUE_STRING(value) || G_IS_VALUE_POINTER(value))
		g_free(value->data.v_pointer);
	else if (G_IS_VALUE_ARRAY(value))
	{
		int i;
		GPtrArray* a = (GPtrArray*)value->data.v_pointer;
		for(i = 0; i < g_ptr_array_size(a); i++)
		{
			GValue * vi = g_ptr_array_get(a, i);
			g_value_destroy(vi);
		}
		g_ptr_array_free(a);
	}
	else if (G_IS_VALUE_HASH_TABLE(value))
	{
		GHashTable* ht = (GHashTable*)value->data.v_pointer;
		g_hash_table_foreach(ht, g_value_destroy_hash_table_item, value);
		g_hash_table_destroy(ht);
	}
	else if (G_IS_VALUE_OBJECT(value))
	{
		g_delete_object((GObject*)value->data.v_pointer);
	}
}

void     
g_value_destroy(GValue * value)
{
	if (value){
		g_value_destroy_data(value);
		g_free(value);
	}
}
void
g_value_set_boolean (GValue  *value,
					 gbool v_boolean)
{
	g_value_destroy_data(value);
	value->g_type = G_TYPE_BOOLEAN;
	value->data.v_int = v_boolean;
}

gbool
g_value_get_boolean (GValue *value)
{
	g_return_val_if_fail (G_IS_VALUE_BOOLEAN (value), 0);

	return value->data.v_int;
}

void
g_value_set_int (GValue *value,
				 gint	 v_int)
{
	g_value_destroy_data(value);
	value->g_type = G_TYPE_INT;
	value->data.v_int = v_int;
}

gint
g_value_get_int (GValue *value)
{
	if (G_IS_VALUE_INT (value) || G_IS_VALUE_LONG (value))
		return value->data.v_int;
	else if (G_IS_VALUE_STRING(value))
	{
		gstring str = g_value_get_string(value);
		return g_strtointeger(str);
	}
	return 0;	
}

void
g_value_set_long (GValue *value,
				  gint64  v_long)
{
	g_value_destroy_data(value);
	value->g_type = G_TYPE_LONG;
	value->data.v_int = (gint)v_long;
}

gint64
g_value_get_long (GValue *value)
{
	g_return_val_if_fail (G_IS_VALUE_LONG (value) || G_IS_VALUE_INT(value), 0);
	
	return value->data.v_int;
}

void
g_value_set_double (GValue *value,
					gdouble v_double)
{
	g_value_destroy_data(value);
	value->g_type = G_TYPE_DOUBLE;
	value->data.v_float = (gfloat)v_double;
}

gdouble
g_value_get_double (GValue *value)
{
	g_return_val_if_fail (G_IS_VALUE_DOUBLE (value), 0);

	return value->data.v_float;
}

void
g_value_set_string (GValue	*value,
					const gchar *v_string)
{
	g_value_destroy_data(value);
	value->g_type = G_TYPE_STRING;
	value->data.v_pointer = g_strdup (v_string);
}

gchar*
g_value_get_string (GValue *value)
{
	g_return_val_if_fail (G_IS_VALUE_STRING (value) || G_IS_VALUE_REFERENCE(value), NULL);

	return value->data.v_pointer;
}

gchar*
g_value_dup_string (GValue *value)
{
	g_return_val_if_fail (G_IS_VALUE_STRING (value) || G_IS_VALUE_REFERENCE(value), NULL);

	return g_strdup (value->data.v_pointer);
}


void           
g_value_set_pointer (GValue *value, gpointer v_pointer)
{
	g_value_destroy_data(value);
	value->g_type = G_TYPE_POINTER;
	value->data.v_pointer = v_pointer;
}
gpointer        
g_value_get_pointer (GValue *value)
{
	g_return_val_if_fail(G_IS_VALUE_POINTER(value) || G_IS_VALUE_REFERENCE(value), NULL);

	return value->data.v_pointer;
}

void            
g_value_set_reference (GValue *value, gpointer v_reference)
{
	g_value_destroy_data(value);
	value->g_type = G_TYPE_REFERENCE;
	value->data.v_pointer = v_reference;
}
gchar*  
g_value_get_reference (GValue *value)
{
	g_return_val_if_fail(G_IS_VALUE_REFERENCE(value), NULL);

	return value->data.v_pointer;
}

void g_value_set_array(GValue *value, GPtrArray* v_pointer)
{
	g_value_destroy_data(value);
	value->g_type = G_TYPE_ARRAY;
	value->data.v_pointer = v_pointer;
}
void g_value_new_array(GValue *value)
{
	g_value_set_array(value, g_ptr_array_new());
}
GPtrArray* g_value_get_array(GValue *value)
{
	g_return_val_if_fail(G_IS_VALUE_ARRAY(value), NULL);

	return (GPtrArray*)value->data.v_pointer;
}
void g_value_set_hash_table(GValue *value, GHashTable* v_pointer)
{
	g_value_destroy_data(value);
	value->g_type = G_TYPE_HASH_TABLE;
	value->data.v_pointer = v_pointer;
}
void g_value_new_hash_table(GValue *value)
{
	g_value_set_hash_table(value, g_hash_table_new(g_str_hash, g_str_equal));
}
GHashTable* g_value_get_hash_table(GValue *value)
{
	g_return_val_if_fail(G_IS_VALUE_HASH_TABLE(value), NULL);

	return (GHashTable*)value->data.v_pointer;
}

void g_value_set_object(GValue *value, GObject* v_pointer)
{
	g_value_destroy_data(value);
	value->g_type = G_TYPE_OBJECT;
	value->data.v_pointer = v_pointer;
}
GObject* g_value_get_object(GValue *value)
{
	g_return_val_if_fail(G_IS_VALUE_OBJECT(value), NULL);

	return (GObject*)value->data.v_pointer;
}
void g_value_set_null(GValue *value)
{
	g_value_destroy_data(value);
	value->g_type = G_TYPE_NULL;	
}
GValue * g_value_get_by_path(GValue * val, gstring path)
{
	int pos, start = 0;
	gstring key;
	if (val == NULL)
		return NULL;
	g_return_val_if_fail(val != NULL, NULL);
	while(G_IS_VALUE_HASH_TABLE(val))
	{
		pos = g_strindexof(path, "/", start);
		key = g_strsubstring(path, start, pos == -1 ? -1 : pos - start);
		val = (GValue*)g_hash_table_lookup(g_value_get_hash_table(val), key);
		g_free(key);
		if (pos == -1) return val;
		if (val == NULL) break;	
		start = pos + 1;
	}
	return NULL;
}

void g_value_set_by_path(GValue * val, gstring path, GValue* pathval)
{
	int pos, start = 0;
	gstring key;
	g_return_if_fail(val != NULL);
	while(G_IS_VALUE_HASH_TABLE(val))
	{
		GHashTable * ht = g_value_get_hash_table(val);
		pos = g_strindexof(path, "/", start);
		key = g_strsubstring(path, start, pos == -1 ? -1 : pos - start);
		val = (GValue*)g_hash_table_lookup(ht, key);
		if (pos == -1) 
		{
			if (val == pathval)
			{
				g_free(key);
				return;
			}
			if (val)
				g_value_destroy(val);
			g_hash_table_insert(ht, key, pathval);
			if (val)
				g_free(key);
			return;
		}
		if (val == NULL) 
		{
			val = g_value_new();
			g_value_new_hash_table(val);
			g_hash_table_insert(ht, key, val);
		}	
		else
		{
			g_free(key);
		}
		start = pos + 1;
	}
}
void g_value_set_string_by_path(GValue * value, gstring path, gstring pathval)
{
	GValue *val = g_value_new();
	g_value_set_string(val, pathval);
	g_value_set_by_path(value, path, val);
}
void g_value_set_int_by_path(GValue * value, gstring path, gint pathval)
{
	GValue *val = g_value_new();
	g_value_set_int(val, pathval);
	g_value_set_by_path(value, path, val);
}
void g_value_set_double_by_path(GValue * value, gstring path, gdouble pathval)
{
	GValue *val = g_value_new();
	g_value_set_double(val, pathval);
	g_value_set_by_path(value, path, val);
}
GValue * g_value_get_by_index(GValue * value, gint index)
{
	GPtrArray * ar = g_value_get_array(value);
	return ar == NULL ? NULL : (GValue *)g_ptr_array_get(ar, index);
}
gint g_value_array_size(GValue * value)
{
	GPtrArray * ar = g_value_get_array(value);
	return ar == NULL ? 0 : g_ptr_array_size(ar);
}

void g_value_array_add(GValue * value, GValue * item)
{
	GPtrArray * ar = g_value_get_array(value);
	g_return_if_fail (ar != NULL);
	g_ptr_array_add(ar, item);
}