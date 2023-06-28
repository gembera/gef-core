#include "gchannel.h"
#include "gcoroutine.h"
#include "glib.h"
#include "gtime.h"
#include <assert.h>

typedef struct {
  gint i;
  GValue num;
  gint sleep_time;
  GChannel *channel;
  gint data_count;
  gint data_error_index;
} SenderUserData;

typedef struct {
  GValue num;
  gint sleep_time;
  GChannel *channel;
} ReceiverUserData;

static void free_sender_user_data(SenderUserData *user_data) {
  g_value_unref(&user_data->num);
  g_free(user_data);
}
static void free_receiver_user_data(ReceiverUserData *user_data) {
  g_value_unref(&user_data->num);
  g_free(user_data);
}
static GCoroutineStatus sender_handler(GCoroutine *co) {
  SenderUserData *ud = (SenderUserData *)co->user_data;
  co_begin(co);
  for (ud->i = 0; ud->i < ud->data_count; ud->i++) {
    g_value_set_int(&ud->num, ud->i * ud->i);
    ud->sleep_time = rand() % 100;
    co_sleep(co, ud->sleep_time);
    if (ud->i == ud->data_error_index) {
      g_channel_error(ud->channel, -1000, "This is a serious error!");
      break;
    } else {
      co_wait_until(co, g_channel_write(ud->channel, &ud->num));
    }
  }
  g_channel_close(ud->channel);
  co_end(co);
}

static GCoroutineStatus receiver_handler(GCoroutine *co) {
  ReceiverUserData *ud = (ReceiverUserData *)co->user_data;
  co_begin(co);
  do {
    ud->sleep_time = rand() % 100;
    co_sleep(co, ud->sleep_time);
    co_wait_until(co, g_channel_read(ud->channel, &ud->num));
    if (!g_value_is_channel_data(&ud->num))
      break;
  } while (TRUE);
  co_end(co);
}

typedef struct {
  GArray *reads;
  GArray *writes;
  gbool closed;
  gbool error;
} ReadWriteCountUserData;

static void on_channel_read(GEvent *event, gpointer args, gpointer user_data) {
  GChannel *ch = (GChannel *)event->sender;
  GValue *item = (GValue *)args;
  ReadWriteCountUserData *counter = (ReadWriteCountUserData *)user_data;
  if (g_value_is_channel_closed(item)) {
    counter->closed = TRUE;
  } else if (g_value_is_error(item)) {
    counter->error = TRUE;
  } else {
    g_array_add(counter->reads, gint, g_value_int(item));
  }
}
static void on_channel_write(GEvent *event, gpointer args, gpointer user_data) {
  GChannel *ch = (GChannel *)event->sender;
  GValue *item = (GValue *)args;
  ReadWriteCountUserData *counter = (ReadWriteCountUserData *)user_data;
  if (g_value_is_channel_data(item)) {
    g_array_add(counter->writes, gint, g_value_int(item));
  }
}
static void channel_test(gint channel_buffer_len, gint data_count,
                         gint data_error_index) {
  GCoroutineContext *context = g_coroutine_context_new();
  GChannel *ch = g_channel_new(channel_buffer_len);
  SenderUserData *ud1 = g_new(SenderUserData);
  GValue *val = g_value_set_str(g_value_new(), "Value 123");
  g_value_assign(&ud1->num, val);
  ud1->channel = ch;
  ud1->data_count = data_count;
  ud1->data_error_index = data_error_index;
  ReceiverUserData *ud2 = g_new(ReceiverUserData);
  ud2->channel = ch;
  g_value_assign(&ud2->num, val);
  ReadWriteCountUserData read_write_counter;
  read_write_counter.reads = g_array_new(gint);
  g_array_set_capacity(read_write_counter.reads, data_count);
  read_write_counter.writes = g_array_new(gint);
  g_array_set_capacity(read_write_counter.writes, data_count);
  g_event_add_listener(ch->on_read, on_channel_read, &read_write_counter);
  g_event_add_listener(ch->on_write, on_channel_write, &read_write_counter);
  GCoroutine *co_odd = g_coroutine_new_with(
      context, sender_handler, ud1, (GFreeCallback)free_sender_user_data);
  GCoroutine *co_even = g_coroutine_new_with(
      context, receiver_handler, ud2, (GFreeCallback)free_receiver_user_data);
  g_value_free(val);
  g_coroutine_start(co_odd);
  g_coroutine_start(co_even);
  while (g_coroutine_context_alive_count(context)) {
    g_coroutine_context_loop(context);
    // g_sleep(10);
  }
  if (data_error_index >= 0 && data_error_index < data_count)
    assert(g_array_size(read_write_counter.reads) == data_error_index);
  else
    assert(g_array_size(read_write_counter.reads) == data_count);
  assert(g_array_size(read_write_counter.reads) ==
         g_array_size(read_write_counter.writes));
  assert(read_write_counter.closed);
  g_channel_free(ch);
  g_coroutine_context_free(context);
  gint size = g_array_size(read_write_counter.reads);
  for (gint i = 0; i < size; i++) {
    assert(g_array_get(read_write_counter.reads, gint, i) == i * i);
    assert(g_array_get(read_write_counter.writes, gint, i) == i * i);
  }
  g_array_free(read_write_counter.reads);
  g_array_free(read_write_counter.writes);
}
int test_channel(int argc, char *argv[]) {
  g_mem_record(g_mem_record_default_callback);
  g_mem_record_begin();

  channel_test(1, 10, -1);
  /*
  channel_test(2, 10, -1);
  channel_test(3, 10, -1);
  channel_test(100, 10, 8);
  */

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
