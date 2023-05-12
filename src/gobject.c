/*
 * Copyright (c) 2023 Gembera Team
 * https://github.com/gembera
 * License: MIT
 */

#include "gobject.h"

static GMap *name_to_class = NULL;

GObject *g_object_new_with(GObjectClass *clazz) {
  g_return_val_if_fail(clazz, NULL);
  GObject *object = g_malloc0(clazz->instance_size);
  object->g_class = clazz;
  if (clazz->init)
    clazz->init(object);
  return object;
}

void g_object_free(gpointer s) {
  GObject *self = (GObject *)s;
  g_return_if_fail(self);
  g_return_if_fail(self->g_class);
  if (self->g_class->finalize)
    self->g_class->finalize(self);
  g_free(self);
}

void g_object_init(gpointer self) {}
void g_object_finalize(gpointer self) {}
void g_object_class_init(gpointer clazz) {}

static void g_object_class_free(gpointer value) {
  GObjectClass *clazz = (GObjectClass *)value;
  if (clazz->g_class_finalize)
    clazz->g_class_finalize(clazz);
  g_free(clazz->g_class_name);
  g_free(clazz);
}

static void g_init_class_system() {
  if (name_to_class == NULL) {
    name_to_class =
        (GMap *)g_auto_with(g_map_new(g_object_class_free),
                            (GFreeCallback)g_map_free, "GClassSystem");
  }
}
GObjectClass *g_register_class(const gstr classname, GObjectClass *baseclazz,
                               gint class_size, GObjectClassInitFunc class_init,
                               GObjectClassFinalizeFunc class_finalize,
                               gint instance_size, GObjectInitFunc init,
                               GObjectFinalizeFunc finalize) {
  GObjectClass *clazz;
  g_return_val_if_fail(classname, NULL);

  clazz = g_class(classname);
  if (clazz == NULL) {
    clazz = (GObjectClass *)g_malloc0(class_size);
    clazz->g_base_class = baseclazz;
    clazz->g_class_finalize = class_finalize;
    clazz->g_class_name = g_dup(classname);
    clazz->instance_size = instance_size;
    clazz->init = init;
    clazz->finalize = finalize;
    if (class_init != NULL)
      class_init(clazz);
    g_map_set(name_to_class, clazz->g_class_name, clazz);
  }
  return clazz;
}

gbool g_is_type_of(GObjectClass *thisclazz, GObjectClass *targetclazz) {
  while (thisclazz != NULL && thisclazz != targetclazz)
    thisclazz = thisclazz->g_base_class;
  return thisclazz == targetclazz;
}
gbool g_is_instance_of(gpointer self, GObjectClass *targetclazz) {
  return g_is_type_of(GOBJECT(self)->g_class, targetclazz);
}
GObjectClass *g_class_GObject() {
  gpointer clazz = g_class(CLASS_NAME(GObject));
  return clazz != NULL
             ? clazz
             : g_register_class("GObjectClass", NULL, sizeof(GObjectClass),
                                NULL, NULL, sizeof(GObject), NULL, NULL);
}
GObjectClass *g_class(const gstr classname) {
  g_init_class_system();
  return (GObjectClass *)g_map_get(name_to_class, classname);
}
gstr g_class_name(GObjectClass *clazz) {
  return clazz == NULL ? NULL : clazz->g_class_name;
}
