/* *
 * * This file is part of NetEmbryo
 *
 * Copyright (C) 2007 by LScube team <team@streaming.polito.it>
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
#include "wsocket-internal.h"
#include <string.h>

#ifndef WIN32
#include <netinet/in.h>
#endif

const char *neb_sock_remote_host(Sock *s)
{
    char str[128];

    if(!(s->remote_host)) {
        sock_ntop_host((struct sockaddr *)&(s->remote_stg),str,sizeof(str));
        s->remote_host = strdup(str);
    }
    return s->remote_host;
}

const char *neb_sock_local_host(Sock *s)
{
    char str[128]; /*Unix domain is largest*/

    if(!(s->local_host)) {
        sock_ntop_host((struct sockaddr *)&(s->local_stg),str,sizeof(str));
        s->local_host = strdup(str);
    }
    return s->local_host;
}

in_port_t neb_sock_remote_port(Sock *s)
{
    int32_t port;
    if(!(s->remote_port)) {
        if((port = sock_get_port((struct sockaddr *)&(s->remote_stg))) < 0)
            return 0;
        s->remote_port = ntohs(port);
    }
    return s->remote_port;
}

in_port_t neb_sock_local_port(Sock *s)
{
    int32_t port;

    if(!(s->local_port)) {
        if((port = sock_get_port((struct sockaddr *)&(s->local_stg))) < 0)
            return 0;
        s->local_port = ntohs(port);
    }
    return s->local_port;
}
