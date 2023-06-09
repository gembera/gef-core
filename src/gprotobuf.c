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
  g_return_val_if_fail(mt->fields, NULL, g_pb_message_type_free(mt));
  g_ptr_array_set_size(mt->fields, max + 1);
  for (f = fields; f->tag > 0; f++) {
    g_ptr_array_set(mt->fields, f->tag, (gpointer)f);
  }
  if (_pb_message_types == NULL) {
    _pb_message_types =
        (GMap *)g_auto_with(g_map_new((GFreeCallback)g_pb_message_type_free),
                            (GFreeCallback)g_map_free, NULL);
    g_return_val_if_fail(_pb_message_types, NULL, g_pb_message_type_free(mt))
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
static GValue *_ensure_field_repeated(GPbMessage *msg, guint32 tag) {
  GValue *varr = (GValue *)g_ptr_array_get(msg->values, tag);
  if (varr == NULL) {
    GPtrArray *arr = g_ptr_array_new_with((GFreeCallback)g_value_free);
    g_return_val_if_fail(arr, NULL);
    varr = g_value_set(g_value_new(), G_TYPE_PTR_ARRAY, arr,
                       (GFreeCallback)g_ptr_array_free);
    g_return_val_if_fail(varr, NULL, g_ptr_array_free(arr));
    g_ptr_array_set(msg->values, tag, varr);
  }
  return varr;
}

typedef GValue *(*GPbDecodeFieldHandler)(pb_istream_t *stream,
                                         GPbFieldType type);

static GValue *_decode_field_bool(pb_istream_t *stream, GPbFieldType type) {
  GValue *field_value = NULL;
  uint32_t value;
  g_return_val_if_fail(pb_decode_varint32(stream, &value), NULL);
  field_value = g_value_set_bool(g_value_new(), value != 0);
  return field_value;
}

static GValue *_decode_field_int32(pb_istream_t *stream, GPbFieldType type) {
  GValue *field_value = NULL;
  union {
    gint32 v_int;
    guint32 v_uint;
  } u;
  g_return_val_if_fail(pb_decode_varint32(stream, &u.v_uint), NULL);
  if (type == PBT_UINT32)
    field_value = g_value_set_long(g_value_new(), u.v_uint);
  else
    field_value = g_value_set_int(g_value_new(), u.v_int);
  return field_value;
}

static GValue *_decode_field_int64(pb_istream_t *stream, GPbFieldType type) {
  GValue *field_value = NULL;
  union {
    gint64 v_int;
    guint64 v_uint;
  } u;
  g_return_val_if_fail(pb_decode_varint(stream, &u.v_uint), FALSE);
  field_value = g_value_set_long(g_value_new(), u.v_int);
  return field_value;
}
static GValue *_decode_field_sint(pb_istream_t *stream, GPbFieldType type) {
  GValue *field_value = NULL;
  int64_t value;
  g_return_val_if_fail(pb_decode_svarint(stream, &value), FALSE);
  field_value = g_value_set_long(g_value_new(), value);
  return field_value;
}

static GValue *_decode_field_fiexed32(pb_istream_t *stream, GPbFieldType type) {
  GValue *field_value = NULL;
  union {
    gint32 v_int;
    gfloat v_float;
  } u;
  g_return_val_if_fail(pb_decode_fixed32(stream, &u), FALSE);
  field_value = g_value_new();
  if (type == PBT_FLOAT) {
    g_value_set_double(field_value, u.v_float);
  } else {
    g_value_set_int(field_value, u.v_int);
  }
  return field_value;
}

static GValue *_decode_field_fiexed64(pb_istream_t *stream, GPbFieldType type) {
  GValue *field_value = NULL;
  union {
    gint64 v_long;
    gdouble v_double;
  } u;
  g_return_val_if_fail(pb_decode_fixed64(stream, &u), FALSE);
  field_value = g_value_new();
  if (type == PBT_DOUBLE) {
    g_value_set_double(field_value, u.v_double);
  } else {
    g_value_set_long(field_value, u.v_long);
  }
  return field_value;
}
static gbool _decode_packed(pb_istream_t *stream, GPbFieldType type,
                            GPbMessage *msg, guint32 tag,
                            GPbDecodeFieldHandler handler) {
  pb_istream_t substream;
  g_return_val_if_fail(pb_make_string_substream(stream, &substream), FALSE);
  GValue *varr = _ensure_field_repeated(msg, tag);
  g_return_val_if_fail(varr, FALSE);
  while (substream.bytes_left > 0) {
    GValue *field_value = handler(&substream, type);
    g_return_val_if_fail(field_value, FALSE);
    g_return_val_if_fail(
        g_ptr_array_add((GPtrArray *)g_value_pointer(varr), field_value), FALSE,
        g_value_free(field_value));
  }
  g_return_val_if_fail(pb_close_string_substream(stream, &substream), FALSE);
  return TRUE;
}
static gbool _decode(GPbMessage *msg, pb_istream_t *stream) {
  pb_wire_type_t wire_type;
  guint32 tag;
  bool eof;
  guint tag_max = g_ptr_array_size(msg->type->fields) - 1;
  msg->values = g_ptr_array_new_with((GFreeCallback)g_value_free);
  g_return_val_if_fail(msg->values, FALSE);
  g_return_val_if_fail(g_ptr_array_set_size(msg->values, tag_max + 1), FALSE);

  while (pb_decode_tag(stream, &wire_type, &tag, &eof)) {
    g_return_val_if_fail(tag <= tag_max, FALSE);
    GPbField *field = g_ptr_array_get(msg->type->fields, tag);
    GValue *field_value = NULL;
    GPbFieldType type = field->type;
    switch (type) {
    case PBT_BOOL:
      if (wire_type == PB_WT_STRING) {
        g_return_val_if_fail(
            _decode_packed(stream, type, msg, tag, _decode_field_bool), FALSE);
        continue;
      } else {
        field_value = _decode_field_bool(stream, type);
      }
      break;
    case PBT_ENUM:
    case PBT_INT32:
    case PBT_UINT32:
      if (wire_type == PB_WT_STRING) {
        g_return_val_if_fail(
            _decode_packed(stream, type, msg, tag, _decode_field_int32), FALSE);
        continue;
      } else {
        field_value = _decode_field_int32(stream, type);
      }
      break;
    case PBT_INT64:
    case PBT_UINT64:
      if (wire_type == PB_WT_STRING) {
        g_return_val_if_fail(
            _decode_packed(stream, type, msg, tag, _decode_field_int64), FALSE);
        continue;
      } else {
        field_value = _decode_field_int64(stream, type);
      }
      break;
    case PBT_SINT32:
    case PBT_SINT64:
      if (wire_type == PB_WT_STRING) {
        g_return_val_if_fail(
            _decode_packed(stream, type, msg, tag, _decode_field_sint), FALSE);
        continue;
      } else {
        field_value = _decode_field_sint(stream, type);
      }
      break;
    case PBT_FLOAT:
    case PBT_FIXED32:
    case PBT_SFIXED32:
      if (wire_type == PB_WT_STRING) {
        g_return_val_if_fail(
            _decode_packed(stream, type, msg, tag, _decode_field_fiexed32),
            FALSE);
        continue;
      } else {
        field_value = _decode_field_fiexed32(stream, type);
      }
      break;
    case PBT_DOUBLE:
    case PBT_FIXED64:
    case PBT_SFIXED64:
      if (wire_type == PB_WT_STRING) {
        g_return_val_if_fail(
            _decode_packed(stream, type, msg, tag, _decode_field_fiexed64),
            FALSE);
        continue;
      } else {
        field_value = _decode_field_fiexed64(stream, type);
      }
      break;
    case PBT_BYTES: {
      g_return_val_if_fail(wire_type == PB_WT_STRING, FALSE);
      uint32_t length;
      g_return_val_if_fail(pb_decode_varint32(stream, &length), FALSE);
      GArray *bytes = g_array_new(guint8);
      g_return_val_if_fail(bytes, FALSE);
      g_return_val_if_fail(g_array_set_size(bytes, length), FALSE);
      pb_read(stream, bytes->data, (size_t)length);
      field_value = g_value_set(g_value_new(), G_TYPE_ARRAY, bytes,
                                (GFreeCallback)g_array_free);
      break;
    }
    case PBT_STRING: {
      g_return_val_if_fail(wire_type == PB_WT_STRING, FALSE);
      uint32_t length;
      g_return_val_if_fail(pb_decode_varint32(stream, &length), FALSE);
      gstr str = g_malloc(length + 1);
      g_return_val_if_fail(str, FALSE);
      pb_read(stream, (pb_byte_t *)str, (size_t)length);
      str[length] = '\0';
      field_value =
          g_value_set(g_value_new(), G_TYPE_STR, str, g_free_callback);
      g_return_val_if_fail(field_value, FALSE, g_free(str));
      break;
    }
    case PBT_MESSAGE: {
      g_return_val_if_fail(wire_type == PB_WT_STRING, FALSE);
      pb_istream_t substream;
      g_return_val_if_fail(pb_make_string_substream(stream, &substream), FALSE);
      GPbMessage *sub = g_new(GPbMessage);
      g_return_val_if_fail(sub, FALSE);
      sub->type = g_pb_message_type_get(field->sub_message_type);
      g_return_val_if_fail(_decode(sub, &substream), FALSE,
                           g_pb_message_free(sub));
      g_return_val_if_fail(pb_close_string_substream(stream, &substream), FALSE,
                           g_pb_message_free(sub));
      field_value = g_value_set(g_value_new(), G_PROTOBUF_MESSAGE, sub,
                                (GFreeCallback)g_pb_message_free);
      g_return_val_if_fail(field_value, FALSE, g_pb_message_free(sub));
      break;
    }
    }
    g_return_val_if_fail(field_value, FALSE);
    if (field->repeated) {
      GValue *varr = _ensure_field_repeated(msg, tag);
      g_return_val_if_fail(
          varr &&
              g_ptr_array_add((GPtrArray *)g_value_pointer(varr), field_value),
          FALSE, g_value_free(field_value));
    } else {
      g_ptr_array_set(msg->values, tag, field_value);
    }
  }

  for (tag = 1; tag <= tag_max; tag++) {
    GPbField *field = g_ptr_array_get(msg->type->fields, tag);
    if (field && !field->repeated) {
      GValue *value = g_ptr_array_get(msg->values, tag);
      if (!value) {
        switch (field->type) {
        case PBT_MESSAGE:
        case PBT_BYTES:
          break;
        case PBT_STRING:
          value = g_value_set(g_value_new(), G_TYPE_STR,
                              (gpointer)field->default_str, NULL);
          break;
        case PBT_DOUBLE:
        case PBT_FLOAT:
          value = g_value_set_double(g_value_new(), field->default_double);
          break;
        default:
          value = g_value_set_long(g_value_new(), field->default_int);
          break;
        }
      }
      if (value) {
        g_ptr_array_set(msg->values, tag, value);
      }
    }
  }
  return TRUE;
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
  g_return_val_if_fail(_decode(self, &stream), NULL, g_pb_message_free(self));
  return self;
}
static bool _read_stream_callback(pb_istream_t *stream, pb_byte_t *buf,
                                  size_t count) {
  GStream *source = (GStream *)stream->state;
  return g_stream_read(source, buf, count) == count;
}

GPbMessage *g_pb_message_decode_stream(gcstr type, GStream *source_stream) {
  g_return_val_if_fail(type && source_stream, NULL);
  GPbMessageType *mt = g_pb_message_type_get(type);
  g_return_val_if_fail(mt, NULL);
  GPbMessage *self = g_new(GPbMessage);
  g_return_val_if_fail(self, NULL);
  self->type = mt;
  pb_istream_t stream;
  stream.callback = &_read_stream_callback;
  stream.state = source_stream;
  stream.bytes_left = g_stream_get_length(source_stream);
  g_return_val_if_fail(_decode(self, &stream), NULL, g_pb_message_free(self));
  return self;
}
static gbool _to_json(GPbMessage *self, GValue *json) {
  g_return_val_if_fail(self && json, FALSE);
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
      g_return_val_if_fail(jarr, FALSE);
      for (vi = 0; vi < vcount; vi++) {
        GValue *item = (GValue *)g_ptr_array_get(arr, vi);
        GValue *jval = g_json_new();
        g_return_val_if_fail(jval, FALSE, g_value_free(jarr));
        if (g_value_is(item, G_PROTOBUF_MESSAGE)) {
          g_return_val_if_fail(
              _to_json((GPbMessage *)g_value_pointer(item), jval), FALSE,
              g_value_free(jarr), g_value_free(jval));
        } else {
          g_return_val_if_fail(g_value_assign(jval, item), FALSE,
                               g_value_free(jarr), g_value_free(jval));
        }
        g_return_val_if_fail(g_json_array_add(jarr, jval), FALSE,
                             g_value_free(jarr), g_value_free(jval));
      }
      g_return_val_if_fail(g_json_set(json, field->name, jarr), FALSE,
                           g_value_free(jarr));
    } else {
      GValue *jval = g_value_new();
      g_return_val_if_fail(jval, FALSE);
      if (field->type == PBT_MESSAGE) {
        g_return_val_if_fail(
            _to_json((GPbMessage *)g_value_pointer(value), jval), FALSE,
            g_value_free(jval));
      } else {
        g_return_val_if_fail(g_value_assign(jval, value), FALSE,
                             g_value_free(jval));
      }
      g_return_val_if_fail(g_json_set(json, field->name, jval), FALSE,
                           g_value_free(jval));
    }
  }
  return TRUE;
}
GValue *g_pb_message_to_json(GPbMessage *self) {
  GValue *json = g_json_new();
  g_return_val_if_fail(json, NULL);
  g_return_val_if_fail(_to_json(self, json), NULL, g_value_free(json));
  return json;
}
static GValue *_from_json_field(GPbField *field, GValue *json_val) {
  GPbFieldType type = field->type;
  GValue *field_value = NULL;
  switch (type) {
  case PBT_MESSAGE: {
    g_return_val_if_fail(field->sub_message_type, NULL);
    GPbMessage *sub = g_pb_json_to_message(field->sub_message_type, json_val);
    g_return_val_if_fail(sub, NULL);
    field_value = g_value_set(g_value_new(), G_PROTOBUF_MESSAGE, sub,
                              (GFreeCallback)g_pb_message_free);
    g_return_val_if_fail(field_value, NULL, g_pb_message_free(sub));
    break;
  }
  default: {
    field_value = g_value_new();
    g_return_val_if_fail(field_value, NULL);
    g_return_val_if_fail(g_value_assign(field_value, json_val), NULL,
                         g_value_free(field_value));
  }
  }
  return field_value;
}
static gbool _from_json(GPbMessage *self, GValue *json) {
  GPbMessageType *type = self->type;
  guint count = g_ptr_array_size(type->fields);
  g_return_val_if_fail(g_ptr_array_set_size(self->values, count), FALSE);
  guint i;
  for (i = 1; i < count; i++) {
    GPbField *field = g_ptr_array_get(type->fields, i);
    if (field == NULL)
      continue;
    GValue *value = g_json_get(json, field->name);
    if (value == NULL)
      continue;
    if (field->repeated) {
      guint vi;
      guint vcount = g_json_array_size(value);
      GPtrArray *arr = g_ptr_array_new_with((GFreeCallback)g_value_free);
      g_return_val_if_fail(arr, FALSE);
      for (vi = 0; vi < vcount; vi++) {
        GValue *item = g_json_array_get(value, vi);
        GValue *field_value = _from_json_field(field, item);
        g_return_val_if_fail(field_value, FALSE, g_ptr_array_free(arr));
        g_return_val_if_fail(g_ptr_array_add(arr, field_value), FALSE,
                             g_value_free(field_value), g_ptr_array_free(arr));
      }
      GValue *varr = g_value_set(g_value_new(), G_TYPE_PTR_ARRAY, arr,
                                 (GFreeCallback)g_ptr_array_free);
      g_return_val_if_fail(varr, FALSE, g_ptr_array_free(arr));
      g_ptr_array_set(self->values, i, varr);
    } else {
      GValue *field_value = _from_json_field(field, value);
      g_return_val_if_fail(field_value, FALSE);
      g_ptr_array_set(self->values, i, field_value);
    }
  }
  return TRUE;
}
GPbMessage *g_pb_json_to_message(gcstr type, GValue *json) {
  GPbMessage *msg = g_new(GPbMessage);
  g_return_val_if_fail(msg, NULL);
  GPbMessageType *mt = g_pb_message_type_get(type);
  g_return_val_if_fail(mt, NULL, g_pb_message_free(msg));
  msg->type = mt;
  msg->values = g_ptr_array_new_with((GFreeCallback)g_value_free);
  g_return_val_if_fail(_from_json(msg, json), NULL, g_pb_message_free(msg));
  return msg;
}
void g_pb_message_free(GPbMessage *self) {
  g_return_if_fail(self);
  g_ptr_array_free(self->values);
  g_free(self);
}

#define CHUNK_SIZE 16
static bool _write_callback(pb_ostream_t *stream, const uint8_t *buf,
                            size_t count) {
  GArray *arr = (GArray *)stream->state;
  gint left = arr->alloc - arr->len - count;
  if (left < 0) {
    g_return_val_if_fail(
        g_array_set_capacity(arr, arr->alloc + count + CHUNK_SIZE), false);
  }
  return g_array_append_items(arr, (gpointer)buf, count);
}
static gbool _write_tag(pb_ostream_t *stream, GPbField *field) {
  pb_wire_type_t wire_type;
  switch (field->type) {
  case PBT_DOUBLE:
  case PBT_FIXED64:
  case PBT_SFIXED64:
    wire_type = PB_WT_64BIT;
    break;
  case PBT_FLOAT:
  case PBT_FIXED32:
  case PBT_SFIXED32:
    wire_type = PB_WT_32BIT;
    break;
  case PBT_STRING:
  case PBT_BYTES:
  case PBT_MESSAGE:
    wire_type = PB_WT_STRING;
    break;
  default:
    wire_type = PB_WT_VARINT;
  }
  return pb_encode_tag(stream, wire_type, field->tag);
}
static gbool _encode_one(pb_ostream_t *stream, GPbField *field, GValue *value) {
  g_return_val_if_fail(_write_tag(stream, field), FALSE);
  switch (field->type) {
  case PBT_DOUBLE:
  case PBT_FIXED64:
  case PBT_SFIXED64: {
    union {
      gint64 v_long;
      gdouble v_float;
    } u;
    if (field->type == PBT_DOUBLE)
      u.v_float = g_value_double(value);
    else
      u.v_long = g_value_int(value);
    g_return_val_if_fail(pb_encode_fixed64(stream, &u), FALSE);
    break;
  }
  case PBT_FLOAT:
  case PBT_FIXED32:
  case PBT_SFIXED32: {
    union {
      gint32 v_int;
      gfloat v_float;
    } u;
    if (field->type == PBT_FLOAT)
      u.v_float = (gfloat)g_value_double(value);
    else
      u.v_int = g_value_int(value);
    g_return_val_if_fail(pb_encode_fixed32(stream, &u), FALSE);
    break;
  }
  case PBT_STRING: {
    gstr str = g_value_str(value);
    guint64 len = g_len(str);
    g_return_val_if_fail(pb_encode_varint(stream, len), FALSE);
    g_return_val_if_fail(pb_write(stream, (pb_byte_t *)str, len), FALSE);
    break;
  }
  case PBT_BYTES: {
    GArray *buf = (GArray *)g_value_pointer(value);
    guint64 len = g_array_size(buf);
    g_return_val_if_fail(pb_encode_varint(stream, len), FALSE);
    g_return_val_if_fail(pb_write(stream, buf->data, len), FALSE);
    break;
  }
  case PBT_MESSAGE: {
    GArray *arr = g_pb_message_encode((GPbMessage *)g_value_pointer(value));
    g_return_val_if_fail(arr, FALSE);
    guint64 len = g_array_size(arr);
    g_return_val_if_fail(pb_encode_varint(stream, len), FALSE,
                         g_array_free(arr));
    g_return_val_if_fail(pb_write(stream, arr->data, len), FALSE,
                         g_array_free(arr));
    g_array_free(arr);
    break;
  }
  case PBT_BOOL: {
    g_return_val_if_fail(pb_encode_varint(stream, g_value_bool(value)), FALSE);
    break;
  }
  case PBT_ENUM:
  case PBT_INT32:
  case PBT_UINT32:
  case PBT_INT64:
  case PBT_UINT64: {
    g_return_val_if_fail(pb_encode_varint(stream, g_value_long(value)), FALSE);
    break;
  }
  case PBT_SINT32:
  case PBT_SINT64: {
    g_return_val_if_fail(pb_encode_svarint(stream, g_value_long(value)), FALSE);
    break;
  }
  }
  return TRUE;
}
static gbool _encode(pb_ostream_t *stream, GPbMessage *msg) {
  GPbMessageType *mt = msg->type;
  gint count = g_ptr_array_size(mt->fields);
  gint i;
  for (i = 1; i < count; i++) {
    GPbField *field = g_ptr_array_get(mt->fields, i);
    if (!field)
      continue;
    GValue *value = g_ptr_array_get(msg->values, i);
    if (!value)
      continue;
    if (field->repeated) {
      GPtrArray *arr = (GPtrArray *)g_value_pointer(value);
      gint vcount = g_ptr_array_size(arr);
      gint vi;
      for (vi = 0; vi < vcount; vi++) {
        g_return_val_if_fail(
            _encode_one(stream, field, (GValue *)g_ptr_array_get(arr, vi)),
            FALSE);
      }
    } else {
      g_return_val_if_fail(_encode_one(stream, field, value), FALSE);
    }
  }
  return TRUE;
}

GArray *g_pb_message_encode(GPbMessage *self) {
  g_return_val_if_fail(self, NULL);
  GArray *arr = g_array_new(guint8);
  g_return_val_if_fail(arr, NULL);
  pb_ostream_t stream = {&_write_callback, (gpointer)arr, SIZE_MAX, 0};
  g_return_val_if_fail(_encode(&stream, self), NULL, g_array_free(arr));
  return arr;
}
gbool g_pb_message_encode_buffer(GPbMessage *self, GArray *buffer) {
  g_return_val_if_fail(self && buffer, FALSE);
  pb_ostream_t stream = {&_write_callback, (gpointer)buffer, SIZE_MAX, 0};
  g_return_val_if_fail(_encode(&stream, self), FALSE);
  return TRUE;
}

static bool _write_stream_callback(pb_ostream_t *stream, const uint8_t *buf,
                                   size_t count) {
  GStream *target_stream = (GStream *)stream->state;
  return count == g_stream_write(target_stream, (gcstr)buf, count);
}
gbool g_pb_message_encode_stream(GPbMessage *self, GStream *target_stream) {
  g_return_val_if_fail(self && target_stream, FALSE);
  pb_ostream_t stream = {&_write_stream_callback, (gpointer)target_stream,
                         SIZE_MAX, 0};
  g_return_val_if_fail(_encode(&stream, self), FALSE);
  return TRUE;
}