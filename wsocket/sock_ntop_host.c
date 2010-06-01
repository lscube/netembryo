/* *
 * * This file is part of NetEmbryo
 *
 * Copyright (C) 2009 by LScube team <team@lscube.org>
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
 * this piece of code is taken from NeMeSI
 * */

#include "netembryo/wsocket.h"

#if !defined(WIN32) && defined(AF_UNIX)
# include <sys/un.h>
#endif

#include <string.h>
#include <assert.h>

#ifdef WIN32
static const char *inet_ntop(int af, const void *src, char *dst, unsigned cnt)
{
        if (af == AF_INET)
        {
                struct sockaddr_in in;
                memset(&in, 0, sizeof(in));
                in.sin_family = AF_INET;
                memcpy(&in.sin_addr, src, sizeof(struct in_addr));
                getnameinfo((struct sockaddr *)&in, sizeof(struct sockaddr_in), dst, cnt, NULL, 0, NI_NUMERICHOST);
                return dst;
        }
        else if (af == AF_INET6)
        {
                struct sockaddr_in6 in;
                memset(&in, 0, sizeof(in));
                in.sin6_family = AF_INET6;
                memcpy(&in.sin6_addr, src, sizeof(struct in_addr6));
                getnameinfo((struct sockaddr *)&in, sizeof(struct sockaddr_in6), dst, cnt, NULL, 0, NI_NUMERICHOST);
                return dst;
        }
        return NULL;
}

static int inet_pton(int af, const char *src, void *dst)
{
        struct addrinfo hints, *res, *ressave;

        memset(&hints, 0, sizeof(struct addrinfo));
        hints.ai_family = af;

        if (getaddrinfo(src, NULL, &hints, &res) != 0)
           return -1;

        ressave = res;

        while (res)
        {
                memcpy(dst, res->ai_addr, res->ai_addrlen);
                res = res->ai_next;
        }

        freeaddrinfo(ressave);
        return 0;
}
#endif


static void _neb_sock_ntop_host(const struct sockaddr *sa, char *str, size_t len)
{
    switch (sa->sa_family) {
    case AF_INET: {
        struct sockaddr_in    *sin = (struct sockaddr_in *) sa;
        if ( inet_ntop(AF_INET, &(sin->sin_addr), str, len) == NULL )
            goto error;
        return;
    }

#ifdef    IPV6
    case AF_INET6: {
        struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *) sa;
        int a = 0;
        char *tmp = str;

        if (inet_ntop(AF_INET6, &(sin6->sin6_addr), str, len) == NULL )
            goto error;

        while ((tmp = strchr(tmp, '.'))) {
            a++;
            tmp++;
        }
        if (a == 3) {
            if (!strncmp(str, "::ffff:", 7)) {
                //this is an IPv4 address mapped in IPv6 address space
                memmove (str, &str[7], strlen(str) - 6); // one char more for trailing NUL char
            } else {
                //this is an IPv6 address containg an IPv4 address (like ::127.0.0.1)
                memmove (str, &str[2], strlen(str) - 1);
            }
        }
        return;
    }
#endif

#if !defined(WIN32) && defined(AF_UNIX)
    case AF_UNIX: {
        struct sockaddr_un *unp = (struct sockaddr_un *) sa;

            /* OK to have no pathname bound to the socket: happens on
               every connect() unless client calls bind() first. */
        if (unp->sun_path[0] == '\0')
            strncpy(str, "(no pathname bound)", len);
        else
            strncpy(str, unp->sun_path, len);

        return;
    }
#endif

    default:
        break;
    }

 error:
    memset(str, 0, len);
}

/**
 * get the port from a sockaddr
 * @return the port or -1 on error
 */
static int _neb_sock_get_port(const struct sockaddr *sa)
{
    switch (sa->sa_family) {
    case AF_INET: {
        struct sockaddr_in *sin = (struct sockaddr_in *) sa;

        return(sin->sin_port);
    }

#ifdef IPV6
    case AF_INET6: {
        struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *) sa;

        return(sin6->sin6_port);
    }
#endif
    }

    assert(0);
}

static void _neb_sock_parse_address(const struct sockaddr *sa, char **host_p, in_port_t *port_p)
{
    char host[128];

    _neb_sock_ntop_host(sa, host, sizeof(host));

    *host_p = strdup(host);
    *port_p = ntohs(_neb_sock_get_port(sa));

    assert(*host_p != NULL);
}

int _neb_sock_remote_addr(Sock *s)
{
    struct sockaddr *sa_p = (struct sockaddr *) &(s->remote_stg);
    socklen_t sa_len = sizeof(struct sockaddr_storage);

    if ( getpeername(s->fd, sa_p, &sa_len) )
        return -1;

    _neb_sock_parse_address(sa_p, &s->remote_host, &s->remote_port);

    return 0;
}

int _neb_sock_local_addr(Sock *s)
{
    struct sockaddr_storage sa;
    socklen_t sa_len = sizeof(struct sockaddr_storage);

    if ( getsockname(s->fd, (struct sockaddr *)&sa, &sa_len) )
        return -1;

    _neb_sock_parse_address((struct sockaddr *)&sa, &s->local_host, &s->local_port);

    return 0;
}
