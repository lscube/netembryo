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

#include <config.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

#if HAVE_INET_NTOP
# include <arpa/inet.h>
#endif

int neb_sock_parse_address(const struct sockaddr *sa, char **host_p, in_port_t *port_p)
{
    char host[128] = { 0, };
    switch (sa->sa_family) {
    case AF_INET:
        {
            struct sockaddr_in *sin = (struct sockaddr_in *) sa;
#ifdef HAVE_INET_NTOP
            if ( inet_ntop(AF_INET, &(sin->sin_addr), host, sizeof(host)-1) == NULL )
                goto error;
#else
            if ( getnameinfo(sa, sizeof(struct sockaddr_in), host, sizeof(host)-1,
                             NULL, 0, NI_NUMERICHOST) )
                goto error;
#endif

            *port_p = ntohs(sin->sin_port);
        }
        break;
    case AF_INET6:
        {
            struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *) sa;
            int a = 0;
            char *tmp = host;

#ifdef HAVE_INET_NTOP
            if ( inet_ntop(AF_INET6, &(sin6->sin6_addr), host, sizeof(host)-1) == NULL )
                goto error;
#else
            if ( getnameinfo(sa, sizeof(struct sockaddr_in6), host, sizeof(host)-1,
                             NULL, 0, NI_NUMERICHOST) )
                goto error;
#endif

            while ((tmp = strchr(tmp, '.'))) {
                a++;
                tmp++;
            }

            if (a == 3) {
                if (!strncmp(host, "::ffff:", 7)) {
                    //this is an IPv4 address mapped in IPv6 address space
                    memmove (host, &host[7], strlen(host) - 6); // one char more for trailing NUL char
                } else {
                    //this is an IPv6 address containg an IPv4 address (like ::127.0.0.1)
                    memmove (host, &host[2], strlen(host) - 1);
                }
            }

            *port_p = ntohs(sin6->sin6_port);
        }
        break;
    default:
        goto error;
    }

    *host_p = strdup(host);
    return 0;

 error:
    *host_p = NULL;
    *port_p = 0;
    return -1;
}

