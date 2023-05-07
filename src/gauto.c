/*
 * Copyright (c) 2023 Gembera Team
 * https://github.com/gembera
 * License: MIT
 */

#include "glib.h"

typedef struct {
  gpointer data;
  GFreeCallback free_callback;
} GAutoPointer;

#define DEFAULT_AUTO_CONTAINER "DEFAULT_AUTO_CONTAINER"
static GMap *containers = NULL;

static void g_container_free_callback(GArray *pointers) {
  gint size = g_array_size(pointers);
  for (gint i = 0; i < size; i++) {
    GAutoPointer *pointer = g_array(pointers, GAutoPointer) + i;
    g_free_with(pointer->data, pointer->free_callback);
  }
  g_array_free(pointers);
}
static void g_containers_init() {
  if (containers == NULL) {
    containers = g_map_new((GFreeCallback)g_container_free_callback);
  }
}
gpointer g_auto_with(gpointer data, GFreeCallback free_callback,
                     gcstr auto_container_name) {
  g_containers_init();
  if (auto_container_name == NULL)
    auto_container_name = DEFAULT_AUTO_CONTAINER;
  GArray *pointers = (GArray *)g_map_get(containers, auto_container_name);
  if (pointers == NULL) {
    pointers = g_array_new(GAutoPointer);
    g_map_set(containers, (gpointer)auto_container_name, pointers);
  }
  GAutoPointer pointer;
  pointer.data = data;
  pointer.free_callback = free_callback;
  g_array_add(pointers, GAutoPointer, pointer);
  return data;
}
void g_auto_container_free(gcstr auto_container_name) {
  g_return_if_fail(containers);
  g_map_remove(containers, auto_container_name);
}
void g_auto_free() {
  if (containers == NULL)
    return;
  g_map_free(containers);
  containers = NULL;
}