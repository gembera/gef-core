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

#define g_warning g_log_warn
#define g_error g_log_error

#define g_log_fatal printf
#define g_log_error printf
#define g_log_warn printf
#define g_log_info printf
#define g_log_debug printf

#define g_print printf
