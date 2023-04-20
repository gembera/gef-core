/*
 * Copyright (c) 2023 Gembera Team
 * https://github.com/gembera
 * License: MIT
 */

#include "glib.h"

GList *g_list_new_with(GFreeCallback node_data_free_callback) {
  GList *new_list = g_new(GList);
  g_return_val_if_fail(new_list, NULL);
  new_list->head = NULL;
  new_list->node_data_free_callback = node_data_free_callback;
  return new_list;
}

GListNode *g_list_node_new(gpointer data) {
  GListNode *new_list = g_new(GListNode);
  g_return_val_if_fail(new_list, NULL);
  new_list->data = data;
  new_list->next = NULL;
  return new_list;
}

void g_list_free(GList *self) {
  g_return_if_fail(self);
  GListNode *next;
  GListNode *current = self->head;
  for (; current != NULL; current = next) {
    next = current->next;
    g_free_with(current->data, self->node_data_free_callback);
    g_free(current);
  }
  g_free(self);
}

void g_list_append(GList *self, gpointer data) {
  g_return_if_fail(self);
  GListNode *new_node;
  GListNode *last;
  new_node = g_list_node_new(data);
  if (self->head) {
    last = g_list_last(self);
    last->next = new_node;
  } else {
    self->head = new_node;
  }
}

void g_list_prepend(GList *self, gpointer data) {
  g_return_if_fail(self);
  GListNode *new_node;
  new_node = g_list_node_new(data);
  new_node->next = self->head;
  self->head = new_node;
}

GListNode *g_list_last(GList *self) {
  g_return_val_if_fail(self, NULL);
  GListNode *current = self->head;
  if (current) {
    while (current->next)
      current = current->next;
  }

  return current;
}

guint g_list_size(GList *self) {
  g_return_val_if_fail(self, 0);
  guint size = 0;
  GListNode *current = self->head;
  while (current) {
    size++;
    current = current->next;
  }
  return size;
}
void g_list_remove(GList *self, gpointer data) {
  g_return_if_fail(self);
  GListNode *tmp;
  GListNode *prev;
  prev = NULL;
  tmp = self->head;
  while (tmp) {
    if (tmp->data == data) {
      if (prev)
        prev->next = tmp->next;
      if (self->head == tmp)
        self->head = tmp->next;
      tmp->next = NULL;
      g_free(tmp);
      break;
    }
    prev = tmp;
    tmp = tmp->next;
  }
}
GListNode *g_list_get(GList *self, guint n) {
  g_return_val_if_fail(self, NULL);
  GListNode *current = self->head;
  while ((n-- > 0) && current)
    current = current->next;
  return current;
}

gint g_list_index_of(GList *self, gpointer data) {
  g_return_val_if_fail(self, -1);
  GListNode *current = self->head;
  gint i = 0;
  for (; current; i++, current = current->next) {
    if (current->data == data)
      return i;
  }
  return -1;
}

void g_list_visit(GList *self, GListVisitCallback func, gpointer user_data) {
  g_return_if_fail(self);
  g_return_if_fail(func);
  GListNode *current = self->head;
  for (; current; current = current->next) {
    func(self, current, user_data);
  }
}