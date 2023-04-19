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
  gint num;
  gint sleep_time;
  GChannel *channel;
} SenderUserData;

typedef struct {
  gint num;
  gint sleep_time;
  GChannel *channel;
} ReceiverUserData;

static GCoroutineStatus sender_handler(GCoroutine *co) {
  SenderUserData *ud = (SenderUserData *)co->user_data;
  co_begin(co);
  for (ud->i = 0; ud->i < 5; ud->i++) {
    ud->num = ud->i * ud->i;
    ud->sleep_time = g_rand(100);
    co_sleep(co, ud->sleep_time);
    co_wait_until(co, g_channel_write(ud->channel, &ud->num));
    channel_case_record("sender", ud->num, ud->sleep_time);
  }
  ud->num = -1;
  co_wait_until(co, g_channel_write(ud->channel, &ud->num));
  co_end(co);
}

static GCoroutineStatus receiver_handler(GCoroutine *co) {
  ReceiverUserData *ud = (ReceiverUserData *)co->user_data;
  co_begin(co);
  do {
    ud->sleep_time = g_rand(100);
    co_sleep(co, ud->sleep_time);
    co_wait_until(co, g_channel_read(ud->channel, &ud->num));
    if (ud->num != -1)
      channel_case_record("receiver", ud->num, ud->sleep_time);
  } while (ud->num != -1);
  co_end(co);
}
static void channel_test(gint max) {
  channel_case_check = g_array_new(gint);
  GCoroutineManager *manager = g_coroutine_manager_new();
  GChannel *ch = g_channel_new(gint, max);
  SenderUserData *ud1 = g_new(SenderUserData);
  ud1->channel = ch;
  ReceiverUserData *ud2 = g_new(ReceiverUserData);
  ud2->channel = ch;
  GCoroutine *co_odd =
      g_coroutine_new_with(manager, sender_handler, ud1, g_free_callback);
  GCoroutine *co_even =
      g_coroutine_new_with(manager, receiver_handler, ud2, g_free_callback);

  g_coroutine_start(co_odd);
  g_coroutine_start(co_even);
  while (g_coroutine_manager_alive_count(manager)) {
    g_coroutine_manager_loop(manager);
    // g_sleep(10);
  }
  g_channel_free(ch);
  g_coroutine_manager_free(manager);
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
int test_channel(int, char *[]) {
  g_mem_record(g_mem_record_default_callback);
  g_mem_record_begin();

  channel_test(1);
  channel_test(2);
  channel_test(3);
  
  gulong allocated = 0;
  gulong freed = 0;
  gulong peak = 0;
  g_mem_profile(&allocated, &freed, &peak);
  printf("\nallocated memory: %d  \nfreed memory: %d\npeak memory: %d\n",
         allocated, freed, peak);
  assert(allocated == freed);
  g_mem_record_end();
  return 0;
}
