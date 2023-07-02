
#include "gjson.h"
#include "gprotobuf.h"
#include "gtime.h"
#include <assert.h>
/*
Protobuf :
syntax = "proto2";

enum PhoneType {
  MOBILE = 0;
  HOME = 1;
  WORK = 2;
}

message Person {
  required string name = 1;
  required int32 id = 2;
  optional string email = 3;

  message PhoneNumber {
    required string number = 1;
    optional PhoneType type = 2 [ default = HOME ];
  }

  repeated PhoneNumber phone = 4;
  repeated sint32 snums = 5 [ packed = true ];
  repeated float fnums = 6;
  optional double bignum = 7;
  optional uint64 bigint = 8;
  repeated sint64 sbnums = 9 [ packed = true ];

  optional string country = 10 [ default = "China"];
}

Data:
{
    "id": 123,
    "name": "jacky",
    "email": "jacky@gmail.com",
    "phone": [
        {
            "number": "556677",
            "type": 2
        },
        {
            "number": "202308"
        }
    ],
    "snums": [
        -100,
        -50,
        0,
        50,
        100
    ],
    "fnums": [
        1.23,
        2.34,
        3.45
    ],
    "bignum": 123456789.987654321,
    "sbnums": [
        -987654321,
        -123456789,
        8888,
        0,
        123456789987654321
    ]
}
*/
void check_memory() {
  g_mem_record_end();
  gulong allocated = 0;
  gulong freed = 0;
  gulong peak = 0;
  g_mem_profile(&allocated, &freed, &peak);
  g_info("allocated memory: %ld  \tfreed memory: %ld  \tpeak memory: %ld\r\n",
         allocated, freed, peak);
  assert(allocated == freed);
}
void process(guint max_mem) {
  g_mem_profile_reset();
  g_mem_record_begin();
  g_mem_record_set_max(max_mem);
  if (max_mem)
    g_info("Max memory size : %d", max_mem);
  else
    g_info("Max memory size : Unlimited!");

  guint8 buffer[] = {
      0x0a, 0x05, 0x6a, 0x61, 0x63, 0x6b, 0x79, 0x10, 0x7b, 0x1a, 0x0f, 0x6a,
      0x61, 0x63, 0x6b, 0x79, 0x40, 0x67, 0x6d, 0x61, 0x69, 0x6c, 0x2e, 0x63,
      0x6f, 0x6d, 0x22, 0x0a, 0x0a, 0x06, 0x35, 0x35, 0x36, 0x36, 0x37, 0x37,
      0x10, 0x02, 0x22, 0x08, 0x0a, 0x06, 0x32, 0x30, 0x32, 0x33, 0x30, 0x38,
      0x2a, 0x07, 0xc7, 0x01, 0x63, 0x00, 0x64, 0xc8, 0x01, 0x35, 0xa4, 0x70,
      0x9d, 0x3f, 0x35, 0x8f, 0xc2, 0x15, 0x40, 0x35, 0xcd, 0xcc, 0x5c, 0x40,
      0x39, 0xa8, 0x5b, 0xf3, 0x57, 0x34, 0x6f, 0x9d, 0x41, 0x4a, 0x16, 0xe1,
      0xa2, 0xf3, 0xad, 0x07, 0xa9, 0xb4, 0xde, 0x75, 0xf0, 0x8a, 0x01, 0x00,
      0xe0, 0xea, 0x97, 0x85, 0xfc, 0xd2, 0xcd, 0xb6, 0x03};
  gint len = sizeof(buffer);
  GPbMessage *msg = NULL;
  GPbMessage *msg2 = NULL;
  GPbMessage *msg3 = NULL;
  gstr msg_content = NULL;
  gstr msg_content2 = NULL;
  gstr msg_content3 = NULL;
  GArray *buf = NULL;
  GValue *json = NULL;
  GValue *json2 = NULL;
  GValue *json3 = NULL;
  g_info("buffer length : %d", len);
  msg = g_pb_message_decode_buffer("Person", buffer, len);
  g_goto_if_fail(msg, clean);
  buf = g_pb_message_encode(msg);
  g_goto_if_fail(buf, clean);
  g_info("buffer length : %d", g_array_size(buf));
  msg2 = g_pb_message_decode_buffer("Person", buf->data, g_array_size(buf));
  g_goto_if_fail(msg2, clean);
  json2 = g_pb_message_to_json(msg2);
  g_goto_if_fail(json2, clean);
  msg_content2 = g_json_stringify(json2);
  g_goto_if_fail(msg_content2, clean);
  json = g_pb_message_to_json(msg);
  g_goto_if_fail(json, clean);
  msg_content = g_json_stringify(json);
  g_goto_if_fail(msg_content, clean);
  msg3 = g_pb_json_to_message("Person", json);
  g_goto_if_fail(msg3, clean);
  json3 = g_pb_message_to_json(msg3);
  g_goto_if_fail(json3, clean);
  msg_content3 = g_json_stringify(json3);
  g_goto_if_fail(msg_content3, clean);
  g_equal(msg_content, msg_content2);
  g_equal(msg_content, msg_content3);
  g_info(msg_content);
clean:
  g_free(msg_content);
  g_free(msg_content2);
  g_free(msg_content3);
  g_array_free(buf);
  g_value_free(json);
  g_value_free(json2);
  g_value_free(json3);
  g_pb_message_free(msg);
  g_pb_message_free(msg2);
  g_pb_message_free(msg3);
  check_memory();
}

int test_protobuf(int argc, char *argv[]) {
  g_log_init(INFO, NULL, NULL);
  g_mem_record(g_mem_record_default_callback);
  GPbField person_fields[] = {
      {1, "name", PBT_STRING},
      {2, "id", PBT_INT32},
      {3, "email", PBT_STRING},
      {
          4,
          "phone",
          PBT_MESSAGE,
          "PhoneNumber",
          TRUE,
      },
      {
          5,
          "snums",
          PBT_SINT32,
          NULL,
          TRUE,
      },
      {
          6,
          "fnums",
          PBT_FLOAT,
          NULL,
          TRUE,
      },
      {7, "bignum", PBT_DOUBLE},
      {8, "bigint", PBT_UINT64},
      {
          9,
          "sbnums",
          PBT_SINT64,
          NULL,
          TRUE,
      },
      {10, "country", PBT_STRING, NULL, FALSE, {.default_str = "China"}},
      0};
  g_pb_message_type_new("Person", person_fields);

  GPbField phonenumber_fields[] = {{1, "number", PBT_STRING},
                                   {2, "type", PBT_ENUM, "PhoneType", FALSE, 1},
                                   0};
  g_pb_message_type_new("PhoneNumber", phonenumber_fields);

  gbool stress_test = FALSE;
  // stress_test = TRUE;
  if (stress_test) {
    guint max;
    for (max = 0; max <= 20000; max += 100) {
      process(max);
    }
  } else {
    process(0);
  }

  g_auto_free();
  return 0;
}
