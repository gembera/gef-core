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

typedef struct {
  GArray *pointers;
  GCallback monitor_callback;
  gpointer user_data;
} GAutoStack;

typedef struct {
  GArray *stacks;
} GAutoManager;

static GAutoManager *_auto_manager = NULL;
static GAutoManager *_get_manager() {
  if (_auto_manager == NULL) {
    _auto_manager = g_new(GAutoManager);
    _auto_manager->stacks = g_array_new_of(GAutoStack);
  }
  return _auto_manager;
}
static GAutoStack *_get_top_stack() {
  GAutoManager *manager = _get_manager();
  gint size = g_array_size(manager->stacks);
  if (size == 0) {
    size = 1;
    GAutoStack stack;
    stack.pointers = g_array_new_of(GAutoPointer);
    stack.monitor_callback = NULL;
    stack.user_data = NULL;
    g_array_add(manager->stacks, GAutoStack, stack);
  }
  return g_array(manager->stacks, GAutoStack) + size - 1;
}
static void clean_stack(GAutoStack *stack) {
  gint size = g_array_size(stack->pointers);
  for (gint i = 0; i < size; i++) {
    GAutoPointer *pointer = g_array(stack->pointers, GAutoPointer) + i;
    if (pointer->free_callback) {
      pointer->free_callback(pointer->data);
      if (stack->monitor_callback)
        stack->monitor_callback(pointer->data, stack->user_data);
    }
  }
  g_array_free(stack->pointers);
}

gpointer g_auto_with(gpointer data, GFreeCallback free_callback) {
  GAutoStack *stack = _get_top_stack();
  GAutoPointer pointer;
  pointer.data = data;
  pointer.free_callback = free_callback;
  g_array_add(stack->pointers, GAutoPointer, pointer);
  return data;
}

gint g_auto_push_with(GCallback monitor_callback, gpointer user_data) {
  GAutoManager *manager = _get_manager();
  GAutoStack stack;
  stack.pointers = g_array_new_of(GAutoPointer);
  stack.monitor_callback = monitor_callback;
  stack.user_data = user_data;
  g_array_add(manager->stacks, GAutoStack, stack);
  return g_array_size(manager->stacks) - 1;
}

void g_auto_pop() {
  GAutoManager *manager = _get_manager();
  gint size = g_array_size(manager->stacks);
  if (size > 0) {
    GAutoStack *stack = g_array(manager->stacks, GAutoStack) + size - 1;
    clean_stack(stack);
    g_array_set_size(manager->stacks, size - 1);
  }
}

void g_auto_pop_to(gint mark) {
  g_return_if_fail(mark >= 0);
  GAutoManager *manager = _get_manager();
  gint size = g_array_size(manager->stacks);
  for (gint i = size - 1; i >= mark; i--) {
    GAutoStack *stack = g_array(manager->stacks, GAutoStack) + i;
    clean_stack(stack);
  }
  g_array_set_size(manager->stacks, mark);
}

void g_auto_pop_all() { g_auto_pop_to(0); }

void g_auto_free() {
  g_auto_pop_all();
  g_array_free(_auto_manager->stacks);
  g_free(_auto_manager);
  _auto_manager = NULL;
}