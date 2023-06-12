/*
 * Copyright (c) 2023 Gembera Team
 * https://github.com/gembera
 * License: MIT
 */

#ifndef __G_LIB_H__
#define __G_LIB_H__

#include "glibconfig.h"
#include <stdint.h> 
#ifndef NULL
#define NULL ((void *)0)
#endif

#ifndef FALSE
#define FALSE (0)
#endif

#ifndef TRUE
#define TRUE (!FALSE)
#endif

#undef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

#undef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

#undef ABS
#define ABS(a) (((a) < 0) ? -(a) : (a))

#undef CLAMP
#define CLAMP(x, low, high)                                                    \
  (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))

#undef MAXUINT
#define MAXUINT ((guint) ~((guint)0))

#undef MAXINT
#define MAXINT ((gint)(MAXUINT >> 1))

#undef MININT
#define MININT ((gint)~MAXINT)

#define g_return_if_fail(expr, ...)                                            \
  if (!(expr)) {                                                               \
    g_warning("\r\nfile %s: line %d: assertion \"%s\" failed.\n", __FILE__,    \
              __LINE__, #expr);                                                \
    __VA_ARGS__;                                                               \
    return;                                                                    \
  };

#define g_return_val_if_fail(expr, val, ...)                                   \
  if (!(expr)) {                                                               \
    g_warning("\r\nfile %s: line %d: assertion \"%s\" failed.\n", __FILE__,    \
              __LINE__, #expr);                                                \
    __VA_ARGS__;                                                               \
    return val;                                                                \
  };

#ifdef __cplusplus
extern "C" {
#pragma }
#endif /* __cplusplus */

// Provide type definitions for commonly used types.
typedef char gchar;
typedef short gshort;
typedef long glong;
typedef int gint;
typedef gint gbool;

typedef unsigned char guchar;
typedef unsigned short gushort;
typedef unsigned long gulong;
typedef unsigned int guint;

typedef float gfloat;
typedef double gdouble;

typedef void *gpointer;
typedef const void *gcpointer;

typedef gpointer ghandle;

typedef int8_t gint8;
typedef uint8_t guint8;

typedef int16_t gint16;
typedef uint16_t guint16;

typedef int32_t gint32;
typedef uint32_t guint32;

typedef int64_t gint64;
typedef uint64_t guint64;

typedef guint16 gwchar;
typedef gchar *gstr;
typedef gwchar *gwstr;
typedef const gchar *gcstr;
typedef const gwchar *gcwstr;

// Memory management

#ifdef HAVE_MEMMOVE
#define g_memmove memmove
#else
void *_memmove(void *dst, const void *src, size_t n);
#define g_memmove _memmove
#endif

#define g_new(type) ((type *)g_malloc0((unsigned)sizeof(type)))
#define g_new_many(type, count)                                                \
  ((type *)g_malloc0((unsigned)sizeof(type) * (count)))

#ifdef ENABLE_MEM_RECORD
typedef void (*GMemRecordCallback)(gulong index, gpointer memnew,
                                   gpointer memfree, gulong allocated,
                                   gulong freed, gcstr __file__,
                                   const int __line__);
void g_mem_record_default_callback(gulong index, gpointer memnew,
                                   gpointer memfree, gulong allocated,
                                   gulong freed, gcstr __file__, gint __line__);
void g_mem_record(GMemRecordCallback callback);
void g_mem_record_begin();
void g_mem_record_end();

gpointer g_mem_record_malloc(gulong size, gcstr __file__, const int __line__);
gpointer g_mem_record_malloc0(gulong size, gcstr __file__, const int __line__);
gpointer g_mem_record_realloc(gpointer mem, gulong size, gcstr __file__,
                              const int __line__);
void g_mem_record_free(gpointer mem, gcstr __file__, const int __line__);

gpointer _g_malloc(gulong size);
gpointer _g_malloc0(gulong size);
gpointer _g_realloc(gpointer mem, gulong size);
void _g_free(gpointer mem);

#define g_malloc(size) g_mem_record_malloc(size, __FILE__, __LINE__)
#define g_malloc0(size) g_mem_record_malloc0(size, __FILE__, __LINE__)
#define g_realloc(mem, size) g_mem_record_realloc(mem, size, __FILE__, __LINE__)
#define g_free(mem) g_mem_record_free(mem, __FILE__, __LINE__)
#else
#define g_mem_record(callback)
#define g_mem_record_begin()
#define g_mem_record_end()

gpointer _g_malloc(gulong size);
gpointer _g_malloc0(gulong size);
gpointer _g_realloc(gpointer mem, gulong size);
void _g_free(gpointer mem);

#define g_malloc(size) _g_malloc(size)
#define g_malloc0(size) _g_malloc0(size)
#define g_realloc(mem, size) _g_realloc(mem, size)
#define g_free(mem) _g_free(mem)

#endif

#ifdef ENABLE_MEM_PROFILE
void g_mem_profile(gulong *allocated, gulong *freed, gulong *ppeak);
#else
#define g_mem_profile(allocated, freed, ppeak)
#endif

// General callbacks and handlers
typedef void (*GCallback)(gpointer data, gpointer user_data);
typedef gint (*GCompareHandler)(gcpointer a, gcpointer b);
typedef void (*GFreeCallback)(gpointer data);
void g_free_callback(gpointer data);
#define g_free_with(data, free_callback)                                       \
  if ((data) && (free_callback)) {                                             \
    g_free_callback == (free_callback) ? g_free(data) : (free_callback)(data); \
  }

// Auto memory management
#define g_auto(data, auto_container_name)                                      \
  g_auto_with(data, g_free_callback, auto_container_name)
gpointer g_auto_with(gpointer data, GFreeCallback free_callback,
                     gcstr auto_container_name);
void g_auto_container_free(gcstr auto_container_name);
void g_auto_free();

// Array
typedef struct {
  gpointer data;
  guint len;
  guint alloc;
  guint item_len;
} GArray;

typedef struct {
  gpointer *data;
  guint size;
  guint alloc;
  GFreeCallback free_callback;
} GPtrArray;

typedef void (*GArrayVisitCallback)(GArray *self, guint index, gcpointer item,
                                    gcpointer user_data);
#define g_array_new(type) g_array_new_with(sizeof(type))
GArray *g_array_new_with(guint item_len);
void g_array_free(GArray *self);
#define g_array(self, type) ((type *)(self->data))
#define g_array_get(self, type, index) (*(type *)g_array_get_ref(self, index))
#define g_array_set(self, type, index, val)                                    \
  {                                                                            \
    type __tmp__ = val;                                                        \
    g_array_set_ref(self, index, &__tmp__);                                    \
  }
void g_array_copy(GArray *self, gpointer data, guint index, guint count);
gpointer g_array_get_ref(GArray *self, guint index);
void g_array_set_ref(GArray *self, guint index, gpointer ref);
guint g_array_size(GArray *self);
void g_array_set_size(GArray *self, guint size);
void g_array_set_capacity(GArray *self, guint capacity);
#define g_array_add_ref(self, ref)                                             \
  g_array_insert_ref(self, g_array_size(self), ref)
#define g_array_add(self, type, val)                                           \
  g_array_insert(self, type, g_array_size(self), val)
void g_array_remove(GArray *self, guint index);
void g_array_insert_ref(GArray *self, guint index, gpointer ref);
#define g_array_insert(self, type, index, val)                                 \
  {                                                                            \
    type __tmp__ = val;                                                        \
    g_array_insert_ref(self, index, &__tmp__);                                 \
  }
void g_array_append_items(GArray *self, gpointer items, guint count);
void g_array_prepend_items(GArray *self, gpointer items, guint count);
void g_array_visit(GArray *self, GArrayVisitCallback func, gpointer user_data);

typedef void (*GPtrArrayVisitCallback)(GPtrArray *self, guint index,
                                       gcpointer item, gcpointer user_data);
#define g_ptr_array_new() g_ptr_array_new_with(NULL)
GPtrArray *g_ptr_array_new_with(GFreeCallback free_func);
void g_ptr_array_free(GPtrArray *self);
#define g_ptr_array_get(self, index) (self)->data[index]
#define g_ptr_array_set(self, index, val) (self)->data[index] = val
#define g_ptr_array_size(self) ((self)->size)
void g_ptr_array_set_size(GPtrArray *self, guint length);
void g_ptr_array_set_capacity(GPtrArray *self, guint capacity);
#define g_ptr_array_add(self, data)                                            \
  g_ptr_array_insert(self, g_ptr_array_size(self), data)
void g_ptr_array_remove(GPtrArray *self, guint index);
void g_ptr_array_insert(GPtrArray *self, guint index, gpointer data);
void g_ptr_array_append_items(GPtrArray *self, gpointer *items, guint count);
void g_ptr_array_prepend_items(GPtrArray *self, gpointer *items, guint count);
gint g_ptr_array_index_of(GPtrArray *self, gpointer item);
void g_ptr_array_visit(GPtrArray *self, GPtrArrayVisitCallback func,
                       gpointer user_data);

// Singly linked list
typedef struct _GListNode GListNode;
struct _GListNode {
  gpointer data;
  GListNode *next;
};

typedef struct {
  GListNode *head;
  GFreeCallback node_data_free_callback;
} GList;

typedef void (*GListVisitCallback)(GList *self, GListNode *item,
                                   gcpointer user_data);
#define g_list_new() g_list_new_with(NULL)
GList *g_list_new_with(GFreeCallback node_data_free_callback);
GListNode *g_list_node_new(gpointer data);
void g_list_free(GList *self);
GListNode *g_list_last(GList *self);
guint g_list_size(GList *self);
void g_list_append(GList *self, gpointer data);
void g_list_prepend(GList *self, gpointer data);
void g_list_remove(GList *self, gpointer data);
gint g_list_index_of(GList *self, gpointer data);
GListNode *g_list_get(GList *self, guint n);
void g_list_visit(GList *self, GListVisitCallback func, gpointer user_data);

// HashMap
typedef struct _GMap GMap;
typedef gpointer (*GMapKeyNewHandler)(gpointer data);
typedef void (*GMapVisitCallback)(GMap *self, gpointer key, gpointer value,
                                  gpointer user_data);

typedef struct {
  gpointer key;
  gpointer value;
  GFreeCallback value_custom_free_callback;
} GMapEntry;

struct _GMap {
  GArray *data;
  GCompareHandler key_compare_handler;
  GFreeCallback key_free_callback;
  GFreeCallback value_default_free_callback;
  GMapKeyNewHandler key_new_handler;
};

#define g_map_new(value_free_callback)                                         \
  g_map_new_with((GMapKeyNewHandler)g_dup, g_free_callback,                    \
                 value_free_callback, (GCompareHandler)g_cmp)
GMap *g_map_new_with(GMapKeyNewHandler key_new_handler,
                     GFreeCallback key_free_callback,
                     GFreeCallback value_free_callback,
                     GCompareHandler key_compare_handler);
void g_map_free(GMap *self);
gbool g_map_has(GMap *self, gcpointer key);
gpointer g_map_get(GMap *self, gcpointer key);
GMapEntry *g_map_get_entry(GMap *self, gcpointer key);
#define g_map_set(self, key, value) g_map_set_with(self, key, value, NULL)
void g_map_set_with(GMap *self, gpointer key, gpointer value,
                    GFreeCallback value_free_callback);
void g_map_remove(GMap *self, gcpointer key);
void g_map_remove_all(GMap *self);
guint g_map_size(GMap *self);
void g_map_visit(GMap *self, GMapVisitCallback func, gpointer user_data);

// String utility
#define g_len(str) strlen(str)
#define g_cpy(strdst, strsrc) strcpy(strdst, strsrc)
#define g_ncpy(strdst, strsrc, len) strncpy(strdst, strsrc, len)
#define g_num(str) g_parse(str, '\0', 10)
#define g_equal(str1, str2) (g_cmp(str1, str2) == 0)
void g_delimit(gstr str, gcstr delimiters, gchar new_delimiter);
gstr g_dup(gcstr str);
gint g_cmp(gcstr str1, gcstr str2);
void g_down(gstr str);
void g_up(gstr str);
void g_reverse(gstr str);
gbool g_start_with(gstr str, gstr sub);
gbool g_end_with(gstr str, gstr sub);
gint g_index_of(gstr str, gstr sub, gint index);
gint g_last_index_of(gstr str, gstr sub);
gstr g_substring(gstr str, gint st, gint len);
gint g_hex(gchar c);
gint g_parse(gstr str, gchar chend, gint base);
gstr g_replace(gstr source, gstr sub, gstr rep);
gstr g_replace_free(gstr source, gstr sub, gstr rep);
gstr g_trim(gstr str);
gbool g_is_space(gwchar c);
gstr g_format(gcstr fmt, ...);
void g_format_to(gstr buffer, guint len, gcstr fmt, ...);

int g_log_enabled(gstr file, int line, gstr func, gstr level);
void g_log(gstr fmt, ...);

#ifdef HAVE_TIMER
typedef struct _GTimer {
  ghandle handle;
  gbool enabled;
  gint interval;
  gbool support_background_running;
  GList *callback_list;
  GList *data_list;
} GTimer;
typedef void (*GTimerCallback)(GTimer *timer, gpointer data);
GTimer *g_timer_new(gint interval);
GTimer *g_timer_new_ex(gint interval, gbool background);
void g_timer_destroy(GTimer *timer);

gbool g_timer_is_enabled(GTimer *timer);
void g_timer_start(GTimer *timer);
void g_timer_stop(GTimer *timer);
void g_timer_add_listener(GTimer *timer, GTimerCallback callback,
                          gpointer data);
void g_timer_remove_listener(GTimer *timer, GTimerCallback callback,
                             gpointer data);
#endif

// gulong g_mktime(GDateTime* time);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __G_LIB_H__ */
