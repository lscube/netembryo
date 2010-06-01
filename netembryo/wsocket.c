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

static int _neb_sock_remote_addr(Sock *s)
{
    struct sockaddr *sa_p = (struct sockaddr *) &(s->remote_stg);
    socklen_t sa_len = sizeof(struct sockaddr_storage);

    if ( getpeername(s->fd, sa_p, &sa_len) )
        return -1;

    neb_sock_parse_address(sa_p, &s->remote_host, &s->remote_port);

    return 0;
}

static int _neb_sock_local_addr(Sock *s)
{
    struct sockaddr *sa_p = (struct sockaddr *) &(s->local_stg);
    socklen_t sa_len = sizeof(struct sockaddr_storage);

    if ( getsockname(s->fd, sa_p, &sa_len) )
        return -1;

    neb_sock_parse_address(sa_p, &s->local_host, &s->local_port);

    return 0;
}

#include "wsocket-lowlevel.c"

Sock * neb_sock_accept(Sock *s)
{
    int res = -1;
    Sock *new_s = NULL;

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

    if ( _neb_sock_remote_addr(new_s) ) {
        neb_log(NEB_LOG_DEBUG,
                "Unable to get remote address in neb_sock_accept().\n");
        neb_sock_close(new_s);
        return NULL;
    }

    if ( _neb_sock_local_addr(new_s) ) {
        neb_log(NEB_LOG_DEBUG,
                "Unable to get local address in neb_sock_accept().\n");
        neb_sock_close(new_s);
        return NULL;
    }

    neb_log(NEB_LOG_DEBUG, "Socket accepted between local=\"%s\":%u and "
            "remote=\"%s\":%u.\n", new_s->local_host, new_s->local_port,
            new_s->remote_host, new_s->remote_port);

    return new_s;
}

Sock * neb_sock_bind(const char const *host, const char const *port, Sock *sock,
                     sock_type socktype)
{

    Sock *s = NULL;
    int sockfd = -1;

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

    if ( _neb_sock_local_addr(s) )
        goto error;

    neb_log(NEB_LOG_DEBUG,
            "Socket bound with addr=\"%s\" and port=\"%u\".\n",
            s->local_host, s->local_port);

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

    free(s->remote_host);
    free(s->local_host);
    free(s);

    return res;
}

Sock * neb_sock_connect(const char const *host, const char const *port,
                         Sock *binded, sock_type socktype)
{
    Sock *s;
    int sockfd = -1;

    if(binded) {
        sockfd = binded->fd;
    }

    if (_neb_sock_connect(host, port, &sockfd, socktype)) {
        neb_log(NEB_LOG_DEBUG, "neb_sock_connect() failure.\n");
        return NULL;
    }

    if (binded) {
        s = binded;
        free(s->local_host);
        s->local_host = NULL;
        free(s->remote_host);
        s->remote_host = NULL;
    } else if ( (s = calloc(1, sizeof(Sock))) == NULL )
        goto error;

    s->fd = sockfd;
    s->socktype = socktype;

    if ( _neb_sock_local_addr(s) )
        goto error;

    if ( _neb_sock_remote_addr(s) ) {
        neb_log(NEB_LOG_DEBUG,
                "Unable to get remote address in neb_sock_connect().\n");
        goto error;
    }

    neb_log(NEB_LOG_DEBUG,
            "Socket connected between local=\"%s\":%u and remote=\"%s\":%u.\n",
            s->local_host, s->local_port, s->remote_host, s->remote_port);

    return s;
 error:
    if ( s != NULL )
        neb_sock_close(s);
    else
        close(sockfd);
    return NULL;
}
