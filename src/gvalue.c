#include "gvalue.h"

GValue *g_value_new() {
  GValue *self = g_new(GValue);
  g_return_val_if_fail(self, NULL);
  return self;
}

gbool g_value_is(GValue *self, gint type) { return self->type == type; }

static void g_value_free_data(GValue *self) {
  g_return_if_fail(self);
  if (self->type >= G_TYPE_POINTER && self->free_callback) {
    self->free_callback(self->data.v_pointer);
  }
  self->type = G_TYPE_NULL;
}

GValue *g_value_set(GValue *self, gint type, gpointer pointer,
                    GFreeCallback free_callback) {
  g_return_val_if_fail(self, NULL);
  g_value_free_data(self);
  self->type = type;
  if (type < G_TYPE_POINTER) {
    switch (type) {
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
  } else {
    self->data.v_pointer = pointer;
    self->free_callback = free_callback;
  }
  return self;
}
void g_value_copy(GValue *self, GValue *dest, gbool transferOwnership) {
  g_return_if_fail(self && dest);
  g_value_free_data(dest);
  *dest = *self;
  if (transferOwnership) {
    self->free_callback = NULL;
  } else {
    dest->free_callback = NULL;
  }
}
void g_value_free(GValue *self) {
  g_return_if_fail(self);
  g_value_free_data(self);
  g_free(self);
}

gbool g_value_bool(GValue *self) {
  g_return_val_if_fail(self, FALSE);
  if (self->type < G_TYPE_POINTER) {
    switch (self->type) {
    case G_TYPE_NULL:
      return FALSE;
    case G_TYPE_BOOL:
      return self->data.v_bool == 0;
    case G_TYPE_INT:
      return self->data.v_int == 0;
    case G_TYPE_LONG:
      return self->data.v_long == 0;
    case G_TYPE_DOUBLE:
      return self->data.v_double == 0;
    }
  } else {
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
  if (self->type < G_TYPE_POINTER) {
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
    }
  } else {
    return 0;
  }
}

GValue *g_value_set_long(GValue *self, gint64 v_long) {
  return g_value_set(self, G_TYPE_LONG, &v_long, NULL);
}

gint64 g_value_long(GValue *self) {
  g_return_val_if_fail(self, 0);
  if (self->type < G_TYPE_POINTER) {
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
    }
  } else {
    return 0;
  }
}

GValue *g_value_set_double(GValue *self, gdouble v_double) {
  return g_value_set(self, G_TYPE_DOUBLE, &v_double, NULL);
}

gdouble g_value_double(GValue *self) {
  g_return_val_if_fail(self, 0);
  if (self->type < G_TYPE_POINTER) {
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
    }
  } else {
    return 0;
  }
}

gpointer g_value_pointer(GValue *self) {
  g_return_val_if_fail(self, NULL);
  return self->data.v_pointer;
}

GValue *g_value_set_null(GValue *self) {
  return g_value_set(self, G_TYPE_NULL, NULL, NULL);
}
