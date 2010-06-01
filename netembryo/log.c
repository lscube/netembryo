/* *
 * * This file is part of NetEmbryo
 *
 * Copyright (C) 2010 by LScube team <team@lscube.org>
 * See AUTHORS for more details
 *
 * NetEmbryo is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * NetEmbryo is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with NetEmbryo; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * */

#include <config.h>

#include "netembryo/wsocket.h"
#include "netembryo/wsocket-internal.h"

void neb_log(NebLogLevel level, const char *fmt, ...)
{
    va_list vl;
    va_start(vl, fmt);
    neb_vlog(level, fmt, vl);
    va_end(vl);
}

void neb_vlog(NebLogLevel level, const char *fmt, va_list args)
    __attribute__((weak));
void neb_vlog(NebLogLevel level, const char *fmt, va_list args)
{
    static const char *const prefixes[] = {
        [NEB_LOG_FATAL] = "fatal error",
        [NEB_LOG_WARN] = "warning",
#ifdef NEB_DEBUG_VERBOSE
        [NEB_LOG_DEBUG] = "debug",
        [NEB_LOG_VERBOSE] = "verbose debug",
#endif
        [NEB_LOG_INFO] = "info",
        [NEB_LOG_UNKNOWN] = "unk",
    };

    /* if the level is out of our field, make it print unknown… */
    if ( level >= sizeof(prefixes)/sizeof(prefixes[0]) )
        level = sizeof(prefixes)/sizeof(prefixes[0])-1;

    if ( prefixes[level] ) {
        fprintf(stderr, "[%s] ", prefixes[level]);
        vfprintf(stderr, fmt, args);
    }
}

