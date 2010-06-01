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
 * */

/**
 * @file Sock.c
 * High level inteface
 */

#include <config.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

#include <assert.h>

#ifdef ENABLE_SCTP
#include <netinet/sctp.h>
#endif


#ifndef WIN32
# include <sys/ioctl.h>
#endif

#include "netembryo/wsocket.h"
#include "wsocket-internal.h"

#include "wsocket-lowlevel.c"

Sock * neb_sock_accept(Sock *s)
{
    int res = -1;
    Sock *new_s = NULL;
    struct sockaddr *sa_p = NULL;
    socklen_t sa_len = sizeof(struct sockaddr_storage);

    if (!s)
        return NULL;

    if ((res = accept(s->fd, NULL, 0)) < 0) {
        neb_log(NEB_LOG_DEBUG, "error in accept().\n");
        return NULL;
    }

    if (!(new_s = calloc(1, sizeof(Sock)))) {
        neb_log(NEB_LOG_FATAL,
                "Unable to allocate a Sock struct in neb_sock_accept().\n");
        close(res);
        return NULL;
    }

    new_s->fd = res;
    new_s->socktype = s->socktype;

    /* Avoid fetching it again, we know what it is already! */
    memcpy(&new_s->local_stg, &s->local_stg, sizeof(struct sockaddr_storage));

    sa_p = (struct sockaddr *) &(new_s->remote_stg);

    if ( getpeername(new_s->fd, sa_p, &sa_len) ) {
        neb_log(NEB_LOG_DEBUG,
                "Unable to get remote address in neb_sock_accept().\n");
        neb_sock_close(new_s);
        return NULL;
    }

    return new_s;
}

Sock * neb_sock_bind(const char const *host, const char const *port, Sock *sock,
                     sock_type socktype)
{

    Sock *s = NULL;
    int sockfd = -1;
    struct sockaddr *sa_p = NULL;
    socklen_t sa_len = sizeof(struct sockaddr_storage);

    if(sock) {
        sockfd = sock->fd;
    }

    if (_neb_sock_bind(host, port, &sockfd, socktype)) {
        neb_log(NEB_LOG_DEBUG, "Error in low level sock_bind().\n");
        return NULL;
    }

    if ( (s = calloc(1, sizeof(Sock))) == NULL )
        goto error;

    s->fd = sockfd;
    s->socktype = socktype;

    sa_p = (struct sockaddr *) &(s->local_stg);

    if ( getsockname(s->fd, sa_p, &sa_len) )
        goto error;

    return s;

 error:
    if ( s != NULL )
        neb_sock_close(s);
    else
        close(sockfd);
    return NULL;
}

/**
 * Close an existing socket.
 * @param s Existing socket.
 */

int neb_sock_close(Sock *s)
{
    int res;

    if (!s)
        return -1;

    res = close(s->fd);

    free(s);

    return res;
}
