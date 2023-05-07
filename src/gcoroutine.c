/*
 * Copyright (c) 2023 Gembera Team
 * https://github.com/gembera
 * License: MIT
 */

#include "gcoroutine.h"

GCoroutineContext *g_coroutine_context_new_with(GCoroutineContext *parent) {
  GCoroutineContext *self = g_new(GCoroutineContext);
  g_return_val_if_fail(self, NULL);
  self->parent = parent;
  self->coroutines = g_ptr_array_new_with((GFreeCallback)g_coroutine_free);
  self->children = g_ptr_array_new_with((GFreeCallback)g_coroutine_context_free);
  self->shared = g_map_new(NULL);
  return self;
}
void g_coroutine_context_loop(GCoroutineContext *self) {
  g_return_if_fail(self);
  GCoroutine *co;
  gint size = g_ptr_array_size(self->coroutines);
  for (gint i = 0; i < size; i++) {
    co = (GCoroutine *)g_ptr_array_get(self->coroutines, i);
    if (g_coroutine_is_alive(co)) {
      co->status = co->handler(co);
    }
  }
}
gint g_coroutine_context_alive_count(GCoroutineContext *self) {
  g_return_val_if_fail(self, 0);
  GCoroutine *co;
  gint count = 0;
  gint size = g_ptr_array_size(self->coroutines);
  for (gint i = 0; i < size; i++) {
    co = (GCoroutine *)g_ptr_array_get(self->coroutines, i);
    if (g_coroutine_is_alive(co)) {
      count++;
    }
  }
  return count;
}
void g_coroutine_context_free(GCoroutineContext *self) {
  g_return_if_fail(self);
  g_ptr_array_free(self->coroutines);
  g_ptr_array_free(self->children);
  g_map_free(self->shared);
  g_free(self);
}

gbool g_coroutine_is_ready(void);
void g_coroutine_initialize(void);

GCoroutine *g_coroutine_new_with(GCoroutineContext *context,
                                 GCoroutineHandler handler, gpointer user_data,
                                 GFreeCallback user_data_free_callback) {
  g_return_val_if_fail(context && handler, NULL);
  GCoroutine *self = g_new(GCoroutine);
  g_return_val_if_fail(self, NULL);
  self->status = COROUTINE_STATUS_ENDED;
  self->context = context;
  self->handler = handler;
  self->user_data = user_data;
  self->user_data_free_callback = user_data_free_callback;
  g_ptr_array_add(context->coroutines, self);
  return self;
}
void g_coroutine_free(GCoroutine *self) {
  g_return_if_fail(self);
  g_coroutine_stop(self);
  g_free_with(self->user_data, self->user_data_free_callback);
  g_free(self);
}
GCoroutineSemaphore *g_coroutine_semaphore_new(guint count) {
  GCoroutineSemaphore *cs = g_new(GCoroutineSemaphore);
  g_return_val_if_fail(cs, NULL);
  cs->count = count;
  return cs;
}

void g_coroutine_stop(GCoroutine *self) {
  g_return_if_fail(self);
  if (g_coroutine_is_alive(self)) {
    self->status = COROUTINE_STATUS_EXITED;
  }
}
void g_coroutine_start(GCoroutine *self) {
  g_return_if_fail(self);
  if (!g_coroutine_is_alive(self)) {
    self->status = COROUTINE_STATUS_WAITING;
  }
}
