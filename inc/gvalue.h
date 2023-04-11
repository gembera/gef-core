/*
 * Copyright (c) 2023 Gembera Team
 * https://github.com/gembera
 * License: MIT
 */

#ifndef _G_VALUE_H_
#define _G_VALUE_H_

#include "glib.h"

#ifdef __cplusplus
extern "C" {
#pragma }
#endif /* __cplusplus */

typedef guint32					GType;
typedef struct _GValue			GValue;
typedef struct _GObject			GObject;
typedef struct _GObjectClass	GObjectClass;

struct _GValue
{
	/*< private >*/
	GType		g_type;

	union {
		gint	v_int;
		gfloat	v_float;
		gpointer v_pointer;
	} data;
};

typedef enum    /*< skip >*/
{
	G_TYPE_NULL,

	G_TYPE_BOOLEAN,
	G_TYPE_INT,
	G_TYPE_LONG,
	G_TYPE_DOUBLE,
	G_TYPE_STRING,
	G_TYPE_POINTER,
	G_TYPE_REFERENCE,
	G_TYPE_ARRAY,
	G_TYPE_HASH_TABLE,
	G_TYPE_HASH_TABLE_WITH_REF_KEY,
	G_TYPE_OBJECT
} GTypeFundamentals;

/* --- type macros --- */
#define G_CHECK_TYPE(value, type)		((value) != NULL && (value->g_type) == (type))
#define G_IS_VALUE_NULL(value)			(G_CHECK_TYPE ((value), G_TYPE_NULL))
#define G_IS_VALUE_BOOLEAN(value)       (G_CHECK_TYPE ((value), G_TYPE_BOOLEAN))
#define G_IS_VALUE_INT(value)           (G_CHECK_TYPE ((value), G_TYPE_INT))
#define G_IS_VALUE_LONG(value)          (G_CHECK_TYPE ((value), G_TYPE_LONG))
#define G_IS_VALUE_DOUBLE(value)        (G_CHECK_TYPE ((value), G_TYPE_DOUBLE))
#define G_IS_VALUE_STRING(value)        (G_CHECK_TYPE ((value), G_TYPE_STRING))
#define G_IS_VALUE_POINTER(value)       (G_CHECK_TYPE ((value), G_TYPE_POINTER))
#define G_IS_VALUE_REFERENCE(value)     (G_CHECK_TYPE ((value), G_TYPE_REFERENCE))

#define G_IS_VALUE_ARRAY(value)			(G_CHECK_TYPE ((value), G_TYPE_ARRAY))
#define G_IS_VALUE_HASH_TABLE(value)	(G_CHECK_TYPE ((value), G_TYPE_HASH_TABLE) || G_CHECK_TYPE ((value), G_TYPE_HASH_TABLE_WITH_REF_KEY))
#define G_IS_VALUE_OBJECT(value)		(G_CHECK_TYPE ((value), G_TYPE_OBJECT))

/* --- prototypes --- */
GValue *        g_value_new             (void);
GValue *        g_value_new_with_value  (GValue * value);
void			g_value_set_null        (GValue * value);
void            g_value_copy            (GValue * v1, GValue * v2);
void            g_value_destroy         (GValue * value);
void            g_value_destroy_data    (GValue * value);
void            g_value_set_boolean     (GValue         *value,
										 gbool        v_boolean);
gbool        g_value_get_boolean     (GValue         *value);
void            g_value_set_int         (GValue         *value,
										 gint            v_int);
gint            g_value_get_int         (GValue         *value);
void            g_value_set_long        (GValue         *value,
										 gint64          v_long);
gint64          g_value_get_long        (GValue         *value);
void            g_value_set_double      (GValue         *value,
										 gdouble         v_double);
gdouble         g_value_get_double      (GValue         *value);
void            g_value_set_string      (GValue         *value,
										 const gchar    *v_string);
gchar*          g_value_get_string      (GValue         *value);
gchar*          g_value_dup_string      (GValue         *value);
void            g_value_set_pointer     (GValue         *value,
										 gpointer       v_pointer);
gpointer        g_value_get_pointer     (GValue         *value);

void            g_value_set_reference   (GValue         *value,
										 gpointer       v_reference);
gchar*          g_value_get_reference   (GValue         *value);

void            g_value_set_array       (GValue         *value,
										 GPtrArray*     v_pointer);
void            g_value_new_array       (GValue         *value);
GPtrArray*      g_value_get_array       (GValue         *value);

void            g_value_set_hash_table	(GValue         *value,
										 GHashTable*    v_pointer);
void            g_value_new_hash_table	(GValue         *value);
GHashTable*     g_value_get_hash_table	(GValue         *value);

void            g_value_set_object     (GValue         *value,
										GObject*       v_pointer);
GObject*        g_value_get_object     (GValue         *value);

GValue *		g_value_get_by_path(GValue * value, gstring path);
GValue *		g_value_get_by_index(GValue * value, gint index);
gint			g_value_array_length(GValue * value);
void			g_value_array_add(GValue * value, GValue * item);
void			g_value_set_by_path(GValue * val, gstring path, GValue* pathval);
void			g_value_set_string_by_path(GValue * value, gstring path, gstring pathval);
void			g_value_set_int_by_path(GValue * value, gstring path, gint pathval);
void			g_value_set_double_by_path(GValue * value, gstring path, gdouble pathval);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif