#include "gvalue.h"

GValue *g_value_new() {
  GValue *self = g_new(GValue);
  g_return_val_if_fail(self, NULL);
  return self;
}

gbool g_value_is(GValue *self, gint type) { return self->type == type; }

gbool g_value_is_error(GValue *self) { return self->type < 0; }

static void g_value_free_data(GValue *self) {
  g_return_if_fail(self);
  g_free(self->refs);
  g_free_with(self->data.v_pointer, self->free_callback);
  self->type = G_TYPE_NULL;
}

GValue *g_value_set(GValue *self, gint type, gpointer pointer,
                    GFreeCallback free_callback) {
  g_return_val_if_fail(self, NULL);
  g_value_free_data(self);
  self->type = type;
  self->refs = NULL;
  self->data.v_pointer = pointer;
  self->free_callback = free_callback;
  if (free_callback) {
    self->refs = g_new(gint);
    *self->refs = 1;
  }
  switch (type) {
  case G_TYPE_NULL:
    break;
  case G_TYPE_BOOL:
    self->data.v_bool = *(gbool *)pointer;
    break;
  case G_TYPE_INT:
    self->data.v_int = *(gint32 *)pointer;
    break;
  case G_TYPE_LONG:
    self->data.v_long = *(gint64 *)pointer;
    break;
  case G_TYPE_DOUBLE:
    self->data.v_double = *(gdouble *)pointer;
    break;
  }
  return self;
}
GValue *g_value_ref(GValue *self, GValue *target) {
  g_return_val_if_fail(self && target, self);
  g_value_unref(self);
  *self = *target;
  if (self->refs) {
    (*self->refs)++;
  }
  return self;
}
void g_value_unref(GValue *self) {
  g_return_if_fail(self);
  if (self->refs) {
    g_return_if_fail(*self->refs > 0);
    (*self->refs)--;
    if (*self->refs == 0)
      g_value_free_data(self);
  } else {
    g_value_free_data(self);
  }
}
void g_value_free(GValue *self) {
  g_return_if_fail(self);
  g_value_unref(self);
  g_free(self);
}

gbool g_value_bool(GValue *self) {
  g_return_val_if_fail(self, FALSE);
  switch (self->type) {
  case G_TYPE_NULL:
    return FALSE;
  case G_TYPE_BOOL:
    return self->data.v_bool;
  case G_TYPE_INT:
    return self->data.v_int != 0;
  case G_TYPE_LONG:
    return self->data.v_long != 0;
  case G_TYPE_DOUBLE:
    return self->data.v_double != 0;
  default:
    return TRUE;
  }
}

GValue *g_value_set_bool(GValue *self, gbool v_bool) {
  return g_value_set(self, G_TYPE_BOOL, &v_bool, NULL);
}

GValue *g_value_set_int(GValue *self, gint v_int) {
  return g_value_set(self, G_TYPE_INT, &v_int, NULL);
}

gint32 g_value_int(GValue *self) {
  g_return_val_if_fail(self, 0);
  switch (self->type) {
  case G_TYPE_NULL:
    return 0;
  case G_TYPE_BOOL:
    return self->data.v_bool ? 1 : 0;
  case G_TYPE_INT:
    return self->data.v_int;
  case G_TYPE_LONG:
    return (gint32)self->data.v_long;
  case G_TYPE_DOUBLE:
    return (gint32)self->data.v_double;
  default:
    return 0;
  }
}

GValue *g_value_set_long(GValue *self, gint64 v_long) {
  return g_value_set(self, G_TYPE_LONG, &v_long, NULL);
}

gint64 g_value_long(GValue *self) {
  g_return_val_if_fail(self, 0);
  switch (self->type) {
  case G_TYPE_NULL:
    return 0;
  case G_TYPE_BOOL:
    return self->data.v_bool ? 1 : 0;
  case G_TYPE_INT:
    return self->data.v_int;
  case G_TYPE_LONG:
    return self->data.v_long;
  case G_TYPE_DOUBLE:
    return (gint64)self->data.v_double;
  default:
    return 0;
  }
}

GValue *g_value_set_double(GValue *self, gdouble v_double) {
  return g_value_set(self, G_TYPE_DOUBLE, &v_double, NULL);
}

gdouble g_value_double(GValue *self) {
  g_return_val_if_fail(self, 0);
  switch (self->type) {
  case G_TYPE_NULL:
    return 0;
  case G_TYPE_BOOL:
    return self->data.v_bool ? 1 : 0;
  case G_TYPE_INT:
    return self->data.v_int;
  case G_TYPE_LONG:
    return self->data.v_long;
  case G_TYPE_DOUBLE:
    return self->data.v_double;
  default:
    return 0;
  }
}

gstr g_value_str(GValue *self) {
  g_return_val_if_fail(g_value_is(self, G_TYPE_STR), NULL);
  return (gstr)g_value_pointer(self);
}
GValue *g_value_set_str(GValue *self, gcstr str) {
  g_return_val_if_fail(self, NULL);
  g_value_set(self, G_TYPE_STR, (gpointer)g_dup(str), g_free_callback);
  return self;
}

gpointer g_value_pointer(GValue *self) {
  g_return_val_if_fail(self, NULL);
  return self->data.v_pointer;
}

GValue *g_value_set_null(GValue *self) {
  g_return_val_if_fail(self, NULL);
  return g_value_set(self, G_TYPE_NULL, NULL, NULL);
}
