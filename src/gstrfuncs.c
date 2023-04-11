/*
 * Copyright (c) 2023 Gembera Team
 * https://github.com/gembera
 * License: MIT
 */

#include "glib.h"

void g_strcpy(gchar *dst, gchar *src) { strcpy(dst, src); }
gchar *g_strdup(const gchar *str) {
  gchar *new_str;

  new_str = NULL;
  if (str) {
    new_str = g_malloc(strlen(str) + 1);
    strncpy(new_str, str, strlen(str));
  }

  return new_str;
}
gchar *g_strreplaceandfree(gchar *source, gchar *sub, gchar *rep) {
  gstring dst = g_strreplace(source, sub, rep);
  g_free(source);
  return dst;
}
gchar *g_strreplace(gchar *source, gchar *sub, gchar *rep) {
  gchar *result;
  gchar *pc1, *pc2, *pc3;
  int isource, isub, irep;
  isub = strlen(sub);
  irep = strlen(rep);
  isource = strlen(source);
  if (0 == *sub)
    return g_strdup(source);
  result = (gchar *)g_malloc(
      (irep > isub ? irep * strlen(source) / isub + 1 : isource) + 1);
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

gchar *g_strconcat(const gchar *string1, ...) {
  guint l;
  va_list args;
  gchar *s;
  gchar *concat;

  g_return_val_if_fail(string1 != NULL, NULL);

  l = 1 + strlen(string1);
  va_start(args, string1);
  s = va_arg(args, gchar *);
  while (s) {
    l += strlen(s);
    s = va_arg(args, gchar *);
  }
  va_end(args);

  concat = g_new(gchar, l);
  concat[0] = 0;

  strcat(concat, string1);
  va_start(args, string1);
  s = va_arg(args, gchar *);
  while (s) {
    strcat(concat, s);
    s = va_arg(args, gchar *);
  }
  va_end(args);

  return concat;
}

void g_strdown(gchar *string) {
  register gchar *s;

  g_return_if_fail(string != NULL);

  s = string;

  while (*s) {
    *s = tolower(*s);
    s++;
  }
}

void g_strup(gchar *string) {
  register gchar *s;

  g_return_if_fail(string != NULL);

  s = string;

  while (*s) {
    *s = toupper(*s);
    s++;
  }
}

void g_strreverse(gchar *string) {
  g_return_if_fail(string != NULL);

  if (*string) {
    register gchar *h, *t;

    h = string;
    t = string + strlen(string) - 1;

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
gint g_strcasecmp(const gchar *s1, const gchar *s2) {
#ifdef HAVE_STRCASECMP
  return strcasecmp(s1, s2);
#else
  gint c1, c2;

  if (s1 == NULL || s2 == NULL)
    return FALSE;

  while (*s1 && *s2) {
    /* According to A. Cox, some platforms have islower's that
     * don't work right on non-uppercase
     */
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

void g_strdelimit(gchar *string, const gchar *delimiters, gchar new_delim) {
  register gchar *c;

  g_return_if_fail(string != NULL);

  if (!delimiters)
    delimiters = G_STR_DELIMITERS;

  for (c = string; *c; c++) {
    if (strchr(delimiters, *c))
      *c = new_delim;
  }
}

gboolean g_strstartwith(gchar *string, gchar *sub) {
  gint i, len = strlen(sub);
  if ((gint)strlen(string) < len)
    return FALSE;
  for (i = 0; i < len; i++) {
    if (string[i] != sub[i])
      return FALSE;
  }
  return TRUE;
}
gboolean g_strendwith(gchar *string, gchar *sub) {
  gint i, len = strlen(sub);
  gint lensrc = strlen(string);
  if (lensrc < len)
    return FALSE;
  lensrc -= len;
  for (i = 0; i < len; i++) {
    if (string[lensrc + i] != sub[i])
      return FALSE;
  }
  return TRUE;
}
gint g_strindexof(gstring fstring, gchar *str, gint index) {
  char *l;
  if (index < 0)
    index = 0;
  else if (index >= (gint)strlen(fstring))
    return -1;
  l = strstr(fstring + index, str);
  return l == NULL ? -1 : l - fstring;
}
gint g_strlastindexof(gstring fstring, gchar *str) {
  gint pos = -1;
  gint posnext;
  gint len;
  if (str == NULL)
    return -1;
  len = strlen(str);
  if (len == 0)
    return -1;
  do {
    posnext = g_strindexof(fstring, str, pos);
    if (posnext == -1)
      return pos == -1 ? -1 : pos - len;
    pos = posnext + len;
  } while (TRUE);
}
gstring g_strsubstring(gstring fstring, gint st, gint len) {
  gint i;
  gstring r;
  gint fslen = strlen(fstring);
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

gint g_strgethex(gchar c) {
  if ('0' <= c && c <= '9')
    return c - '0';
  if ('A' <= c && c <= 'F')
    return c - 'A' + 10;
  if ('a' <= c && c <= 'f')
    return c - 'a' + 10;
  return -1;
}
gint g_strparseinteger(gstring fstring, gchar chend, gint base) {
  gchar *s = fstring;
  gboolean negate = FALSE;
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
    int i = g_strgethex(*s);
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

gchar *g_strtrim(gchar *str) {
  gint i;
  gint len;
  gchar *result;
  if (str == NULL)
    return NULL;
  len = strlen(str);
  for (i = len - 1; i >= 0 && g_is_space(str[i]); i--, len--)
    ;
  for (i = 0; i < len && g_is_space(str[i]); i++)
    ;
  result = g_malloc(len - i + 1);
  g_memmove(result, str + i, len - i);
  result[len - i] = '\0';
  return result;
}

gint g_str_equal(gconstpointer v, gconstpointer v2) {
  return strcmp((const gchar *)v, (const gchar *)v2) == 0;
}

static guint g_str_aphash_with_length(gconstpointer v, guint len) {
  const unsigned char *str = (unsigned char *)v;
  unsigned int hash = 0xAAAAAAAA;
  unsigned int i = 0;

  for (i = 0; i < len; str++, i++) {
    hash ^= ((i & 1) == 0)
                ? ((hash << 7) ^ ((unsigned int)*str) * (hash >> 3))
                : (~((hash << 11) + (((unsigned int)*str) ^ (hash >> 5))));
  }
  return hash;
}

guint g_str_aphash(gconstpointer v) {
  const char *str = (char *)v;
  guint len = strlen(str);
  return g_str_aphash_with_length(v, len);
}
/* a char* hash function from ASU */
guint g_str_hash(gconstpointer v) {
  const char *s = (char *)v;
  const char *p;
  guint h = 0, g;

  for (p = s; *p != '\0'; p += 1) {
    h = (h << 4) + *p;
    if ((g = h & 0xf0000000)) {
      h = h ^ (g >> 24);
      h = h ^ g;
    }
  }

  return h /* % M */;
}

gint g_str_iequal(gconstpointer v, gconstpointer v2) {
  return g_strcasecmp((const gchar *)v, (const gchar *)v2) == 0;
}

/* a char* hash function from ASU */
guint g_str_ihash(gconstpointer v) {
  const char *p;
  guint h = 0, g;

  for (p = (char *)v; *p != '\0'; p += 1) {
    h = (h << 4) + tolower(*p);
    if ((g = h & 0xf0000000)) {
      h = h ^ (g >> 24);
      h = h ^ g;
    }
  }

  return h /* % M */;
}

gboolean g_is_space(gwchar c) {
  return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}
