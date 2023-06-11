/*
 * Copyright (c) 2023 Gembera Team
 * https://github.com/gembera
 * License: MIT
 */

#include "glib.h"
#include "gstring.h"

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
gstr g_format(gcstr fmt, ...) {
  GString *str = g_string_new();
  va_list ap;
  va_start(ap, fmt);
  g_string_vprintf(str, fmt, ap);
  va_end(ap);
  return g_string_unwrap(str);
}
void g_format_to(gstr buffer, guint len, gcstr fmt, ...) {
  GString *str = g_string_wrap(buffer, len);
  va_list ap;
  va_start(ap, fmt);
  g_string_vprintf(str, fmt, ap);
  va_end(ap);
  g_string_unwrap(str);
}
