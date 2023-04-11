/*
 * Copyright (c) 2023 Gembera Team
 * https://github.com/gembera
 * License: MIT
 */

#include "glib.h"

#ifndef HAVE_MEMMOVE
char *_memmove(char *dst, register char *src, register int n) {
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
#define SIZE_META SIZEOF_LONG
static gulong allocated_mem = 0;
static gulong freed_mem = 0;
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
  allocated_mem += size;
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
  allocated_mem += size;
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
    freed_mem += *t;
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
  allocated_mem += size;
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
    freed_mem += size;
    mem = t;
#endif
    free(mem);
  }
}
#ifdef ENABLE_MEM_RECORD
static gbool mem_record_enabled = FALSE;
static gchar mem_record_log[1024];
static gulong mem_record_index = 0;
static GMemRecordCallback mem_record_callback = NULL;
void g_mem_record(GMemRecordCallback callback) {
  mem_record_callback = callback;
}
void g_mem_record_begin() { mem_record_enabled = TRUE; }
void g_mem_record_end() { mem_record_enabled = FALSE; }
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
gpointer g_mem_record_malloc(gulong size, const char *__file__,
                             const int __line__) {
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
gpointer g_mem_record_malloc0(gulong size, const char *__file__,
                              const int __line__) {
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
gpointer g_mem_record_realloc(gpointer mem, gulong size, const char *__file__,
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
void g_mem_record_free(gpointer mem, const char *__file__, const int __line__) {
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

void g_mem_record_default_callback(gulong index, gpointer memnew,
                                 gpointer memfree, gulong allocated,
                                 gulong freed, const char *__file__,
                                 const int __line__) {
  printf("\n%ld\t%lx\t%lx\t%ld\t%ld\t%s(%d)", index, memnew, memfree, allocated,
         freed, __file__, __line__);
}
#endif

#ifdef ENABLE_MEM_PROFILE
void g_mem_profile(gulong *pallocated, gulong *pfreed) {
  *pallocated = allocated_mem;
  *pfreed = freed_mem;
}
#endif
