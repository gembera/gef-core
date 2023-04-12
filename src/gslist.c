/*
 * Copyright (c) 2023 Gembera Team
 * https://github.com/gembera
 * License: MIT
 */

#include "glib.h"

GSList *g_slist_new() {
  GSList *new_list = g_new(GSList);
  g_return_val_if_fail(new_list, NULL);
  new_list->head = NULL;
  return new_list;
}

GSListNode *g_slist_node_new(gpointer data) {
  GSListNode *new_list = g_new(GSListNode);
  g_return_val_if_fail(new_list, NULL);
  new_list->data = data;
  new_list->next = NULL;
  return new_list;
}

void g_slist_free(GSList *self) {
  g_return_if_fail(self);
  GSListNode *next;
  GSListNode *current = self->head;
  for (; current != NULL; current = next) {
    next = current->next;
    g_free(current);
  }
  g_free(self);
}

void g_slist_append(GSList *self, gpointer data) {
  g_return_if_fail(self);
  GSListNode *new_node;
  GSListNode *last;
  new_node = g_slist_node_new(data);
  if (self->head) {
    last = g_slist_last(self);
    last->next = new_node;
  } else {
    self->head = new_node;
  }
}

void g_slist_prepend(GSList *self, gpointer data) {
  g_return_if_fail(self);
  GSListNode *new_node;
  new_node = g_slist_node_new(data);
  new_node->next = self->head;
  self->head = new_node;
}

GSListNode *g_slist_last(GSList *self) {
  g_return_val_if_fail(self, NULL);
  GSListNode *current = self->head;
  if (current) {
    while (current->next)
      current = current->next;
  }

  return current;
}

guint g_slist_size(GSList *self) {
  g_return_val_if_fail(self, 0);
  guint size = 0;
  GSListNode *current = self->head;
  while (current) {
    size++;
    current = current->next;
  }
  return size;
}
void g_slist_remove(GSList *self, gpointer data) {
  g_return_if_fail(self);
  GSListNode *tmp;
  GSListNode *prev;
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
GSListNode *g_slist_get(GSList *self, guint n) {
  g_return_val_if_fail(self, NULL);
  GSListNode *current = self->head;
  while ((n-- > 0) && current)
    current = current->next;
  return current;
}

gint g_slist_index_of(GSList *self, gpointer data) {
  g_return_val_if_fail(self, -1);
  GSListNode *current = self->head;
  gint i = 0;
  for (; current; i++, current = current->next) {
    if (current->data == data)
      return i;
  }
  return -1;
}

GSListNode *g_slist_search(GSList *self, GSListSearchHandler func,
                           gpointer user_data) {
  g_return_val_if_fail(self, NULL);
  g_return_val_if_fail(func, NULL);
  GSListNode *current = self->head;
  for (; current; current = current->next) {
    if (func(self, current, user_data))
      return current;
  }
  return NULL;
}

void g_slist_visit(GSList *self, GSListVisitCallback func, gpointer user_data) {
  g_return_if_fail(self);
  g_return_if_fail(func);
  GSListNode *current = self->head;
  for (; current; current = current->next) {
    func(self, current, user_data);
  }
}