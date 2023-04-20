/*
 * Copyright (c) 2023 Gembera Team
 * https://github.com/gembera
 * License: MIT
 */

#include "gcoroutine.h"

typedef struct {
  GCoroutineHandler func;
  GCoroutine *co;
} GCoroutineRunner;

GCoroutineManager *g_coroutine_manager_new() {
  GCoroutineManager *self = g_new(GCoroutineManager);
  g_return_val_if_fail(self, NULL);
  self->coroutines = g_ptr_array_new_with((GFreeCallback)g_coroutine_free);
  return self;
}
void g_coroutine_manager_loop(GCoroutineManager *self) {
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
gint g_coroutine_manager_alive_count(GCoroutineManager *self) {
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
void g_coroutine_manager_free(GCoroutineManager *self) {
  g_return_if_fail(self);
  g_ptr_array_free(self->coroutines);
  g_free(self);
}

gbool g_coroutine_is_ready(void);
void g_coroutine_initialize(void);

GCoroutine *g_coroutine_new_with(GCoroutineManager *manager,
                                 GCoroutineHandler handler, gpointer user_data,
                                 GFreeCallback user_data_free_callback) {
  g_return_val_if_fail(manager && handler, NULL);
  GCoroutine *self = g_new(GCoroutine);
  g_return_val_if_fail(self, NULL);
  self->status = COROUTINE_STATUS_ENDED;
  self->manager = manager;
  self->handler = handler;
  self->user_data = user_data;
  self->user_data_free_callback = user_data_free_callback;
  g_ptr_array_add(manager->coroutines, self);
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
