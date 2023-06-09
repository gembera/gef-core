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

GValue *g_json_parse(gcstr s);
GValue *g_json_get(GValue *self, gcstr key);    // get value by key
GValue *g_json_index(GValue *self, gint index); // get item by index
guint g_json_size(GValue *self);                // get array size

#endif