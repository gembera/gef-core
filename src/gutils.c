/*
 * Copyright (c) 2023 Gembera Team
 * https://github.com/gembera
 * License: MIT
 */

#include "glib.h"

// from : include/linux/time.h
// static gulong gauss_mktime (guint year, guint mon,
// 							guint day, guint hour,
// 							guint min, guint sec)
// {
// 	if (0 >= (int)(mon -= 2)) {    /* 1..12 -> 11,12,1..10 */
// 		mon += 12;      /* Puts Feb last since it has leap day */
// 		year -= 1;
// 	}

// 	return (((
// 		(gulong) (year/4 - year/100 + year/400 + 367*mon/12 + day) +
// 		year*365 - 719499
// 		)*24 + hour /* now have hours */
// 		)*60 + min /* now have minutes */
// 		)*60 + sec; /* finally seconds */
// }

// gulong g_mktime(GDateTime* time) {
// 	gulong ret = 0;
// 	g_return_val_if_fail(time != NULL, ret);
// 	ret = gauss_mktime(time->year, time->mon, time->day, time->hour,
// time->min, time->sec);
// 	//g_log_info("g_mktime %ld", ret);
// 	return ret;
// }

guint g_ptr_hash(gconstpointer v) { return g_pointer_to_unum(v); }

gint g_ptr_equal(gconstpointer v, gconstpointer v2) { return v == v2; }

static char encoding_table[] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
    'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
    'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'};
static char *decoding_table = NULL;
static int mod_table[] = {0, 2, 1};

gstring g_base64_encode(const gstring input, gint input_length,
                        gint *output_length) {
  char *encoded_data;
  const guint8 *data = (const guint8 *)input;
  int i, j;
  *output_length = 4 * ((input_length + 2) / 3);

  encoded_data = malloc(*output_length + 1);
  if (encoded_data == NULL)
    return NULL;

  for (i = 0, j = 0; i < input_length;) {

    guint32 octet_a = i < input_length ? data[i++] : 0;
    guint32 octet_b = i < input_length ? data[i++] : 0;
    guint32 octet_c = i < input_length ? data[i++] : 0;

    guint32 triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

    encoded_data[j++] = encoding_table[(triple >> 3 * 6) & 0x3F];
    encoded_data[j++] = encoding_table[(triple >> 2 * 6) & 0x3F];
    encoded_data[j++] = encoding_table[(triple >> 1 * 6) & 0x3F];
    encoded_data[j++] = encoding_table[(triple >> 0 * 6) & 0x3F];
  }

  for (i = 0; i < mod_table[input_length % 3]; i++)
    encoded_data[*output_length - 1 - i] = '=';

  encoded_data[*output_length] = '\0';
  return encoded_data;
}

void build_decoding_table() {
  int i;
  decoding_table = malloc(256);

  for (i = 0; i < 0x40; i++)
    decoding_table[encoding_table[i]] = i;
}

void base64_cleanup() { free(decoding_table); }

gstring g_base64_decode(const gstring data, gint input_length,
                        gint *output_length) {
  char *decoded_data;
  int i, j;
  if (decoding_table == NULL)
    build_decoding_table();

  if (input_length % 4 != 0)
    return NULL;

  *output_length = input_length / 4 * 3;
  if (data[input_length - 1] == '=')
    (*output_length)--;
  if (data[input_length - 2] == '=')
    (*output_length)--;

  decoded_data = malloc(*output_length + 1);
  if (decoded_data == NULL)
    return NULL;

  for (i = 0, j = 0; i < input_length;) {

    guint32 sextet_a = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
    guint32 sextet_b = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
    guint32 sextet_c = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
    guint32 sextet_d = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];

    guint32 triple = (sextet_a << 3 * 6) + (sextet_b << 2 * 6) +
                     (sextet_c << 1 * 6) + (sextet_d << 0 * 6);

    if (j < *output_length)
      decoded_data[j++] = (triple >> 2 * 8) & 0xFF;
    if (j < *output_length)
      decoded_data[j++] = (triple >> 1 * 8) & 0xFF;
    if (j < *output_length)
      decoded_data[j++] = (triple >> 0 * 8) & 0xFF;
  }

  decoded_data[*output_length] = '\0';

  return decoded_data;
}

#include <time.h>
gint g_get_tick_count() { return time(NULL); }

#include "stdlib.h"
static gint rand_init = 0;
gint g_random(gint max) {
  if (!rand_init) {
    rand_init = 1;
    srand((unsigned int)g_get_tick_count());
  }
  return rand() % max;
  return 0;
}
