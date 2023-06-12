/*
 * Copyright (c) 2023 Gembera Team
 * https://github.com/gembera
 * License: MIT
 */

#include "glib.h"

#ifndef HAVE_MEMMOVE
void *_memmove(void *str1, const void *str2, size_t n) {
  char *dst = (char *)str1;
  char *src = (char *)str2;
  register char *svdst;

  if ((dst > src) && (dst < src + n)) {
    src += n;
    for (svdst = dst + n; n-- > 0;)
      *--svdst = *--src;
  } else {
    for (svdst = dst; n-- > 0;)
      *svdst++ = *src++;
  }
  return dst;
}
#endif

#ifdef ENABLE_MEM_PROFILE
#define SIZE_META sizeof(gulong)
static gulong total_allocated_mem = 0;
static gulong total_freed_mem = 0;
static gulong current_mem = 0;
static gulong peak_mem = 0;

static void _g_mem_record_alloc_free(gulong allocated, gulong freed) {
  total_allocated_mem += allocated;
  total_freed_mem += freed;
  current_mem += allocated - freed;
  if (current_mem > peak_mem)
    peak_mem = current_mem;
}
#endif

gpointer _g_malloc(gulong size) {
  gpointer p;
  if (size == 0)
    return NULL;

#ifdef ENABLE_MEM_PROFILE
  gulong *t;
  size += SIZE_META;
#endif

  p = (gpointer)malloc(size);
  if (!p)
    g_error("could not allocate %ld bytes", size);

#ifdef ENABLE_MEM_PROFILE
  size -= SIZE_META;
  t = p;
  p = ((guchar *)p + SIZE_META);
  *t = size;
  _g_mem_record_alloc_free(size, 0);
#endif

  return p;
}

gpointer _g_malloc0(gulong size) {
  gpointer p;
  if (size == 0)
    return NULL;

#ifdef ENABLE_MEM_PROFILE
  gulong *t;
  size += SIZE_META;
#endif

  p = (gpointer)calloc(size, 1);
  if (!p)
    g_error("could not allocate %ld bytes", size);

#ifdef ENABLE_MEM_PROFILE
  size -= SIZE_META;
  t = p;
  p = ((guchar *)p + SIZE_META);
  *t = size;
  _g_mem_record_alloc_free(size, 0);
#endif

  return p;
}

gpointer _g_realloc(gpointer mem, gulong size) {
  gpointer p;
  if (size == 0)
    return NULL;

#ifdef ENABLE_MEM_PROFILE
  gulong *t;
  size += SIZE_META;
#endif

  if (!mem)
    p = (gpointer)malloc(size);
  else {
#ifdef ENABLE_MEM_PROFILE
    t = (gulong *)((guchar *)mem - SIZE_META);
    _g_mem_record_alloc_free(0, *t);
    mem = t;
#endif
    p = (gpointer)realloc(mem, size);
  }

  if (!p)
    g_error("could not reallocate %ld bytes", size);

#ifdef ENABLE_MEM_PROFILE
  size -= SIZE_META;
  t = p;
  p = ((guchar *)p + SIZE_META);
  *t = size;
  _g_mem_record_alloc_free(size, 0);
#endif

  return p;
}

void _g_free(gpointer mem) {
  if (mem) {
#ifdef ENABLE_MEM_PROFILE
    gulong *t;
    gulong size;
    t = (gulong *)((guchar *)mem - SIZE_META);
    size = *t;
    _g_mem_record_alloc_free(0, size);
    mem = t;
#endif
    free(mem);
  }
}
#ifdef ENABLE_MEM_RECORD
static gbool mem_record_enabled = FALSE;
static gulong mem_record_index = 0;
static GMemRecordCallback mem_record_callback = NULL;
void g_mem_record(GMemRecordCallback callback) {
  mem_record_callback = callback;
}
static gulong mem_size(gpointer mem) {
  if (mem == NULL)
    return 0;
#ifdef ENABLE_MEM_PROFILE
  gulong *t;
  gulong size;
  t = (gulong *)((guchar *)mem - SIZE_META);
  return *t;
#else
  return 0;
#endif
}
gpointer g_mem_record_malloc(gulong size, gcstr __file__, const int __line__) {
  gpointer mem = _g_malloc(size);
  if (mem_record_enabled) {
    if (mem_record_callback) {
      gulong allocated = size;
      gulong freed = 0;
      mem_record_callback(mem_record_index++, mem, NULL, allocated, freed,
                          __file__, __line__);
    }
  }
  return mem;
}
gpointer g_mem_record_malloc0(gulong size, gcstr __file__, const int __line__) {
  gpointer mem = _g_malloc0(size);
  if (mem_record_enabled) {
    if (mem_record_callback) {
      gulong allocated = size;
      gulong freed = 0;
      mem_record_callback(mem_record_index++, mem, NULL, allocated, freed,
                          __file__, __line__);
    }
  }
  return mem;
}
gpointer g_mem_record_realloc(gpointer mem, gulong size, gcstr __file__,
                              const int __line__) {
  gulong allocated = size;
  gulong freed = mem_size(mem);
  gpointer memnew = _g_realloc(mem, size);
  if (mem_record_enabled) {
    if (mem_record_callback) {
      mem_record_callback(mem_record_index++, memnew, mem, allocated, freed,
                          __file__, __line__);
    }
  }
  return memnew;
}
void g_mem_record_free(gpointer mem, gcstr __file__, const int __line__) {
  if (!mem) return;
  if (mem_record_enabled) {
    if (mem_record_callback) {
      gulong allocated = 0;
      gulong freed = mem_size(mem);
      mem_record_callback(mem_record_index++, NULL, mem, allocated, freed,
                          __file__, __line__);
    }
  }
  _g_free(mem);
}

typedef struct {
  gpointer mem;
  gulong size;
  gcstr file;
  gint line;
} GMemRecord;
gint leak_records_size = 0;
gint leak_records_alloc = 0;
GMemRecord *leak_records = NULL;
void g_mem_leak_record_free(gpointer memfree, gulong freed, gcstr __file__,
                            gint __line__) {
  if (freed == 0)
    return;
  g_return_if_fail(leak_records_size > 0 && leak_records);
  for (gint i = 0; i < leak_records_size; i++) {
    GMemRecord *ri = leak_records + i;
    if (ri->mem == memfree) {
      g_return_if_fail(ri->size == freed);
      for (gint j = i + 1; j < leak_records_size; j++) {
        leak_records[j - 1] = leak_records[j];
      }
      leak_records_size--;
      return;
    }
  }
  g_return_if_fail("memfree pointer could not be found")
}

void g_mem_leak_record_alloc(gpointer memnew, gulong allocated, gcstr __file__,
                             gint __line__) {
  if (allocated == 0)
    return;
  gint index;
  if (!leak_records) {
    leak_records_size = leak_records_alloc = 1;
    leak_records = malloc(sizeof(GMemRecord));
    index = 0;
  } else {
    for (gint i = 0; i < leak_records_size; i++) {
      GMemRecord *ri = leak_records + i;
      g_return_if_fail(ri->mem != memnew)
    }
    index = leak_records_size++;
    if (leak_records_size > leak_records_alloc) {
      leak_records_alloc = leak_records_size;
      leak_records =
          realloc(leak_records, sizeof(GMemRecord) * leak_records_alloc);
    }
  }
  leak_records[index].mem = memnew;
  leak_records[index].size = allocated;
  leak_records[index].file = __file__;
  leak_records[index].line = __line__;
}
void g_mem_print_leaks() {
  for (gint i = 0; i < leak_records_size; i++) {
    GMemRecord *ri = leak_records + i;
    printf("\n*** LEAK *** : %p\t%ld\t%s(%d)", ri->mem, ri->size, ri->file,
           ri->line);
  }
}
void g_mem_record_default_callback(gulong index, gpointer memnew,
                                   gpointer memfree, gulong allocated,
                                   gulong freed, gcstr __file__,
                                   gint __line__) {
  g_mem_leak_record_free(memfree, freed, __file__, __line__);
  g_mem_leak_record_alloc(memnew, allocated, __file__, __line__);
  printf("\r\n%ld\t%p\t%p\t+%ld\t-%ld\t%s(%d)", index, memnew, memfree, allocated,
         freed, __file__, __line__);
}

void g_mem_record_begin() { mem_record_enabled = TRUE; }
void g_mem_record_end() {
  mem_record_enabled = FALSE;
  g_mem_print_leaks();
  free(leak_records);
  leak_records = NULL;
  leak_records_size = 0;
  leak_records_alloc = 0;
}
#endif

#ifdef ENABLE_MEM_PROFILE
void g_mem_profile(gulong *pallocated, gulong *pfreed, gulong *ppeak) {
  *pallocated = total_allocated_mem;
  *pfreed = total_freed_mem;
  *ppeak = peak_mem;
}
#endif

void g_free_callback(gpointer data) { g_free(data); }