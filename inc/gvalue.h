#ifndef __G_VALUE_H__
#define __G_VALUE_H__

#include "glib.h"

#ifdef __cplusplus
extern "C" {
#pragma }
#endif /* __cplusplus */

typedef struct _GValue GValue;
typedef struct _GObject GObject;
typedef struct _GObjectClass GObjectClass;

struct _GValue {
  gint type;
  gint *refs;
  union {
    gbool v_bool;
    gint32 v_int;
    gint64 v_long;
    gdouble v_double;
    gpointer v_pointer;
  } data;
  GFreeCallback free_callback;
};
#define G_TYPE_ERROR -1

#define G_TYPE_NULL 0

#define G_TYPE_BOOL 1
#define G_TYPE_INT 2
#define G_TYPE_LONG 3
#define G_TYPE_DOUBLE 4

#define G_TYPE_POINTER 100
#define G_TYPE_STR 101
#define G_TYPE_ARRAY 102
#define G_TYPE_PTR_ARRAY 103
#define G_TYPE_MAP 104
#define G_TYPE_OBJECT 105

#define G_TYPE_CUSTOM 200

GValue *g_value_new();
void g_value_free(GValue *self);

gbool g_value_is(GValue *self, gint type);
gbool g_value_is_error(GValue *self);

GValue *g_value_set_null(GValue *self);
gbool g_value_bool(GValue *self);
GValue *g_value_set_bool(GValue *self, gbool v_bool);
gint32 g_value_int(GValue *self);
GValue *g_value_set_int(GValue *self, gint32 v_int);
gint64 g_value_long(GValue *self);
GValue *g_value_set_long(GValue *self, gint64 v_long);
gdouble g_value_double(GValue *self);
GValue *g_value_set_double(GValue *self, gdouble v_double);
gstr g_value_str(GValue *self);
GValue *g_value_set_str(GValue *self, gcstr str);
gpointer g_value_pointer(GValue *self);
GValue *g_value_set(GValue *self, gint type, gpointer pointer,
                    GFreeCallback free_callback);

GValue *g_value_ref(GValue *self, GValue *target);
void g_value_unref(GValue *self);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif