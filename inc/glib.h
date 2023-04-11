/*
 * Copyright (c) 2023 Gembera Team
 * https://github.com/gembera
 * License: MIT
 */

#ifndef __G_LIB_H__
#define __G_LIB_H__

#include "glibconfig.h"

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

/* Provide simple macro statement wrappers (adapted from Perl):
 *  G_STMT_START { statements; } G_STMT_END;
 *  can be used as a single statement, as in
 *  if (x) G_STMT_START { ... } G_STMT_END; else ...
 */
#if !(defined(G_STMT_START) && defined(G_STMT_END))
#if defined(__GNUC__) && !defined(__STRICT_ANSI__) && !defined(__cplusplus)
#define G_STMT_START	(void)(
#define G_STMT_END		)
#else
#if (defined(sun) || defined(__sun__))
#define G_STMT_START if (1)
#define G_STMT_END else(void) 0
#else
#define G_STMT_START do
#define G_STMT_END while (0)
#endif
#endif
#endif

#ifdef __DMALLOC_H__

#define g_new(type, count) ALLOC(type, count)
#define g_new0(type, count) CALLOC(type, count)

#else /* __DMALLOC_H__ */

#define g_new(type, count) ((type *)g_malloc((unsigned)sizeof(type) * (count)))
#define g_new0(type, count)                                                    \
  ((type *)g_malloc0((unsigned)sizeof(type) * (count)))
#endif /* __DMALLOC_H__ */

#ifdef G_DISABLE_ASSERT

#define g_assert(expr)
#define g_assert_not_reached()

#else /* !G_DISABLE_ASSERT */

#ifdef __GNUC__

#define g_assert(expr)                                                         \
  G_STMT_START {                                                               \
    if (!(expr))                                                               \
      g_error("file %s: line %d (%s): \"%s\"", __FILE__, __LINE__,             \
              __PRETTY_FUNCTION__, #expr);                                     \
  }                                                                            \
  G_STMT_END

#define g_assert_not_reached()                                                 \
  G_STMT_START {                                                               \
    g_error("file %s: line %d (%s): \"should not be reached\"", __FILE__,      \
            __LINE__, __PRETTY_FUNCTION__);                                    \
  }                                                                            \
  G_STMT_END

#else /* !__GNUC__ */

#define g_assert(expr)                                                         \
  G_STMT_START {                                                               \
    if (!(expr))                                                               \
      g_error("file %s: line %d: \"%s\"", __FILE__, __LINE__, #expr);          \
  }                                                                            \
  G_STMT_END

#define g_assert_not_reached()                                                 \
  G_STMT_START {                                                               \
    g_error("file %s: line %d: \"should not be reached\"", __FILE__,           \
            __LINE__);                                                         \
  }                                                                            \
  G_STMT_END

#endif /* __GNUC__ */

#endif /* G_DISABLE_ASSERT */

#ifdef G_DISABLE_CHECKS

#define g_return_if_fail(expr)
#define g_return_val_if_fail(expr, val)

#else /* !G_DISABLE_CHECKS */

#ifdef __GNUC__

#define g_return_if_fail(expr)                                                 \
  G_STMT_START {                                                               \
    if (!(expr)) {                                                             \
      g_warning("file %s: line %d (%s): assertion \"%s\" failed.\n", __FILE__, \
                __LINE__, __PRETTY_FUNCTION__, #expr);                         \
      return;                                                                  \
    };                                                                         \
  }                                                                            \
  G_STMT_END

#define g_return_val_if_fail(expr, val)                                        \
  G_STMT_START {                                                               \
    if (!(expr)) {                                                             \
      g_warning("file %s: line %d (%s): assertion \"%s\" failed.\n", __FILE__, \
                __LINE__, __PRETTY_FUNCTION__, #expr);                         \
      return val;                                                              \
    };                                                                         \
  }                                                                            \
  G_STMT_END

#else /* !__GNUC__ */

#define g_return_if_fail(expr)                                                 \
  G_STMT_START {                                                               \
    if (!(expr)) {                                                             \
      g_warning("file %s: line %d: assertion. \"%s\" failed.\n", __FILE__,     \
                __LINE__, #expr);                                              \
      return;                                                                  \
    };                                                                         \
  }                                                                            \
  G_STMT_END

#define g_return_val_if_fail(expr, val)                                        \
  G_STMT_START {                                                               \
    if (!(expr)) {                                                             \
      g_warning("file %s: line %d: assertion \"%s\" failed.\n", __FILE__,      \
                __LINE__, #expr);                                              \
      return val;                                                              \
    };                                                                         \
  }                                                                            \
  G_STMT_END

#endif /* !__GNUC__ */

#endif /* G_DISABLE_CHECKS */

#ifdef __cplusplus
extern "C" {
#pragma }
#endif /* __cplusplus */

/* Provide type definitions for commonly used types.
 *  These are useful because a "gint8" can be adjusted
 *  to be 1 byte (8 bits) on all platforms. Similarly and
 *  more importantly, "gint32" can be adjusted to be
 *  4 bytes (32 bits) on all platforms.
 */

typedef char gchar;
typedef short gshort;
typedef long glong;
typedef int gint;
typedef gint gboolean;

typedef unsigned char guchar;
typedef unsigned short gushort;
typedef unsigned long gulong;
typedef unsigned int guint;

typedef float gfloat;
typedef double gdouble;

typedef void *gpointer;
typedef const void *gconstpointer;

typedef gpointer ghandle;

#if (SIZEOF_CHAR == 1)
typedef signed char gint8;
typedef unsigned char guint8;
#endif /* SIZEOF_CHAR */

#if (SIZEOF_SHORT == 2)
typedef signed short gint16;
typedef unsigned short guint16;
#endif /* SIZEOF_SHORT */

#if (SIZEOF_INT == 4)
typedef signed int gint32;
typedef unsigned int guint32;
#elif (SIZEOF_LONG == 4)
typedef signed long gint32;
typedef unsigned long guint32;
#endif /* SIZEOF_INT */

#if (SIZEOF_LONG == 8)
#define HAVE_GINT64 1
typedef signed long gint64;
typedef unsigned long guint64;
#elif (SIZEOF_LONG_LONG == 8)
#define HAVE_GINT64 1
typedef signed long long gint64;
typedef unsigned long long guint64;
#else
/* No gint64 */
#undef HAVE_GINT64
#endif

typedef guint16 gwchar;
typedef gchar *gstring;
typedef gwchar *gwstring;
typedef const gchar *gconststring;
typedef const gwchar *gconstwstring;

/* Define macros for storing integers inside pointers */

#if (SIZEOF_INT == SIZEOF_VOID_P)

#define GPOINTER_TO_INT(p) ((gint)(p))
#define GPOINTER_TO_UINT(p) ((guint)(p))

#define GINT_TO_POINTER(i) ((gpointer)(i))
#define GUINT_TO_POINTER(u) ((gpointer)(u))

#else

#define GPOINTER_TO_INT(p) ((gint)(glong)(p))
#define GPOINTER_TO_UINT(p) ((guint)(gulong)(p))

#define GINT_TO_POINTER(i) ((gpointer)(glong)(i))
#define GUINT_TO_POINTER(u) ((gpointer)(gulong)(u))

#endif

typedef struct _GList GList;
typedef struct _GSList GSList;
typedef struct _GHashNode GHashNode;
typedef struct _GHashTable GHashTable;
typedef struct _GString GString;
typedef struct _GArray GArray;
typedef struct _GPtrArray GPtrArray;
typedef struct _GByteArray GByteArray;
typedef struct _GTimer GTimer;
typedef struct _GBuffer GBuffer;

struct _GBuffer {
  gint length;
  gpointer buffer;
};

struct _GTimer {
  ghandle handle;
  gboolean enabled;
  gint interval;
  gboolean support_background_running;
  GList *callback_list;
  GList *data_list;
};

typedef void (*GFunc)(gpointer data, gpointer user_data);
typedef void (*GHFunc)(gpointer key, gpointer value, gpointer user_data);

typedef guint (*GHashFunc)(gconstpointer key);
typedef gint (*GCompareFunc)(gconstpointer a, gconstpointer b);
typedef gboolean (*GSearchFunc)(gconstpointer item, gconstpointer target);

typedef void (*GTimerCallback)(GTimer *timer, gpointer data);

typedef void (*GMemRecordCallback)(gulong index, gpointer memnew,
                                   gpointer memfree, gulong allocated,
                                   gulong freed, const char *__file__,
                                   const int __line__);

struct _GList {
  gpointer data;
  GList *next;
  GList *prev;
};

struct _GSList {
  gpointer data;
  GSList *next;
};

struct _GString {
  gchar *str;
  gint len;
};

struct _GArray {
  gpointer data;
};

struct _GPtrArray {
  gpointer *data;
  gint len;
};

struct _GHashNode {
  guint hash;
  gpointer key;
  gpointer value;
};

struct _GHashTable {
  GArray *nodes;
  GHashFunc hash_func;
  GCompareFunc key_compare_func;
};

void g_buffer_set_length(gint len);
gint g_buffer_get_length(void);
gpointer g_buffer_get(void);
void g_buffer_release(gpointer buf);

/* Doubly linked lists
 */
GList *g_list_alloc(void);
void g_list_free(GList *list);
void g_list_free_1(GList *list);
GList *g_list_append(GList *list, gpointer data);
GList *g_list_prepend(GList *list, gpointer data);
GList *g_list_insert(GList *list, gpointer data, gint position);
GList *g_list_insert_sorted(GList *list, gpointer data, GCompareFunc func);
GList *g_list_concat(GList *list1, GList *list2);
GList *g_list_remove(GList *list, gpointer data);
GList *g_list_remove_link(GList *list, GList *link);
GList *g_list_reverse(GList *list);
GList *g_list_nth(GList *list, guint n);
GList *g_list_find(GList *list, gpointer data);
GList *g_list_find_custom(GList *list, gpointer data, GCompareFunc func);
gint g_list_position(GList *list, GList *link);
gint g_list_index(GList *list, gpointer data);
GList *g_list_last(GList *list);
GList *g_list_first(GList *list);
guint g_list_length(GList *list);
void g_list_foreach(GList *list, GFunc func, gpointer user_data);
gpointer g_list_nth_data(GList *list, guint n);

#define g_list_previous(list) ((list) ? (((GList *)(list))->prev) : NULL)
#define g_list_next(list) ((list) ? (((GList *)(list))->next) : NULL)

/* Singly linked lists
 */
GSList *g_slist_alloc(void);
void g_slist_free(GSList *list);
void g_slist_free_1(GSList *list);
GSList *g_slist_append(GSList *list, gpointer data);
GSList *g_slist_prepend(GSList *list, gpointer data);
GSList *g_slist_insert(GSList *list, gpointer data, gint position);
GSList *g_slist_insert_sorted(GSList *list, gpointer data, GCompareFunc func);
GSList *g_slist_concat(GSList *list1, GSList *list2);
GSList *g_slist_remove(GSList *list, gpointer data);
GSList *g_slist_remove_link(GSList *list, GSList *link);
GSList *g_slist_reverse(GSList *list);
GSList *g_slist_nth(GSList *list, guint n);
GSList *g_slist_find(GSList *list, gpointer data);
GSList *g_slist_find_custom(GSList *list, gpointer data, GCompareFunc func);
gint g_slist_position(GSList *list, GSList *link);
gint g_slist_index(GSList *list, gpointer data);
GSList *g_slist_last(GSList *list);
guint g_slist_length(GSList *list);
void g_slist_foreach(GSList *list, GFunc func, gpointer user_data);
gpointer g_slist_nth_data(GSList *list, guint n);

#define g_slist_next(slist) ((slist) ? (((GSList *)(slist))->next) : NULL)

/* Hash tables
 */
GHashTable *g_hash_table_new(GHashFunc hash_func,
                             GCompareFunc key_compare_func);
#define g_hash_table_new_with_string_key()                                     \
  g_hash_table_new(g_str_hash, g_str_equal)
void g_hash_table_destroy(GHashTable *hash_table);
void g_hash_table_deep_destroy(GHashTable *hash_table);
void g_hash_table_insert(GHashTable *hash_table, gpointer key, gpointer value);
void g_hash_table_remove(GHashTable *hash_table, gconstpointer key);
gpointer g_hash_table_lookup(GHashTable *hash_table, gconstpointer key);
gboolean g_hash_table_lookup_extended(GHashTable *hash_table,
                                      gconstpointer lookup_key,
                                      gpointer *orig_key, gpointer *value);
void g_hash_table_foreach(GHashTable *hash_table, GHFunc func,
                          gpointer user_data);
gint g_hash_table_size(GHashTable *hash_table);
void g_hash_table_clear(GHashTable *hash_table);

/* Memory
 */

#ifdef ENABLE_MEM_RECORD
void g_mem_record_default_callback(gulong index, gpointer memnew,
                                   gpointer memfree, gulong allocated,
                                   gulong freed, const char *__file__,
                                   const int __line__);
void g_mem_record(GMemRecordCallback callback);
void g_mem_record_begin();
void g_mem_record_end();

gpointer g_mem_record_malloc(gulong size, const char *__file__,
                             const int __line__);
gpointer g_mem_record_malloc0(gulong size, const char *__file__,
                              const int __line__);
gpointer g_mem_record_realloc(gpointer mem, gulong size, const char *__file__,
                              const int __line__);
void g_mem_record_free(gpointer mem, const char *__file__, const int __line__);

gpointer _g_malloc(gulong size);
gpointer _g_malloc0(gulong size);
gpointer _g_realloc(gpointer mem, gulong size);
void _g_free(gpointer mem);

#define g_malloc(size) g_mem_record_malloc(size, __FILE__, __LINE__)
#define g_malloc0(size) g_mem_record_malloc0(size, __FILE__, __LINE__)
#define g_realloc(mem, size) g_mem_record_realloc(mem, size, __FILE__, __LINE__)
#define g_free(mem) g_mem_record_free(mem, __FILE__, __LINE__)
#else
#define g_mem_record()
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
void g_mem_profile(gulong *allocated, gulong *freed);
#endif

/* String utility functions
 */
#define G_STR_DELIMITERS "_-|> <."
void g_strdelimit(gchar *string, const gchar *delimiters, gchar new_delimiter);
gchar *g_strdup(const gchar *str);
gchar *g_strconcat(const gchar *string1, ...); /* NULL terminated */
gint g_strcasecmp(const gchar *s1, const gchar *s2);
void g_strdown(gchar *string);
void g_strup(gchar *string);
void g_strreverse(gchar *string);
void g_strcpy(gchar *dst, gchar *src);
gboolean g_strstartwith(gchar *string, gchar *sub);
gboolean g_strendwith(gchar *string, gchar *sub);
gint g_strindexof(gstring fstring, gchar *str, gint index);
gint g_strlastindexof(gstring fstring, gchar *str);
gstring g_strsubstring(gstring fstring, gint st, gint len);
gint g_strgethex(gchar c);
gint g_strparseinteger(gstring fstring, gchar chend, gint base);
#define g_strtointeger(str) g_strparseinteger(str, '\0', 10)
#define g_strequal(s1, s2) (g_strcasecmp(s1, s2) == 0)
gchar *g_strreplace(gchar *source, gchar *sub, gchar *rep);
gchar *g_strreplaceandfree(gchar *source, gchar *sub, gchar *rep);
gchar *g_strtrim(gchar *str);

#ifdef HAVE_MEMMOVE
#define g_memmove memmove
#else
char *_memmove(char *dst, register char *src, register int n);
#define g_memmove _memmove
#endif

/* Strings
 */

gstring g_limit(gstring str, gint len);
gstring g_format(const gstring format, ...);
gwstring g_unicode(gconststring str);
gwstring g_unicode_dup(gconststring str);

#define g_utf8_length_of_wchar(c)                                              \
  ((c) < 0x80    ? 1                                                           \
   : (c) < 0x800 ? 2                                                           \
                 : ((c) < 0x10000     ? 3                                      \
                    : (c) < 0x200000  ? 4                                      \
                    : (c) < 0x4000000 ? 5                                      \
                                      : 6))
#define g_utf8_skip(c)                                                         \
  ((c) < 0xC0   ? 1                                                            \
   : (c) < 0xE0 ? 2                                                            \
                : ((c) < 0xF0 ? 3 : ((c) < 0xF8 ? 4 : ((c) < 0xFC ? 5 : 6))))
#define g_utf8_next_char(p) (char *)((p) + g_utf8_skip(*(guchar *)(p)))
gstring g_utf8(gconstwstring str);
gstring g_utf8_dup(gconstwstring str);
gint g_utf8_strlen(const gchar *p, gint max);
gwchar g_utf8_get_char(const gchar *p);
int g_unichar_to_utf8(gwchar c, gchar *outbuf);

gboolean g_is_space(gwchar c);
#define g_is_cjk(c) ((c) >= 8192)

GString *g_string_new(const gchar *init);
GString *g_string_wrap(gchar *init);
GString *g_string_sized_new(guint dfl_size);
void g_string_free(GString *string);
GString *g_string_assign(GString *lval, const gchar *rval);
GString *g_string_truncate(GString *string, gint len);
GString *g_string_append(GString *string, const gchar *val);
GString *g_string_append_with_length(GString *string, const gchar *val,
                                     gint len);
GString *g_string_append_c(GString *string, gchar c);
GString *g_string_prepend(GString *string, const gchar *val);
GString *g_string_prepend_c(GString *string, gchar c);
GString *g_string_insert(GString *string, gint pos, const gchar *val);
GString *g_string_insert_c(GString *string, gint pos, gchar c);
GString *g_string_erase(GString *string, gint pos, gint len);
GString *g_string_down(GString *string);
GString *g_string_up(GString *string);
void g_string_sprintf(GString *string, const gchar *format, ...);
void g_string_sprintfa(GString *string, const gchar *format, ...);
GString *g_string_trim(GString *fstring);
gint g_string_index_of(GString *fstring, gchar *str, int index);
GString *g_string_substring(GString *fstring, gint st, gint len);
GPtrArray *g_string_split(GString *fstring, gchar *str);
gint g_string_parse_integer(GString *fstring, gchar chend, gint base);
#define g_string_to_integer(str) g_string_parse_integer(str, '\0', 10)

int g_log_enabled(char *file, int line, char *func, char *level);
void g_log(char *fmt, ...);
void base64_cleanup();

/* Resizable arrays
 */
GArray *g_array_new(gint item_size);
#define g_array_new_of(type) g_array_new(sizeof(type))
void g_array_free(GArray *array);
#define g_array(array, type) ((type *)(array->data))
#define g_array_get(array, type, index) ((type *)(array->data))[index]
#define g_array_set(array, type, index, val)                                   \
  ((type *)(array->data))[index] = val
gint g_array_length(GArray *array);
void g_array_set_length(GArray *arr, gint length);
void g_array_set_capacity(GArray *arr, gint length);
#define g_array_add_ref(arr, ref)                                              \
  g_array_insert_ref(arr, g_array_length(arr), ref)
#define g_array_add(arr, type, val)                                            \
  g_array_insert(arr, type, g_array_length(arr), val)
void g_array_remove(GArray *arr, gint index);
void g_array_insert_ref(GArray *arr, gint index, gpointer ref);
#define g_array_insert(arr, type, index, val)                                  \
  {                                                                            \
    type __tmp__ = val;                                                        \
    g_array_insert_ref(arr, index, &__tmp__);                                  \
  }
void g_array_append_items(GArray *arr, gpointer items, gint count);
void g_array_prepend_items(GArray *arr, gpointer items, gint count);

/* Resizable pointer array.
 */
GPtrArray *g_ptr_array_new(void);
void g_ptr_array_free(GPtrArray *array);
#define g_ptr_array(array, type) ((type *)(array->data))
#define g_ptr_array_get(array, index) array->data[index]
#define g_ptr_array_set(array, index, val) array->data[index] = val
#define g_ptr_array_length(array) (array->len)
void g_ptr_array_set_length(GPtrArray *array, gint length);
void g_ptr_array_set_capacity(GPtrArray *farray, gint capacity);
#define g_ptr_array_add(array, data)                                           \
  g_ptr_array_insert(array, g_ptr_array_length(array), data)
void g_ptr_array_remove(GPtrArray *array, gint index);
void g_ptr_array_insert(GPtrArray *farray, gint index, gpointer data);
void g_ptr_array_append_items(GPtrArray *arr, gpointer *items, gint count);
void g_ptr_array_prepend_items(GPtrArray *arr, gpointer *items, gint count);
gint g_ptr_array_index_of(GPtrArray *array, gpointer item);
gint g_ptr_array_search(GPtrArray *array, GSearchFunc func, gpointer item);

#ifdef GLIB_HASE_TIME
GTimer *g_timer_new(gint interval);
GTimer *g_timer_new_ex(gint interval, gboolean background);
void g_timer_destroy(GTimer *timer);

gboolean g_timer_is_enabled(GTimer *timer);
void g_timer_start(GTimer *timer);
void g_timer_stop(GTimer *timer);
void g_timer_add_listener(GTimer *timer, GTimerCallback callback,
                          gpointer data);
void g_timer_remove_listener(GTimer *timer, GTimerCallback callback,
                             gpointer data);
#endif

/* Hash Functions
 */
gint g_str_equal(gconstpointer v, gconstpointer v2);
guint g_str_hash(gconstpointer v);

gint g_str_iequal(gconstpointer v, gconstpointer v2);
guint g_str_ihash(gconstpointer v);

gint g_int_equal(gconstpointer v, gconstpointer v2);
guint g_int_hash(gconstpointer v);

guint g_str_aphash(gconstpointer v);

guint g_str_aphash_with_length(gconstpointer v, guint len);

/* This "hash" function will just return the key's adress as an
 * unsigned integer. Useful for hashing on plain adresses or
 * simple integer values.
 */
guint g_direct_hash(gconstpointer v);
gint g_direct_equal(gconstpointer v, gconstpointer v2);

// gulong g_mktime(GDateTime* time);

/* Glib static variable clean
 */

// void g_mem_static_clean_gmcore(void);

gstring g_base64_encode(const gstring data, gint input_length,
                        gint *output_length);
gstring g_base64_decode(const gstring data, gint input_length,
                        gint *output_length);

gint g_random(gint max);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __G_LIB_H__ */
