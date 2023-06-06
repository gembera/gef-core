#include "gchannel.h"
#include "gcoroutine.h"
#include "glib.h"
#include <assert.h>

static GArray *channel_case_check = NULL;
static void channel_case_record(gcstr label, gint num, gint sleep_time) {
  g_array_add(channel_case_check, gint, num);
  printf("\n%ld(%d) > %s : %d\n", g_tick_count(), sleep_time, label, num);
}
typedef struct {
  gint i;
  GValue num;
  gint sleep_time;
  GChannel *channel;
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
  for (ud->i = 0; ud->i < 5; ud->i++) {
    g_value_set_int(&ud->num, ud->i * ud->i);
    ud->sleep_time = g_rand(100);
    co_sleep(co, ud->sleep_time);
    co_wait_until(co, g_channel_write(ud->channel, &ud->num));
    channel_case_record("sender", g_value_int(&ud->num), ud->sleep_time);
  }
  g_channel_close(ud->channel);
  co_end(co);
}

static GCoroutineStatus receiver_handler(GCoroutine *co) {
  ReceiverUserData *ud = (ReceiverUserData *)co->user_data;
  co_begin(co);
  do {
    ud->sleep_time = g_rand(100);
    co_sleep(co, ud->sleep_time);
    co_wait_until(co, g_channel_read(ud->channel, &ud->num));
    if (g_value_is_channel_closed(&ud->num) || g_value_is_error(&ud->num))
      break;
    channel_case_record("receiver", g_value_int(&ud->num), ud->sleep_time);
  } while (TRUE);
  co_end(co);
}

typedef struct {
  gint reads;
  gint writes;
} ReadWriteCountUserData;
static void on_channel_read(GEvent *event, gpointer args, gpointer user_data) {
  ReadWriteCountUserData *counter = (ReadWriteCountUserData *)user_data;
  counter->reads++;
}
static void on_channel_write(GEvent *event, gpointer args, gpointer user_data) {
  ReadWriteCountUserData *counter = (ReadWriteCountUserData *)user_data;
  counter->writes++;
}
static void on_channel_closed(GEvent *event, gpointer args,
                              gpointer user_data) {
  gbool *closed = (gbool *)user_data;
  *closed = TRUE;
}
static void on_channel_error(GEvent *event, gpointer args, gpointer user_data) {
  gbool *error = (gbool *)user_data;
  *error = TRUE;
}
static void channel_test(gint max) {
  channel_case_check = g_array_new(gint);
  GCoroutineContext *context = g_coroutine_context_new();
  GChannel *ch = g_channel_new(max);
  SenderUserData *ud1 = g_new(SenderUserData);
  ud1->channel = ch;
  ReceiverUserData *ud2 = g_new(ReceiverUserData);
  ud2->channel = ch;
  ReadWriteCountUserData read_write_counter;
  read_write_counter.reads = 0;
  read_write_counter.writes = 0;
  gbool closed = FALSE;
  g_event_add_listener(ch->on_read, on_channel_read, &read_write_counter);
  g_event_add_listener(ch->on_write, on_channel_write, &read_write_counter);
  g_event_add_listener(ch->on_close, on_channel_closed, &closed);
  GCoroutine *co_odd = g_coroutine_new_with(
      context, sender_handler, ud1, (GFreeCallback)free_sender_user_data);
  GCoroutine *co_even = g_coroutine_new_with(
      context, receiver_handler, ud2, (GFreeCallback)free_receiver_user_data);

  g_coroutine_start(co_odd);
  g_coroutine_start(co_even);
  while (g_coroutine_context_alive_count(context)) {
    g_coroutine_context_loop(context);
    // g_sleep(10);
  }
  assert(read_write_counter.reads == 6); // 5 data + 1 close
  assert(read_write_counter.reads == read_write_counter.writes);
  assert(closed);
  g_channel_free(ch);
  g_coroutine_context_free(context);
  gint size = g_array_size(channel_case_check);
  assert(size == 10);
  if (max == 1) {
    gint *nums = g_array(channel_case_check, gint);
    for (gint i = 0; i < size; i++) {
      assert(nums[i] == (i / 2) * (i / 2));
    }
  }
  g_array_free(channel_case_check);
}

static GCoroutineStatus sender_error_handler(GCoroutine *co) {
  SenderUserData *ud = (SenderUserData *)co->user_data;
  co_begin(co);
  for (ud->i = 0; ud->i < 5; ud->i++) {
    g_value_set_int(&ud->num, ud->i * ud->i);
    ud->sleep_time = g_rand(100);
    co_sleep(co, ud->sleep_time);
    if (ud->i == 3) {
      g_channel_error(ud->channel, -1000, "I don't like number 3!");
      break;
    } else {
      co_wait_until(co, g_channel_write(ud->channel, &ud->num));
    }
    channel_case_record("sender", g_value_int(&ud->num), ud->sleep_time);
  }
  if (ud->i == 5)
    g_channel_close(ud->channel);
  co_end(co);
}
static void channel_error_test() {
  channel_case_check = g_array_new(gint);
  GCoroutineContext *context = g_coroutine_context_new();
  GChannel *ch = g_channel_new(1);
  SenderUserData *ud1 = g_new(SenderUserData);
  ud1->channel = ch;
  ReceiverUserData *ud2 = g_new(ReceiverUserData);
  ud2->channel = ch;
  ReadWriteCountUserData read_write_counter;
  read_write_counter.reads = 0;
  read_write_counter.writes = 0;
  gbool closed = FALSE;
  gbool error = FALSE;
  g_event_add_listener(ch->on_read, on_channel_read, &read_write_counter);
  g_event_add_listener(ch->on_write, on_channel_write, &read_write_counter);
  g_event_add_listener(ch->on_close, on_channel_closed, &closed);
  g_event_add_listener(ch->on_error, on_channel_error, &error);

  GCoroutine *co_odd = g_coroutine_new_with(
      context, sender_error_handler, ud1, (GFreeCallback)free_sender_user_data);
  GCoroutine *co_even = g_coroutine_new_with(
      context, receiver_handler, ud2, (GFreeCallback)free_receiver_user_data);

  g_coroutine_start(co_odd);
  g_coroutine_start(co_even);
  while (g_coroutine_context_alive_count(context)) {
    g_coroutine_context_loop(context);
    // g_sleep(10);
  }
  assert(read_write_counter.reads == 4);  // 3 data + 1 error
  assert(read_write_counter.writes == 4); // 3 data + 1 error
  assert(!closed);
  assert(error);
  g_channel_free(ch);
  g_coroutine_context_free(context);
  gint size = g_array_size(channel_case_check);
  assert(size == 6);
  gint *nums = g_array(channel_case_check, gint);
  for (gint i = 0; i < size; i++) {
    assert(nums[i] == (i / 2) * (i / 2));
  }
  g_array_free(channel_case_check);
}

static void on_channel_discard(GEvent *event, gpointer args,
                               gpointer user_data) {
  gint *count = (gint *)user_data;
  (*count)++;
  GValue *val = ( GValue *)args;
  printf("\ndiscard : %d\n", g_value_int(val));
}

static GCoroutineStatus sender_discard_handler(GCoroutine *co) {
  SenderUserData *ud = (SenderUserData *)co->user_data;
  co_begin(co);
  for (ud->i = 0; ud->i < 5; ud->i++) {
    g_value_set_int(&ud->num, ud->i * ud->i);
    co_sleep(co, 40);
    co_wait_until(co, g_channel_write(ud->channel, &ud->num));
    channel_case_record("sender", g_value_int(&ud->num), 40);
  }
  if (ud->i == 5)
    g_channel_close(ud->channel);
  co_end(co);
}

static GCoroutineStatus receiver_discard_handler(GCoroutine *co) {
  ReceiverUserData *ud = (ReceiverUserData *)co->user_data;
  co_begin(co);
  co_wait_until(co, g_channel_readable_count(ud->channel) > 0);
  g_channel_discard(ud->channel);
  do {
    co_sleep(co, 100);
    co_wait_until(co, g_channel_read(ud->channel, &ud->num));
    if (g_value_is_channel_closed(&ud->num) || g_value_is_error(&ud->num))
      break;
    channel_case_record("receiver", g_value_int(&ud->num), 100);
  } while (TRUE);
  co_end(co);
}
static void channel_discard_test() {
  channel_case_check = g_array_new(gint);
  GCoroutineContext *context = g_coroutine_context_new();
  GChannel *ch = g_channel_new(1);
  ch->auto_discard = TRUE;
  SenderUserData *ud1 = g_new(SenderUserData);
  ud1->channel = ch;
  ReceiverUserData *ud2 = g_new(ReceiverUserData);
  ud2->channel = ch;
  ReadWriteCountUserData read_write_counter;
  read_write_counter.reads = 0;
  read_write_counter.writes = 0;
  gbool closed = FALSE;
  gbool error = FALSE;
  gint discards = 0;
  g_event_add_listener(ch->on_read, on_channel_read, &read_write_counter);
  g_event_add_listener(ch->on_write, on_channel_write, &read_write_counter);
  g_event_add_listener(ch->on_close, on_channel_closed, &closed);
  g_event_add_listener(ch->on_error, on_channel_error, &error);
  g_event_add_listener(ch->on_discard, on_channel_discard, &discards);

  GCoroutine *co_odd =
      g_coroutine_new_with(context, sender_discard_handler, ud1,
                           (GFreeCallback)free_sender_user_data);
  GCoroutine *co_even =
      g_coroutine_new_with(context, receiver_discard_handler, ud2,
                           (GFreeCallback)free_receiver_user_data);

  g_coroutine_start(co_odd);
  g_coroutine_start(co_even);
  while (g_coroutine_context_alive_count(context)) {
    g_coroutine_context_loop(context);
    // g_sleep(10);
  }
  assert(discards == 3);
  assert(read_write_counter.reads == 3);  // 2 data + 1 close
  assert(read_write_counter.writes == 6); // 5 data + 1 close
  assert(closed);
  assert(!error);
  g_channel_free(ch);
  g_coroutine_context_free(context);
  gint size = g_array_size(channel_case_check);
  assert(size == 7);
  gint *nums = g_array(channel_case_check, gint);
  assert(nums[0] == 0);  // send 0
  assert(nums[1] == 1);  // send 1
  assert(nums[2] == 4);  // send 4
  assert(nums[3] == 4);  // receive 4
  assert(nums[4] == 9);  // send 9
  assert(nums[5] == 16);  // send 16
  assert(nums[6] == 16);  // receive 16

  g_array_free(channel_case_check);
}
int test_channel(int argc, char *argv[]) {
  g_mem_record(g_mem_record_default_callback);
  g_mem_record_begin();

  channel_test(1);
  channel_test(2);
  channel_test(3);

  channel_error_test();

  channel_discard_test();
  gulong allocated = 0;
  gulong freed = 0;
  gulong peak = 0;

  g_mem_profile(&allocated, &freed, &peak);
  g_mem_record_end();
  printf("\r\nallocated memory: %ld  \r\nfreed memory: %ld  \r\npeak memory: %ld\r\n",
         allocated, freed, peak);
  assert(allocated == freed);
  return 0;
}
