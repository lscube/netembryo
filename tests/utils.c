/* *
 * * This file is part of NetEmbryo
 *
 * Copyright (C) 2008 by LScube team <team@streaming.polito.it>
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

#include "netembryo/wsocket.h"

void neb_vlog(NebLogLevel level, const char *fmt, va_list args)
{
    static const char *const prefixes[] = {
        [NEB_LOG_FATAL] = "fatal error",
        [NEB_LOG_WARN] = "warning",
        [NEB_LOG_DEBUG] = "debug",
        [NEB_LOG_VERBOSE] = "verbose debug",
        [NEB_LOG_INFO] = "info",
        [NEB_LOG_UNKNOWN] = "unk",
    };

    /* if the level is out of our field, make it print unknown… */
    if ( level >= sizeof(prefixes)/sizeof(prefixes[0]) ) {
        fprintf(stderr, "level %d exceeds limits", level);
        level = sizeof(prefixes)/sizeof(prefixes[0])-1;
    }

    fprintf(stderr, "[%s] ", prefixes[level]);
    vfprintf(stderr, fmt, args);
}

