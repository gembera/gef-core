/*
 * Copyright (c) 2023 Gembera Team
 * https://github.com/gembera
 * License: MIT
 */

#include "glib.h"

#define G_STR_DELIMITERS "_-|> <."

void g_cpy(gstring dst, gstring src) { strcpy(dst, src); }
gstring g_dup(gcstring str) {
  gstring new_str;

  new_str = NULL;
  if (str) {
    new_str = g_malloc(g_len(str) + 1);
    strncpy(new_str, str, g_len(str));
  }

  return new_str;
}
gstring g_replace_free(gstring source, gstring sub, gstring rep) {
  gstring dst = g_replace(source, sub, rep);
  g_free(source);
  return dst;
}
gstring g_replace(gstring source, gstring sub, gstring rep) {
  gstring result;
  gstring pc1, pc2, pc3;
  int isource, isub, irep;
  isub = g_len(sub);
  irep = g_len(rep);
  isource = g_len(source);
  if (0 == *sub)
    return g_dup(source);
  result = (gstring)g_malloc(
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

gstring g_concat(gcstring string1, ...) {
  guint l;
  va_list args;
  gstring s;
  gstring concat;

  g_return_val_if_fail(string1, NULL);

  l = 1 + g_len(string1);
  va_start(args, string1);
  s = va_arg(args, gstring);
  while (s) {
    l += g_len(s);
    s = va_arg(args, gstring);
  }
  va_end(args);

  concat = g_malloc(l);
  concat[0] = 0;

  strcat(concat, string1);
  va_start(args, string1);
  s = va_arg(args, gstring);
  while (s) {
    strcat(concat, s);
    s = va_arg(args, gstring);
  }
  va_end(args);

  return concat;
}

void g_down(gstring string) {
  register gstring s;
  g_return_if_fail(string);
  s = string;
  while (*s) {
    *s = tolower(*s);
    s++;
  }
}

void g_up(gstring string) {
  register gstring s;
  g_return_if_fail(string);
  s = string;
  while (*s) {
    *s = toupper(*s);
    s++;
  }
}

void g_reverse(gstring string) {
  g_return_if_fail(string != NULL);
  if (*string) {
    register gstring h, t;
    h = string;
    t = string + g_len(string) - 1;
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
gint g_cmp(gcstring s1, gcstring s2) {
#ifdef HAVE_STRCASECMP
  return strcasecmp(s1, s2);
#else
  gint c1, c2;

  if (s1 == NULL || s2 == NULL)
    return FALSE;

  while (*s1 && *s2) {
    c1 = isupper((guchar)*s1) ? tolower((guchar)*s1) : *s1;
    c2 = isupper((guchar)*s2) ? tolower((guchar)*s2) : *s2;
    if (c1 != c2)
      return (c1 - c2);
    s1++;
    s2++;
  }

  return (((gint)(guchar)*s1) - ((gint)(guchar)*s2));
#endif
}

void g_delimit(gstring string, gcstring delimiters, gchar new_delim) {
  register gstring c;

  g_return_if_fail(string != NULL);

  if (!delimiters)
    delimiters = G_STR_DELIMITERS;

  for (c = string; *c; c++) {
    if (strchr(delimiters, *c))
      *c = new_delim;
  }
}

gbool g_start_with(gstring string, gstring sub) {
  gint i, len = g_len(sub);
  if ((gint)g_len(string) < len)
    return FALSE;
  for (i = 0; i < len; i++) {
    if (string[i] != sub[i])
      return FALSE;
  }
  return TRUE;
}
gbool g_end_with(gstring string, gstring sub) {
  gint i, len = g_len(sub);
  gint lensrc = g_len(string);
  if (lensrc < len)
    return FALSE;
  lensrc -= len;
  for (i = 0; i < len; i++) {
    if (string[lensrc + i] != sub[i])
      return FALSE;
  }
  return TRUE;
}
gint g_index_of(gstring fstring, gstring str, gint index) {
  char *l;
  if (index < 0)
    index = 0;
  else if (index >= (gint)g_len(fstring))
    return -1;
  l = strstr(fstring + index, str);
  return l == NULL ? -1 : l - fstring;
}
gint g_last_index_of(gstring fstring, gstring str) {
  gint pos = -1;
  gint posnext;
  gint len;
  if (str == NULL)
    return -1;
  len = g_len(str);
  if (len == 0)
    return -1;
  do {
    posnext = g_index_of(fstring, str, pos);
    if (posnext == -1)
      return pos == -1 ? -1 : pos - len;
    pos = posnext + len;
  } while (TRUE);
}
gstring g_substring(gstring fstring, gint st, gint len) {
  gint i;
  gstring r;
  gint fslen = g_len(fstring);
  if (len <= 0)
    len = fslen - st;
  if (st + len > fslen)
    len = fslen - st;
  r = g_malloc(len + 1);
  for (i = 0; i < len; i++)
    r[i] = fstring[st + i];
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
gint g_parse_num(gstring fstring, gchar chend, gint base) {
  gstring s = fstring;
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

gstring g_trim(gstring str) {
  gint i;
  gint len;
  gstring result;
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
