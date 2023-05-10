/*
 * Copyright (c) 2023 Gembera Team
 * https://github.com/gembera
 * License: MIT
 */

#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "stdarg.h"
#include "ctype.h"


/* Define to empty if the keyword does not work.  */
//#undef const

/* Define as __inline if that's what the C compiler calls it.  */
#define inline


/* The number of bytes in a char.  */
#define SIZEOF_CHAR 1

/* The number of bytes in a int.  */
#define SIZEOF_INT 4

/* The number of bytes in a long.  */
#define SIZEOF_LONG 8

/* The number of bytes in a long long.  */
#define SIZEOF_LONG_LONG 8

/* The number of bytes in a short.  */
#define SIZEOF_SHORT 2

/* Define if you have the memmove function.  */
#undef HAVE_MEMMOVE

/* Define if you have the strcasecmp function.  */
#undef HAVE_STRCASECMP

#define g_warning g_log_warn
#define g_error g_log_error

#define g_log_fatal printf
#define g_log_error printf
#define g_log_warn printf
#define g_log_info printf
#define g_log_debug printf

#define g_print printf
