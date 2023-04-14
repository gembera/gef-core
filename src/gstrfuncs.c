/*
 * Copyright (c) 2023 Gembera Team
 * https://github.com/gembera
 * License: MIT
 */

#include "glib.h"

#define G_STR_DELIMITERS "_-|> <."

gstr g_dup(gcstr str) {
  gstr new_str;
  new_str = NULL;
  if (str) {
    new_str = g_malloc0(g_len(str) + 1);
    g_ncpy(new_str, str, g_len(str));
  }
  return new_str;
}
gstr g_replace_free(gstr source, gstr sub, gstr rep) {
  gstr dst = g_replace(source, sub, rep);
  g_free(source);
  return dst;
}
gstr g_replace(gstr source, gstr sub, gstr rep) {
  gstr result;
  gstr pc1, pc2, pc3;
  gint isource, isub, irep;
  isub = g_len(sub);
  irep = g_len(rep);
  isource = g_len(source);
  if (0 == *sub)
    return g_dup(source);
  result = (gstr)g_malloc0(
      (irep > isub ? irep * g_len(source) / isub + 1 : isource) + 1);
  pc1 = result;
  while (*source != 0) {
    pc2 = source;
    pc3 = sub;
    while (*pc2 == *pc3 && *pc3 != 0 && *pc2 != 0)
      pc2++, pc3++;
    if (0 == *pc3) {
      pc3 = rep;
      while (*pc3 != 0)
        *pc1++ = *pc3++;
      pc2--;
      source = pc2;
    } else
      *pc1++ = *source;
    source++;
  }
  *pc1 = 0;
  return result;
}

gstr g_concat(gcstr str1, ...) {
  g_return_val_if_fail(str1, NULL);
  guint l;
  va_list args;
  gstr s;
  gstr concat;

  l = 1 + g_len(str1);
  va_start(args, str1);
  s = va_arg(args, gstr);
  while (s) {
    l += g_len(s);
    s = va_arg(args, gstr);
  }
  va_end(args);

  concat = g_malloc(l);
  concat[0] = 0;

  strcat(concat, str1);
  va_start(args, str1);
  s = va_arg(args, gstr);
  while (s) {
    strcat(concat, s);
    s = va_arg(args, gstr);
  }
  va_end(args);

  return concat;
}

void g_down(gstr str) {
  g_return_if_fail(str);
  register gstr s;
  s = str;
  while (*s) {
    *s = tolower(*s);
    s++;
  }
}

void g_up(gstr str) {
  g_return_if_fail(str);
  register gstr s;
  s = str;
  while (*s) {
    *s = toupper(*s);
    s++;
  }
}

void g_reverse(gstr str) {
  g_return_if_fail(str != NULL);
  if (*str) {
    register gstr h, t;
    h = str;
    t = str + g_len(str) - 1;
    while (h < t) {
      register gchar c;
      c = *h;
      *h = *t;
      h++;
      *t = c;
      t--;
    }
  }
}
gint g_cmp(gcstr str1, gcstr str2) {
  g_return_val_if_fail(str1 && str2, 0);
  gint c1, c2;
  while (*str1 && *str2) {
    c1 = *str1;
    c2 = *str2;
    if (c1 != c2)
      return (c1 - c2);
    str1++;
    str2++;
  }
  return (((gint)(guchar)*str1) - ((gint)(guchar)*str2));
}

void g_delimit(gstr str, gcstr delimiters, gchar new_delim) {
  g_return_if_fail(str != NULL);
  register gstr c;

  if (!delimiters)
    delimiters = G_STR_DELIMITERS;

  for (c = str; *c; c++) {
    if (strchr(delimiters, *c))
      *c = new_delim;
  }
}

gbool g_start_with(gstr str, gstr sub) {
  gint i, len = g_len(sub);
  if ((gint)g_len(str) < len)
    return FALSE;
  for (i = 0; i < len; i++) {
    if (str[i] != sub[i])
      return FALSE;
  }
  return TRUE;
}
gbool g_end_with(gstr str, gstr sub) {
  gint i, len = g_len(sub);
  gint lensrc = g_len(str);
  if (lensrc < len)
    return FALSE;
  lensrc -= len;
  for (i = 0; i < len; i++) {
    if (str[lensrc + i] != sub[i])
      return FALSE;
  }
  return TRUE;
}
gint g_index_of(gstr str, gstr sub, gint index) {
  gstr l;
  if (index < 0)
    index = 0;
  else if (index >= (gint)g_len(str))
    return -1;
  l = strstr(str + index, sub);
  return l == NULL ? -1 : l - str;
}
gint g_last_index_of(gstr str, gstr sub) {
  gint pos = -1;
  gint posnext;
  gint len;
  if (sub == NULL)
    return -1;
  len = g_len(sub);
  if (len == 0)
    return -1;
  do {
    posnext = g_index_of(str, sub, pos);
    if (posnext == -1)
      return pos == -1 ? -1 : pos - len;
    pos = posnext + len;
  } while (TRUE);
}
gstr g_substring(gstr str, gint st, gint len) {
  gint i;
  gstr r;
  gint fslen = g_len(str);
  if (len <= 0)
    len = fslen - st;
  if (st + len > fslen)
    len = fslen - st;
  r = g_malloc(len + 1);
  for (i = 0; i < len; i++)
    r[i] = str[st + i];
  r[len] = '\0';
  return r;
}

gint g_hex(gchar c) {
  if ('0' <= c && c <= '9')
    return c - '0';
  if ('A' <= c && c <= 'F')
    return c - 'A' + 10;
  if ('a' <= c && c <= 'f')
    return c - 'a' + 10;
  return -1;
}
gint g_parse(gstr str, gchar chend, gint base) {
  gstr s = str;
  gbool negate = FALSE;
  gint result = 0;

  while (g_is_space(*s))
    s++;

  if (*s == '-') {
    negate = TRUE;
    s++;
  } else if (*s == '+') {
    s++;
  }

  while (*s != chend) {
    int i = g_hex(*s);
    if (i == -1)
      break;
    result = result * base + i;
    s++;
  }

  if (negate) {
    result = -result;
  }

  return result;
}

gstr g_trim(gstr str) {
  gint i;
  gint len;
  gstr result;
  if (str == NULL)
    return NULL;
  len = g_len(str);
  for (i = len - 1; i >= 0 && g_is_space(str[i]); i--, len--)
    ;
  for (i = 0; i < len && g_is_space(str[i]); i++)
    ;
  result = g_malloc(len - i + 1);
  g_memmove(result, str + i, len - i);
  result[len - i] = '\0';
  return result;
}

gbool g_is_space(gwchar c) {
  return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

gint g_format_max_length(gcstr fmt, va_list args) {
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
          len += va_arg(args, int);
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
          len += strtol(fmt, (gstr *)&fmt, 10);
          break;
        case 'h':
          short_int = TRUE;
          break;
        case 'l':
          long_int = TRUE;
          break;

          /* I ignore 'q' and 'L', they're not portable anyway. */

        case 's':
          tmp = va_arg(args, gstr);
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
            (void)va_arg(args, long);
          else if (short_int)
            (void)va_arg(args, int);
          else
            (void)va_arg(args, int);
          len += 32;
          done = TRUE;
          break;
        case 'D':
        case 'O':
        case 'U':
          (void)va_arg(args, long);
          len += 32;
          done = TRUE;
          break;
        case 'e':
        case 'E':
        case 'f':
        case 'g':
          (void)va_arg(args, double);
          len += 32;
          done = TRUE;
          break;
        case 'c':
          (void)va_arg(args, int);
          len += 1;
          done = TRUE;
          break;
        case 'p':
        case 'n':
          (void)va_arg(args, void *);
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

gstr g_format(gcstr fmt, ...) {
  va_list args;
  va_start(args, fmt);
  gint length = g_format_max_length(fmt, args);
  va_end(args);
  gstr buffer = g_malloc0(length + 1);
  va_start(args, fmt);
  vsprintf(buffer, fmt, args);
  va_end(args);
  return buffer;
}
void g_format_to(gstr buffer, gcstr fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vsprintf(buffer, fmt, args);
  va_end(args);
}
