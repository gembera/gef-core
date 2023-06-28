
#include "gjson.h"
#include "gprotobuf.h"
#include "gtime.h"
#include <assert.h>

int test_protobuf(int argc, char *argv[]) {
  g_log_init(INFO, NULL, NULL);
  g_mem_record(g_mem_record_default_callback);
  g_mem_record_begin();

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

  guint8 buffer[] = {0x0a, 0x05, 0x6a, 0x61, 0x63, 0x6b, 0x79, 0x10, 0x99, 0xf8,
                     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x01, 0x1a, 0x0f,
                     0x6a, 0x61, 0x63, 0x6b, 0x79, 0x40, 0x63, 0x61, 0x72, 0x6f,
                     0x74, 0x61, 0x2e, 0x61, 0x69, 0x22, 0x0b, 0x0a, 0x07, 0x31,
                     0x32, 0x33, 0x34, 0x38, 0x38, 0x38, 0x10, 0x02, 0x22, 0x05,
                     0x0a, 0x03, 0x35, 0x36, 0x38};
  gint len = sizeof(buffer);
  g_info("len : %d", len);
  GPbMessage *msg = g_pb_message_decode_buffer("Person", buffer, len);
  GValue *json = g_pb_message_to_json(msg);
  gstr msg_content = g_json_stringify(json);
  g_info(msg_content);
  g_free(msg_content);
  g_value_free(json);
  g_pb_message_free(msg);
  g_auto_free();
  gulong allocated = 0;
  gulong freed = 0;
  gulong peak = 0;
  g_mem_profile(&allocated, &freed, &peak);
  g_mem_record_end();
  g_info("allocated memory: %ld  \tfreed memory: %ld  \tpeak memory: %ld\r\n",
         allocated, freed, peak);
  assert(allocated == freed);
  return 0;
}
