#include "gtime.h"

#ifdef _WIN32
int gettimeofday(struct timeval *tv, void *not_used);
#else
#include <sys/time.h>
#include <unistd.h>
#endif

GDate g_date_now() {
  struct timeval t;
  gettimeofday(&t, NULL);
  struct tm tm = *localtime(&t.tv_sec);
  GDate dt = {0};
  dt.year = tm.tm_year + 1900;
  dt.month = tm.tm_mon;
  dt.day = tm.tm_mday;
  dt.hours = tm.tm_hour;
  dt.minutes = tm.tm_min;
  dt.seconds = tm.tm_sec;
  dt.milliseconds = t.tv_usec / 1000;
  dt.timezone_offset = tm.tm_gmtoff / 60;
  return dt;
}

guint64 g_tick() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (uint64_t)(tv.tv_sec) * 1000 + (uint64_t)(tv.tv_usec) / 1000;
}

guint64 g_utick() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (uint64_t)(tv.tv_sec) * 1000000 + (uint64_t)(tv.tv_usec);
}

void g_sleep(guint64 time_ms) { usleep(time_ms * 1000); }
void g_usleep(guint64 time_us) { usleep(time_us); }

#ifdef _WIN32
#include <windows.h>
#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
#define DELTA_EPOCH_IN_MICROSECS 116444736000000000Ui64
#else
#define DELTA_EPOCH_IN_MICROSECS 116444736000000000ULL
#endif
int gettimeofday(struct timeval *tv, void *not_used) {
  FILETIME ft;
  unsigned __int64 tmpres = 0;

  if (NULL != tv) {
    GetSystemTimeAsFileTime(&ft);

    tmpres |= ft.dwHighDateTime;
    tmpres <<= 32;
    tmpres |= ft.dwLowDateTime;

    tmpres -= DELTA_EPOCH_IN_MICROSECS;
    tmpres /= 10;

    tv->tv_sec = (long)(tmpres / 1000000UL);
    tv->tv_usec = (long)(tmpres % 1000000UL);
  }
  return 0;
}
#endif