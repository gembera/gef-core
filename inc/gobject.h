/*
 * Copyright (c) 2023 Gembera Team
 * https://github.com/gembera
 * License: MIT
 */

#ifndef __G_OBJECT_H__
#define __G_OBJECT_H__

#include "glib.h"

#ifdef __cplusplus
extern "C" {
#pragma }
#endif /* __cplusplus */

typedef struct _GObject GObject;
typedef struct _GObjectClass GObjectClass;

typedef void (*GObjectClassInitFunc)(gpointer clazz);
typedef void (*GObjectClassFinalizeFunc)(gpointer clazz);

typedef void (*GObjectInitFunc)(gpointer object);
typedef void (*GObjectFinalizeFunc)(gpointer object);

struct _GObject {
  GObjectClass *g_class;
};

// all methods in class are virtual methods, otherwise you should not add them
// in the class
struct _GObjectClass {
  gstr g_class_name;
  GObjectClassFinalizeFunc g_class_finalize;
  GObjectClass *g_base_class;

  gint instance_size;

  GObjectInitFunc init;
  GObjectFinalizeFunc finalize;
};

#define g_object_new(type) (type *)g_object_new_with(CLASS(type))
GObject *g_object_new_with(GObjectClass *clazz);
void g_object_free(gpointer self);

GObjectClass *g_register_class(const gstr classname, GObjectClass *baseclazz,
                               gint class_size, GObjectClassInitFunc class_init,
                               GObjectClassFinalizeFunc class_finalize,
                               gint instance_size, GObjectInitFunc init,
                               GObjectFinalizeFunc finalize);
GObjectClass *g_class(const gstr classname);
GObjectClass *g_class_GObject(void);
void g_object_init(gpointer self);
void g_object_finalize(gpointer self);
void g_object_class_init(gpointer clazz);
gstr g_class_name(GObjectClass *clazz);
gbool g_is_type_of(GObjectClass *thisclazz, GObjectClass *targetclazz);
gbool g_is_instance_of(gpointer self, GObjectClass *targetclazz);

#define GOBJECT(p) ((GObject *)(p))
#define CLASS(class) (GObjectClass *)g_class_##class()
#define CLASS_NAME(class) #class "Class"
#define REGISTER_CLASS(class, baseclass, init, finalize, class_init,           \
                       class_finalize)                                         \
  g_register_class(#class "Class", CLASS(baseclass), sizeof(class##Class),     \
                   class_init, class_finalize, sizeof(class), init, finalize);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
