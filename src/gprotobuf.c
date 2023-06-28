#include "gprotobuf.h"
#include "../libs/nanopb/pb.h"
#include "../libs/nanopb/pb_common.h"
#include "../libs/nanopb/pb_decode.h"
#include "../libs/nanopb/pb_encode.h"
#include "gjson.h"

static gcstr _pb_field_type_names[] = {
    "double", "float",  "int32",   "int64",   "uint32",   "uint64",
    "sint32", "sint64", "fixed32", "fixed64", "sfixed32", "sfixed64",
    "bool",   "string", "bytes",   "message", "enum"};

static GMap *_pb_message_types = NULL;

gcstr g_pt_field_type_name(GPbFieldType type) {
  return _pb_field_type_names[type];
}

GPbMessageType *g_pb_message_type_new(gcstr name, const GPbField *fields) {
  g_return_val_if_fail(name && fields, NULL);
  GPbMessageType *mt = g_new(GPbMessageType);
  g_return_val_if_fail(mt, NULL);
  mt->name = name;
  const GPbField *f;
  gint max = 0;
  for (f = fields; f->tag > 0; f++) {
    if (max < f->tag)
      max = f->tag;
  }
  mt->fields = g_ptr_array_new();
  g_return_val_if_fail(mt->fields, NULL, g_free(mt));
  g_ptr_array_set_size(mt->fields, max + 1);
  for (f = fields; f->tag > 0; f++) {
    g_ptr_array_set(mt->fields, f->tag, (gpointer)f);
  }
  if (_pb_message_types == NULL) {
    _pb_message_types =
        (GMap *)g_auto_with(g_map_new((GFreeCallback)g_pb_message_type_free),
                            (GFreeCallback)g_map_free, NULL);
  }
  g_map_set(_pb_message_types, name, mt);
  return mt;
}

GPbMessageType *g_pb_message_type_get(gcstr name) {
  g_return_val_if_fail(name && _pb_message_types, NULL);
  return (GPbMessageType *)g_map_get(_pb_message_types, name);
}

void g_pb_message_type_free(GPbMessageType *self) {
  g_return_if_fail(self);
  g_ptr_array_free(self->fields);
  g_free(self);
}

static bool _decode(GPbMessage *self, pb_istream_t *stream) {
  pb_wire_type_t wire_type;
  guint32 tag;
  bool eof;
  guint tag_max = g_ptr_array_size(self->type->fields) - 1;
  self->values = g_ptr_array_new_with((GFreeCallback)g_value_free);
  g_ptr_array_set_size(self->values, tag_max + 1);

  while (pb_decode_tag(stream, &wire_type, &tag, &eof)) {
    g_return_val_if_fail(tag <= tag_max, FALSE);
    GPbField *field = g_ptr_array_get(self->type->fields, tag);
    GValue *field_value = NULL;
    GPbFieldType type = field->type;
    switch (type) {
    case PBT_BOOL: {
      g_return_val_if_fail(
          wire_type == PB_WT_VARINT || wire_type == PB_WT_PACKED, false);
      uint32_t value;
      if (!pb_decode_varint32(stream, &value))
        return false;
      field_value = g_value_set_bool(g_value_new(), value != 0);
      break;
    }
    case PBT_INT32:
    case PBT_UINT32: {
      g_return_val_if_fail(
          wire_type == PB_WT_VARINT || wire_type == PB_WT_PACKED, false);
      union {
        gint32 v_int;
        guint32 v_uint;
      } u;
      if (!pb_decode_varint32(stream, &u.v_uint))
        return false;
      if (type == PBT_INT32)
        field_value = g_value_set_int(g_value_new(), u.v_int);
      else
        field_value = g_value_set_long(g_value_new(), u.v_uint);
      break;
    }
    case PBT_INT64:
    case PBT_UINT64: {
      g_return_val_if_fail(
          wire_type == PB_WT_VARINT || wire_type == PB_WT_PACKED, false);
      union {
        gint64 v_int;
        guint64 v_uint;
      } u;
      if (!pb_decode_varint(stream, &u.v_uint))
        return false;
      field_value = g_value_set_long(g_value_new(), u.v_int);
      break;
    }

    case PBT_SINT32:
    case PBT_SINT64: {
      g_return_val_if_fail(
          wire_type == PB_WT_VARINT || wire_type == PB_WT_PACKED, false);
      int64_t value;
      if (!pb_decode_svarint(stream, &value))
        return false;
      field_value = g_value_set_long(g_value_new(), value);
      break;
    }
    case PBT_FLOAT:
    case PBT_FIXED32: {
      g_return_val_if_fail(
          wire_type == PB_WT_32BIT || wire_type == PB_WT_PACKED, false);
      union {
        gint32 v_int;
        gfloat v_float;
      } u;
      if (!pb_decode_fixed32(stream, &u))
        return false;
      field_value = g_value_new();
      if (type == PBT_FLOAT) {
        g_value_set_double(field_value, u.v_float);
      } else {
        g_value_set_int(field_value, u.v_int);
      }
      break;
    }

    case PBT_DOUBLE:
    case PBT_FIXED64: {
      g_return_val_if_fail(
          wire_type == PB_WT_64BIT || wire_type == PB_WT_PACKED, false);
      union {
        gint64 v_long;
        gdouble v_double;
      } u;
      if (!pb_decode_fixed64(stream, &u))
        return false;
      field_value = g_value_new();
      if (type == PBT_DOUBLE) {
        g_value_set_double(field_value, u.v_double);
      } else {
        g_value_set_long(field_value, u.v_long);
      }
      break;
    }
    case PBT_BYTES: {
      g_return_val_if_fail(wire_type == PB_WT_STRING, false);
      uint32_t length;
      if (!pb_decode_varint32(stream, &length))
        return false;
      GArray *bytes = g_array_new(guint8);
      g_array_set_size(bytes, length);
      pb_read(stream, bytes->data, (size_t)length);
      field_value = g_value_set(g_value_new(), G_TYPE_ARRAY, bytes,
                                (GFreeCallback)g_array_free);
      break;
    }
    case PBT_STRING: {
      g_return_val_if_fail(wire_type == PB_WT_STRING, false);
      uint32_t length;
      if (!pb_decode_varint32(stream, &length))
        return false;
      gstr str = g_malloc(length + 1);
      pb_read(stream, str, (size_t)length);
      str[length] = '\0';
      field_value =
          g_value_set(g_value_new(), G_TYPE_STR, str, g_free_callback);
      break;
    }
    case PBT_MESSAGE: {
      g_return_val_if_fail(wire_type == PB_WT_STRING, false);
      pb_istream_t substream;
      if (!pb_make_string_substream(stream, &substream))
        return false;
      GPbMessage *sub = g_new(GPbMessage);
      g_return_val_if_fail(sub, false);
      sub->type = g_pb_message_type_get(field->sub_message_type);
      _decode(sub, &substream);
      field_value = g_value_set(g_value_new(), G_PROTOBUF_MESSAGE, sub,
                                (GFreeCallback)g_pb_message_free);
      if (!pb_close_string_substream(stream, &substream))
        return false;
      break;
    }
    default:
      return false;
    }
    if (field_value) {
      if (field->repeated) {
        GValue *varr = (GValue *)g_ptr_array_get(self->values, tag);
        if (varr == NULL) {
          GPtrArray *arr = g_ptr_array_new_with((GFreeCallback)g_value_free);
          g_return_val_if_fail(arr, false);
          varr = g_value_set(g_value_new(), G_TYPE_PTR_ARRAY, arr,
                             (GFreeCallback)g_ptr_array_free);
          g_return_val_if_fail(varr, false);
          g_ptr_array_set(self->values, tag, varr);
        }
        g_ptr_array_add((GPtrArray *)g_value_pointer(varr), field_value);
      } else {
        g_ptr_array_set(self->values, tag, field_value);
      }
    }
  }
  return true;
}

GPbMessage *g_pb_message_decode_buffer(gcstr type, gpointer buffer,
                                       guint size) {
  g_return_val_if_fail(type && buffer && size, NULL);
  GPbMessageType *mt = g_pb_message_type_get(type);
  g_return_val_if_fail(mt, NULL);
  GPbMessage *self = g_new(GPbMessage);
  g_return_val_if_fail(self, NULL);
  self->type = mt;
  pb_istream_t stream = pb_istream_from_buffer(buffer, size);
  _decode(self, &stream);
  return self;
}
static void _to_json(GPbMessage *self, GValue *json) {
  GPbMessageType *type = self->type;
  guint count = g_ptr_array_size(type->fields);
  guint i;
  for (i = 1; i < count; i++) {
    GPbField *field = g_ptr_array_get(type->fields, i);
    if (field == NULL)
      continue;
    GValue *value = g_ptr_array_get(self->values, i);
    if (value == NULL)
      continue;
    if (field->repeated) {
      GPtrArray *arr = (GPtrArray *)g_value_pointer(value);
      guint vi;
      guint vcount = g_ptr_array_size(arr);
      GValue *jarr = g_json_new_array();
      for (vi = 0; vi < vcount; vi++) {
        GValue *item = (GValue *)g_ptr_array_get(arr, vi);
        GValue *jval = g_json_new();
        if (g_value_is(item, G_PROTOBUF_MESSAGE)) {
          _to_json((GPbMessage *)g_value_pointer(item), jval);
        } else {
          g_value_assign(jval, item);
        }
        g_json_array_add(jarr, jval);
      }
      g_json_set(json, field->name, jarr);
    } else {
      GValue *jval = g_value_new();
      if (field->type == PBT_MESSAGE) {
        _to_json((GPbMessage *)g_value_pointer(value), jval);
      } else {
        g_value_assign(jval, value);
      }
      g_json_set(json, field->name, jval);
    }
  }
}
GValue *g_pb_message_to_json(GPbMessage *self) {
  GValue *json = g_json_new();
  _to_json(self, json);
  return json;
}

void g_pb_message_free(GPbMessage *self) {
  g_return_if_fail(self);
  g_ptr_array_free(self->values);
  g_free(self);
}