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
    _auto_manager->stacks = g_array_new(GAutoStack);
    // the base stack should never be poped, it can only be destroyed with
    // g_auto_free
    GAutoStack stack;
    stack.pointers = g_array_new(GAutoPointer);
    stack.monitor_callback = NULL;
    stack.user_data = NULL;
    g_array_add(_auto_manager->stacks, GAutoStack, stack);
  }
  return _auto_manager;
}
static GAutoStack *_get_top_stack() {
  GAutoManager *manager = _get_manager();
  gint size = g_array_size(manager->stacks);
  g_return_val_if_fail(size > 0, NULL);
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
gint g_auto_current_stack() {
  GAutoManager *manager = _get_manager();
  return g_array_size(manager->stacks) - 1;
}
gpointer g_auto_with(gpointer data, GFreeCallback free_callback,
                     gint stack_index) {
  GAutoManager *manager = _get_manager();
  gint size = g_array_size(manager->stacks);
  if (stack_index < 0)
    stack_index += size;
  g_return_val_if_fail(stack_index < size, NULL);
  GAutoStack *stack = g_array(manager->stacks, GAutoStack) + stack_index;
  GAutoPointer pointer;
  pointer.data = data;
  pointer.free_callback = free_callback;
  g_array_add(stack->pointers, GAutoPointer, pointer);
  return data;
}

gint g_auto_push_with(GCallback monitor_callback, gpointer user_data) {
  GAutoManager *manager = _get_manager();
  GAutoStack stack;
  stack.pointers = g_array_new(GAutoPointer);
  stack.monitor_callback = monitor_callback;
  stack.user_data = user_data;
  g_array_add(manager->stacks, GAutoStack, stack);
  return g_array_size(manager->stacks) - 1;
}

void g_auto_pop() {
  GAutoManager *manager = _get_manager();
  gint size = g_array_size(manager->stacks);
  if (size > 1) {
    GAutoStack *stack = g_array(manager->stacks, GAutoStack) + size - 1;
    clean_stack(stack);
    g_array_set_size(manager->stacks, size - 1);
  }
}

void g_auto_pop_to(gint stack_index) {
  g_return_if_fail(stack_index > 0);
  GAutoManager *manager = _get_manager();
  gint size = g_array_size(manager->stacks);
  for (gint i = size - 1; i >= stack_index; i--) {
    GAutoStack *stack = g_array(manager->stacks, GAutoStack) + i;
    clean_stack(stack);
  }
  g_array_set_size(manager->stacks, stack_index);
}

void g_auto_pop_all() { g_auto_pop_to(1); }

void g_auto_free() {
  g_auto_pop_all();
  GAutoStack *stack = g_array(_auto_manager->stacks, GAutoStack);
  clean_stack(stack);
  g_array_free(_auto_manager->stacks);
  g_free(_auto_manager);
  _auto_manager = NULL;
}