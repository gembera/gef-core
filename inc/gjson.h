/*
 * Copyright (c) 2023 Gembera Team
 * https://github.com/gembera
 * License: MIT
 */

#ifndef __G_JSON_H__
#define __G_JSON_H__
#include "gvalue.h"

#define G_JSON_NULL G_TYPE_NULL
#define G_JSON_NUMBER G_TYPE_DOUBLE
#define G_JSON_STRING G_TYPE_STR
#define G_JSON_ARRAY G_TYPE_PTR_ARRAY
#define G_JSON_OBJECT G_TYPE_MAP

GValue *g_json_new();
GValue *g_json_new_array();
GValue *g_json_parse(gcstr s);
gstr g_json_stringify(GValue *self);

// G_JSON_OBJECT
gbool g_json_has(GValue *self, gcstr key);
GValue *g_json_get(GValue *self, gcstr key);
void g_json_set(GValue *self, gcstr key, GValue *val);
void g_json_set_int(GValue *self, gcstr key, gint val);
void g_json_set_double(GValue *self, gcstr key, gdouble val);
void g_json_set_str(GValue *self, gcstr key, gstr val);
GMapEntry *g_json_get_entry(GValue *self, gcstr key);
void g_json_remove(GValue *self, gcstr key);
void g_json_remove_all(GValue *self);
guint g_json_size(GValue *self);
void g_json_visit(GValue *self, GMapVisitCallback func, gpointer user_data);

// G_JSON_ARRAY
GValue *g_json_array_get(GValue *self, guint index);
void g_json_array_set(GValue *self, guint index, GValue *item);
guint g_json_array_size(GValue *self);
void g_json_array_add(GValue *self, GValue *item);
GValue *g_json_array_add_int(GValue *self, gint item);
GValue *g_json_array_add_double(GValue *self, gdouble item);
GValue *g_json_array_add_str(GValue *self, gstr item);
void g_json_array_insert(GValue *self, guint index, GValue *item);
GValue *g_json_array_insert_int(GValue *self, guint index, gint item);
GValue *g_json_array_insert_double(GValue *self, guint index, gdouble item);
GValue *g_json_array_insert_str(GValue *self, guint index, gstr item);
gint g_json_array_index_of(GValue *self, GValue *item);
void g_json_array_remove(GValue *self, guint index);
void g_json_array_visit(GValue *self, GPtrArrayVisitCallback func,
                        gpointer user_data);

#endif