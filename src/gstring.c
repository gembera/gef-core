/*
 * Copyright (c) 2023 Gembera Team
 * https://github.com/gembera
 * License: MIT
 */

#include "glib.h"

typedef struct _GRealString GRealString;

struct _GRealString {
  gstr str;
  gint len;
  gint alloc;
};

static gstr global_string_buf = NULL;
static gint global_string_alloc = 0;

static gstr global_utf8_buf = NULL;
static gint global_utf8_alloc = 0;

static gwchar *global_wstring_buf = NULL;
static gint global_wstring_alloc = 0;

void static_clean_gstring() {
  if (global_string_buf != NULL)
    g_free(global_string_buf);
  if (global_wstring_buf != NULL)
    g_free(global_wstring_buf);
  if (global_utf8_buf != NULL)
    g_free(global_utf8_buf);
}

/* Strings.
 */
static gint nearest_pow(gint num) {
  gint n = 1;

  while (n < num)
    n <<= 1;

  return n;
}

static void g_string_maybe_expand(GRealString *string, gint len) {
  if (string->len + len >= string->alloc) {
    // string->alloc = nearest_pow (string->len + len + 1);
    string->alloc = string->len + len + 1;
    string->str = g_realloc(string->str, string->alloc);
    if (string->str == NULL) {
      int i;
      for (i = 0; i < 10 && string->str == NULL; i++) {
        string->str = g_realloc(string->str, string->alloc);
      }
      if (string->str == NULL)
        g_log_error("g_string_maybe_expand : fails to allocate memory!");
    }
  }
}

GString *g_string_wrap(gstr init) {
  GRealString *string;

  string = g_new(GRealString);

  string->len = g_len(init);
  string->alloc = string->len + 1;
  string->str = init;

  return (GString *)string;
}
GString *g_string_sized_new(guint dfl_size) {
  GRealString *string;

  string = g_new(GRealString);

  string->alloc = 0;
  string->len = 0;
  string->str = NULL;

  g_string_maybe_expand(string, MAX(dfl_size, 2));
  string->str[0] = 0;

  return (GString *)string;
}

GString *g_string_new(const gstr init) {
  GString *string;

  string = g_string_sized_new(2);

  if (init)
    g_string_append(string, init);

  return string;
}

void g_string_free(GString *string) {
  g_return_if_fail(string != NULL);

  // if (free_segment)
  g_free(string->str);

  g_free(string);
}

GString *g_string_assign(GString *lval, const gstr rval) {
  g_string_truncate(lval, 0);
  g_string_append(lval, rval);

  return lval;
}

GString *g_string_truncate(GString *fstring, gint len) {
  GRealString *string = (GRealString *)fstring;

  g_return_val_if_fail(string != NULL, NULL);

  string->len = len;

  string->str[len] = 0;

  return fstring;
}

GString *g_string_append_with_length(GString *fstring, const gstr val,
                                     gint len) {
  GRealString *string = (GRealString *)fstring;

  g_return_val_if_fail(string != NULL, NULL);
  g_return_val_if_fail(val != NULL, fstring);

  g_string_maybe_expand(string, len);

  strncpy(string->str + string->len, val, len);

  string->len += len;
  string->str[string->len] = '\0';

  return fstring;
}
GString *g_string_append(GString *fstring, const gstr val) {
  GRealString *string = (GRealString *)fstring;
  int len;

  g_return_val_if_fail(string != NULL, NULL);
  g_return_val_if_fail(val != NULL, fstring);

  len = g_len(val);

  return g_string_append_with_length(fstring, val, len);
}

GString *g_string_append_c(GString *fstring, gchar c) {
  GRealString *string = (GRealString *)fstring;

  g_return_val_if_fail(string != NULL, NULL);
  g_string_maybe_expand(string, 1);

  string->str[string->len++] = c;
  string->str[string->len] = 0;

  return fstring;
}

GString *g_string_prepend(GString *fstring, const gstr val) {
  GRealString *string = (GRealString *)fstring;
  gint len;

  g_return_val_if_fail(string != NULL, NULL);
  g_return_val_if_fail(val != NULL, fstring);

  len = g_len(val);
  g_string_maybe_expand(string, len);

  g_memmove(string->str + len, string->str, string->len);

  strncpy(string->str, val, len);

  string->len += len;

  string->str[string->len] = 0;

  return fstring;
}

GString *g_string_prepend_c(GString *fstring, gchar c) {
  GRealString *string = (GRealString *)fstring;

  g_return_val_if_fail(string != NULL, NULL);
  g_string_maybe_expand(string, 1);

  g_memmove(string->str + 1, string->str, string->len);

  string->str[0] = c;

  string->len += 1;

  string->str[string->len] = 0;

  return fstring;
}

GString *g_string_insert(GString *fstring, gint pos, const gstr val) {
  GRealString *string = (GRealString *)fstring;
  gint len;

  g_return_val_if_fail(string != NULL, NULL);
  g_return_val_if_fail(val != NULL, fstring);
  g_return_val_if_fail(pos >= 0, fstring);
  g_return_val_if_fail(pos <= string->len, fstring);

  len = g_len(val);
  g_string_maybe_expand(string, len);

  g_memmove(string->str + pos + len, string->str + pos, string->len - pos);

  strncpy(string->str + pos, val, len);

  string->len += len;

  string->str[string->len] = 0;

  return fstring;
}

GString *g_string_insert_c(GString *fstring, gint pos, gchar c) {
  GRealString *string = (GRealString *)fstring;

  g_return_val_if_fail(string != NULL, NULL);
  g_return_val_if_fail(pos <= string->len, fstring);

  g_string_maybe_expand(string, 1);

  g_memmove(string->str + pos + 1, string->str + pos, string->len - pos);

  string->str[pos] = c;

  string->len += 1;

  string->str[string->len] = 0;

  return fstring;
}

GString *g_string_erase(GString *fstring, gint pos, gint len) {
  GRealString *string = (GRealString *)fstring;

  g_return_val_if_fail(string != NULL, NULL);
  g_return_val_if_fail(len >= 0, fstring);
  g_return_val_if_fail(pos >= 0, fstring);
  g_return_val_if_fail(pos <= string->len, fstring);
  g_return_val_if_fail(pos + len <= string->len, fstring);

  if (pos + len < string->len)
    g_memmove(string->str + pos, string->str + pos + len,
              string->len - (pos + len));

  string->len -= len;

  string->str[string->len] = 0;

  return fstring;
}

gint g_string_index_of(GString *fstring, gstr str, int index) {
  return g_index_of(fstring->str, str, index);
}
GString *g_string_trim(GString *fstring) {
  gint i;
  for (i = 0; i < fstring->len && g_is_space(fstring->str[i]); i++)
    ;
  g_string_erase(fstring, 0, i);
  for (i = fstring->len - 1; i >= 0 && g_is_space(fstring->str[i]); i--)
    ;
  return g_string_truncate(fstring, i + 1);
}
GPtrArray *g_string_split(GString *fstring, gstr str) {
  gint pos = 0;
  gint l = g_len(str);
  GPtrArray *ar = g_ptr_array_new();
  while (pos < fstring->len) {
    gint p = g_string_index_of(fstring, str, pos);
    if (p == -1)
      break;
    g_ptr_array_add(ar, g_string_substring(fstring, pos, p - pos));
    pos = p + l;
  }
  if (pos < fstring->len)
    g_ptr_array_add(ar, g_string_substring(fstring, pos, -1));
  return ar;
}
gint g_string_parse_integer(GString *fstring, gchar chend, gint base) {
  return g_parse_num(fstring->str, chend, base);
}
GString *g_string_substring(GString *fstring, gint st, gint len) {
  return g_string_wrap(g_substring(fstring->str, st, len));
}
GString *g_string_down(GString *fstring) {
  GRealString *string = (GRealString *)fstring;
  gstr s;

  g_return_val_if_fail(string != NULL, NULL);

  s = string->str;

  while (*s) {
    *s = tolower(*s);
    s++;
  }

  return fstring;
}

GString *g_string_up(GString *fstring) {
  GRealString *string = (GRealString *)fstring;
  gstr s;

  g_return_val_if_fail(string != NULL, NULL);

  s = string->str;

  while (*s) {
    *s = toupper(*s);
    s++;
  }

  return fstring;
}

static int get_length_upper_bound(gcstr fmt, va_list *args) {
  int len = 0;
  int short_int;
  int long_int;
  int done;
  gstr tmp;

  while (*fmt) {
    gchar c = *fmt++;

    short_int = FALSE;
    long_int = FALSE;

    if (c == '%') {
      done = FALSE;
      while (*fmt && !done) {
        switch (*fmt++) {
        case '*':
          len += va_arg(*args, int);
          break;
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
          fmt -= 1;
          len += strtol(fmt, (char **)&fmt, 10);
          break;
        case 'h':
          short_int = TRUE;
          break;
        case 'l':
          long_int = TRUE;
          break;

          /* I ignore 'q' and 'L', they're not portable anyway. */

        case 's':
          tmp = va_arg(*args, char *);
          if (tmp)
            len += g_len(tmp);
          else
            len += g_len("(null)");
          done = TRUE;
          break;
        case 'd':
        case 'i':
        case 'o':
        case 'u':
        case 'x':
        case 'X':
          if (long_int)
            (void)va_arg(*args, long);
          else if (short_int)
            (void)va_arg(*args, int);
          else
            (void)va_arg(*args, int);
          len += 32;
          done = TRUE;
          break;
        case 'D':
        case 'O':
        case 'U':
          (void)va_arg(*args, long);
          len += 32;
          done = TRUE;
          break;
        case 'e':
        case 'E':
        case 'f':
        case 'g':
          (void)va_arg(*args, double);
          len += 32;
          done = TRUE;
          break;
        case 'c':
          (void)va_arg(*args, int);
          len += 1;
          done = TRUE;
          break;
        case 'p':
        case 'n':
          (void)va_arg(*args, void *);
          len += 32;
          done = TRUE;
          break;
        case '%':
          len += 1;
          done = TRUE;
          break;
        default:
          break;
        }
      }
    } else
      len += 1;
  }

  return len;
}

void _allocate_global_string_buf(gint len) {
  if (len >= global_string_alloc) {
    if (global_string_buf)
      g_free(global_string_buf);

    global_string_alloc = len + 1;

    global_string_buf = g_malloc(global_string_alloc);
  }
}

void _allocate_global_utf8_buf(gint len) {
  if (len >= global_utf8_alloc) {
    if (global_utf8_buf)
      g_free(global_utf8_buf);

    global_utf8_alloc = len + 1;

    global_utf8_buf = g_malloc(global_utf8_alloc);
  }
}

void _allocate_global_wstring_buf(gint len) {
  if (len >= global_wstring_alloc) {
    if (global_wstring_buf)
      g_free(global_wstring_buf);

    global_wstring_alloc = len + 1;

    global_wstring_buf = g_new_many(gwchar, global_wstring_alloc);
  }
}
char *g_vsprintf(gcstr fmt, va_list *args, va_list *args2) {

  gint len = get_length_upper_bound(fmt, args);

  _allocate_global_string_buf(len);

  vsprintf(global_string_buf, fmt, *args2);

  return global_string_buf;
}

static char *g_log___file__;
static int g_log___line__;
static char *g_log___func__;
static char *g_log_level;
int g_log_enabled(char *file, int line, char *func, char *level) {
  g_log___file__ = file;
  g_log___line__ = line;
  g_log___func__ = func;
  g_log_level = level;
  // return _vm_log_module(g_log___file__, g_log___line__);
  return 0;
}
void g_log(char *fmt, ...) {
  va_list args, args2;

  va_start(args, fmt);
  va_start(args2, fmt);

  g_vsprintf(fmt, &args, &args2);

  va_end(args);
  va_end(args2);

  g_print("\n%s : %s\n\t in %s (%s #%d)", g_log_level, global_string_buf,
          g_log___func__, g_log___file__, g_log___line__);
  // _vm_log_debug("%s", g_limit(global_string_buf, 100));
}
gstr g_format(gcstr fmt, ...) {
  va_list args, args2;

  va_start(args, fmt);
  va_start(args2, fmt);

  g_vsprintf(fmt, &args, &args2);

  va_end(args);
  va_end(args2);

  return global_string_buf;
}
gstr g_limit(gstr str, gint len) {
  if (global_string_buf != str)
    g_format("%s", str);
  if ((gint)g_len(global_string_buf) > len)
    global_string_buf[len] = '\0';
  return global_string_buf;
}
#define UTF8_COMPUTE(Char, Mask, Len)                                          \
  if (Char < 128) {                                                            \
    Len = 1;                                                                   \
    Mask = 0x7f;                                                               \
  } else if ((Char & 0xe0) == 0xc0) {                                          \
    Len = 2;                                                                   \
    Mask = 0x1f;                                                               \
  } else if ((Char & 0xf0) == 0xe0) {                                          \
    Len = 3;                                                                   \
    Mask = 0x0f;                                                               \
  } else if ((Char & 0xf8) == 0xf0) {                                          \
    Len = 4;                                                                   \
    Mask = 0x07;                                                               \
  } else if ((Char & 0xfc) == 0xf8) {                                          \
    Len = 5;                                                                   \
    Mask = 0x03;                                                               \
  } else if ((Char & 0xfe) == 0xfc) {                                          \
    Len = 6;                                                                   \
    Mask = 0x01;                                                               \
  } else                                                                       \
    Len = -1;

#define UTF8_GET(Result, Chars, Count, Mask, Len)                              \
  (Result) = (Chars)[0] & (Mask);                                              \
  for ((Count) = 1; (Count) < (Len); ++(Count)) {                              \
    if (((Chars)[(Count)] & 0xc0) != 0x80) {                                   \
      (Result) = -1;                                                           \
      break;                                                                   \
    }                                                                          \
    (Result) <<= 6;                                                            \
    (Result) |= ((Chars)[(Count)] & 0x3f);                                     \
  }

gint g_utf8_strlen(gcstr p, gint max) {
  int len = 0;
  gcstr start = p;
  /* special case for the empty string */
  if (!*p)
    return 0;
  /* Note that the test here and the test in the loop differ subtly.
     In the loop we want to see if we've passed the maximum limit --
     for instance if the buffer ends mid-character.  Here at the top
     of the loop we want to see if we've just reached the last byte.  */
  while (max < 0 || p - start < max) {
    p = g_utf8_next_char(p);
    ++len;
    if (!*p || (max > 0 && p - start > max))
      break;
  }
  return len;
}

gwchar g_utf8_get_char(gcstr p) {
  int i, mask = 0, len;
  gwchar result;
  unsigned char c = (unsigned char)*p;

  UTF8_COMPUTE(c, mask, len);
  if (len == -1)
    return (gwchar)-1;
  UTF8_GET(result, p, i, mask, len);

  return result;
}

gwstr g_utf8_to_ucs2(gwstr wbuffer, const char *str, int len) {
  gwstr result;
  gint n_chars, i;
  gcstr p;

  n_chars = g_utf8_strlen(str, len);
  result = wbuffer == NULL ? g_new_many(gwchar, n_chars + 1) : wbuffer;

  p = str;
  for (i = 0; i < n_chars; i++) {
    result[i] = g_utf8_get_char(p);
    p = g_utf8_next_char(p);
  }
  result[n_chars] = 0;
  return result;
}

gwstr g_unicode(gcstr str) {
  if (str == NULL)
    return NULL;
  _allocate_global_wstring_buf(g_utf8_strlen(str, g_len(str)));
  return g_utf8_to_ucs2(global_wstring_buf, str, g_len(str));
}
gwstr g_unicode_dup(gcstr str) {
  return g_utf8_to_ucs2(NULL, str, g_len(str));
}

int g_unichar_to_utf8(gwchar c, gstr outbuf) {
  size_t len = 0;
  int first;
  int i;

  if (c < 0x80) {
    first = 0;
    len = 1;
  } else if (c < 0x800) {
    first = 0xc0;
    len = 2;
  } else if (c < 0x10000) {
    first = 0xe0;
    len = 3;
  } else if (c < 0x200000) {
    first = 0xf0;
    len = 4;
  } else if (c < 0x4000000) {
    first = 0xf8;
    len = 5;
  } else {
    first = 0xfc;
    len = 6;
  }
  if (outbuf != NULL) {
    for (i = len - 1; i > 0; --i) {
      outbuf[i] = (c & 0x3f) | 0x80;
      c >>= 6;
    }
    outbuf[0] = c | first;
  }

  return len;
}

gint g_unicode_strlen(gcwstr str) {
  const gwchar *p = str;
  gint n_chars;
  for (n_chars = 0; *p != 0; p++)
    n_chars += g_unichar_to_utf8(*p, NULL);
  return n_chars;
}
gstr g_ucs2_to_utf8(gstr buffer, gcwstr str) {
  gstr result;
  gint n_chars, i;
  const gwchar *p = str;

  n_chars = g_unicode_strlen(str);

  result = buffer = buffer == NULL ? g_new_many(gchar, n_chars + 1) : buffer;

  for (i = 0; i < n_chars && *p != 0; i++, p++) {
    buffer += g_unichar_to_utf8(*p, buffer);
  }
  *buffer = 0;
  return result;
}

gstr g_utf8(gcwstr str) {
  _allocate_global_utf8_buf(g_unicode_strlen(str));
  return g_ucs2_to_utf8(global_utf8_buf, str);
}
gstr g_utf8_dup(gcwstr str) { return g_ucs2_to_utf8(NULL, str); }

static void g_string_sprintfa_int(GString *string, const gstr fmt,
                                  va_list *args, va_list *args2) {
  g_string_append(string, g_vsprintf(fmt, args, args2));
}

void g_string_sprintf(GString *string, const gstr fmt, ...) {
  va_list args, args2;

  va_start(args, fmt);
  va_start(args2, fmt);

  g_string_truncate(string, 0);

  g_string_sprintfa_int(string, fmt, &args, &args2);

  va_end(args);
  va_end(args2);
}

void g_string_sprintfa(GString *string, const gstr fmt, ...) {
  va_list args, args2;

  va_start(args, fmt);
  va_start(args2, fmt);

  g_string_sprintfa_int(string, fmt, &args, &args2);

  va_end(args);
  va_end(args2);
}