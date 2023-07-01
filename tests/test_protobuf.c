
#include "gjson.h"
#include "gprotobuf.h"
#include "gtime.h"
#include <assert.h>
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

  guint8 buffer[] = {0x0a, 0x05, 0x6a, 0x61, 0x63, 0x6b, 0x79, 0x10, 0x99, 0xf8,
                     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x01, 0x1a, 0x0f,
                     0x6a, 0x61, 0x63, 0x6b, 0x79, 0x40, 0x63, 0x61, 0x72, 0x6f,
                     0x74, 0x61, 0x2e, 0x61, 0x69, 0x22, 0x0b, 0x0a, 0x07, 0x31,
                     0x32, 0x33, 0x34, 0x38, 0x38, 0x38, 0x10, 0x02, 0x22, 0x05,
                     0x0a, 0x03, 0x35, 0x36, 0x38};
  gint len = sizeof(buffer);
  GPbMessage *msg = NULL;
  GPbMessage *msg2 = NULL;
  gstr msg_content2 = NULL;
  gstr msg_content = NULL;
  GArray *buf = NULL;
  GValue *json = NULL;
  GValue *json2 = NULL;
  g_info("buffer length : %d", len);
  msg = g_pb_message_decode_buffer("Person", buffer, len);
  g_goto_if_fail(msg, clean);
  buf = g_pb_message_encode(msg);
  g_goto_if_fail(buf, clean);
  assert(len == g_array_size(buf));
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
  g_equal(msg_content, msg_content2);
  g_info(msg_content);
clean:
  g_free(msg_content);
  g_free(msg_content2);
  g_array_free(buf);
  g_value_free(json);
  g_pb_message_free(msg);
  g_value_free(json2);
  g_pb_message_free(msg2);
  check_memory();
}

int test_protobuf(int argc, char *argv[]) {
  g_log_init(INFO, NULL, NULL);
  g_mem_record(g_mem_record_default_callback);

  GPbField fs_person[] = {{1, "name", PBT_STRING},
                          {2, "id", PBT_INT32},
                          {3, "email", PBT_STRING},
                          {4, "phone", PBT_MESSAGE, "PhoneNumber", TRUE},
                          0};
  GPbMessageType *mt_person = g_pb_message_type_new("Person", fs_person);

  GPbField fs_person_num[] = {
      {1, "number", PBT_STRING}, {2, "type", PBT_INT32}, 0};
  GPbMessageType *mt_person_num =
      g_pb_message_type_new("PhoneNumber", fs_person_num);

  assert(g_equal(mt_person->name, "Person"));
  assert(mt_person == g_pb_message_type_get("Person"));

  guint max;
  for (max = 0; max < 5000; max += 500) {
    process(max);
  }

  g_auto_free();
  return 0;
}
