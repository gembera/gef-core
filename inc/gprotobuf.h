/*
 * Copyright (c) 2023 Gembera Team
 * https://github.com/gembera
 * License: MIT
 */

#ifndef __G_PROTOBUF_H__
#define __G_PROTOBUF_H__
#include "gjson.h"
#include "glib.h"
#include "gstream.h"
#include "gvalue.h"

#define G_PROTOBUF_MESSAGE G_TYPE_CUSTOM

typedef enum _GPbFieldType {
  PBT_DOUBLE,
  PBT_FLOAT,
  PBT_INT32,
  PBT_INT64,
  PBT_UINT32,
  PBT_UINT64,
  PBT_SINT32,
  PBT_SINT64,
  PBT_FIXED32,
  PBT_FIXED64,
  PBT_SFIXED32,
  PBT_SFIXED64,
  PBT_BOOL,
  PBT_STRING,
  PBT_BYTES,
  PBT_MESSAGE,
  PBT_ENUM
} GPbFieldType;

typedef struct _GPbMessageType GPbMessageType;
typedef struct _GPbField {
  gint tag;
  gcstr name;
  GPbFieldType type;
  gcstr sub_message_type;
  gbool repeated;
  union {
    gint64 default_int;
    gdouble default_double;
    gcstr default_str;
  };
} GPbField;

struct _GPbMessageType {
  gcstr name;
  GPtrArray *fields;
};

typedef struct _GPbMessage {
  GPbMessageType *type;
  GPtrArray *values;
} GPbMessage;

GPbMessageType *g_pb_message_type_new(gcstr name, const GPbField *fields);
void g_pb_message_type_free(GPbMessageType *self);
GPbMessageType *g_pb_message_type_get(gcstr name);

GPbMessage *g_pb_message_decode_buffer(gcstr type, gpointer buffer, guint size);
GPbMessage *g_pb_message_decode_stream(gcstr type, GStream *stream);
GValue *g_pb_message_to_json(GPbMessage *self);
GArray *g_pb_message_encode(GPbMessage *self);
gbool g_pb_message_encode_buffer(GPbMessage *self, GArray *buffer);
gbool g_pb_message_encode_stream(GPbMessage *self, GStream *stream);
GPbMessage *g_pb_json_to_message(gcstr type, GValue *json);

void g_pb_message_free(GPbMessage *self);

#endif