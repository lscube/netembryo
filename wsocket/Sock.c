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

#include "netembryo/wsocket.h"
#include "wsocket-internal.h"

#ifndef WIN32
# include <sys/ioctl.h>
#endif

#include "socket.c"

/**
 * Tell if an address is multicast
 */

static int is_multicast_address(const struct sockaddr *stg, sa_family_t family)
{
    switch (family) {
    case AF_INET: {
        struct in_addr *in;
        in = &(((struct sockaddr_in *) stg)->sin_addr);
        return IN_IS_ADDR_MULTICAST(ntohl(in->s_addr));
    }
#ifdef  IPV6
    case AF_INET6: {
        struct in6_addr *in6;
        in6 = &(((struct sockaddr_in6 *) stg)->sin6_addr);
        return IN6_IS_ADDR_MULTICAST(in6);
    }
#endif
#ifdef  AF_UNIX
    case AF_UNIX:
        return -1;
#endif
#ifdef  HAVE_SOCKADDR_DL_STRUCT
    case AF_LINK:
        return -1;
#endif
    default:
        return -1;
    }
}

Sock * neb_sock_accept(Sock *s)
{
    int res = -1;
    char remote_host[128]; /*Unix Domain is largest*/
    char local_host[128]; /*Unix Domain is largest*/
    int remote_port = -1;
    int local_port = -1;
    Sock *new_s = NULL;
    struct sockaddr *sa_p = NULL;
    socklen_t sa_len = 0;

    if (!s)
        return NULL;

    if ((res = _neb_sock_accept(s->fd)) < 0) {
        neb_log(NEB_LOG_DEBUG, "System error in _neb_sock_accept().\n");
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
    new_s->flags = s->flags;

    sa_p = (struct sockaddr *) &(new_s->remote_stg);
    sa_len = sizeof(struct sockaddr_storage);

    if(getpeername(res, sa_p, &sa_len))
        {
            neb_log(NEB_LOG_DEBUG,
                    "Unable to get remote address in neb_sock_accept().\n");
            neb_sock_close(new_s);
            return NULL;
        }

    if(!sock_ntop_host(sa_p, remote_host, sizeof(remote_host)))
        memset(remote_host, 0, sizeof(remote_host));

    if (!(new_s->remote_host = strdup(remote_host))) {
        neb_log(NEB_LOG_FATAL,
                "Unable to allocate remote host in neb_sock_accept().\n");
        neb_sock_close(new_s);
        return NULL;
    }

    remote_port = sock_get_port(sa_p);
    if(remote_port < 0) {
        neb_log(NEB_LOG_DEBUG, "Unable to get remote port in neb_sock_accept().\n");
        neb_sock_close(new_s);
        return NULL;
    }
    else
        new_s->remote_port = ntohs(remote_port);

    sa_p = (struct sockaddr *) &(new_s->remote_stg);
    sa_len = sizeof(struct sockaddr_storage);

    if(getsockname(res, sa_p, &sa_len))
        {
            neb_log(NEB_LOG_DEBUG, "Unable to get remote port in neb_sock_accept().\n");
            neb_sock_close(new_s);
            return NULL;
        }

    if(!sock_ntop_host(sa_p, local_host, sizeof(local_host)))
        memset(local_host, 0, sizeof(local_host));

    if (!(new_s->local_host = strdup(local_host))) {
        neb_log(NEB_LOG_FATAL,
                "Unable to allocate local host in neb_sock_accept().\n");
        neb_sock_close(new_s);
        return NULL;
    }

    local_port = sock_get_port(sa_p);
    if(local_port < 0) {
        neb_log(NEB_LOG_DEBUG, "Unable to get local port in neb_sock_accept().\n");
        neb_sock_close(new_s);
        return NULL;
    }
    else
        new_s->local_port = ntohs(local_port);

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
    struct sockaddr *sa_p;
    socklen_t sa_len;
    char local_host[128];
    int local_port;

    if(sock) {
        sockfd = sock->fd;
    }

    if (_neb_sock_bind(host, port, &sockfd, socktype)) {
        neb_log(NEB_LOG_DEBUG, "Error in low level sock_bind().\n");
        return NULL;
    }

    if (!(s = calloc(1, sizeof(Sock)))) {
        neb_log(NEB_LOG_FATAL,
                "Unable to allocate a Sock struct in neb_sock_bind().\n");
        close(sockfd);
        return NULL;
    }

    s->fd = sockfd;
    s->socktype = socktype;
    s->flags = 0;

    sa_p = (struct sockaddr *)&(s->local_stg);
    sa_len = sizeof(struct sockaddr_storage);

    if(getsockname(s->fd, sa_p, &sa_len) < 0) {
        neb_sock_close(s);
        return NULL;
    }

    if(!sock_ntop_host(sa_p, local_host, sizeof(local_host)))
        memset(local_host, 0, sizeof(local_host));

    if (!(s->local_host = strdup(local_host))) {
        neb_log(NEB_LOG_FATAL,
                "Unable to allocate local host in neb_sock_bind().\n");
        neb_sock_close(s);
        return NULL;
    }

    local_port = sock_get_port(sa_p);

    if(local_port < 0) {
        neb_log(NEB_LOG_DEBUG, "Unable to get local port in neb_sock_bind().\n");
        neb_sock_close(s);
        return NULL;
    } else
        s->local_port = ntohs(local_port);

    neb_log(NEB_LOG_DEBUG,
            "Socket bound with addr=\"%s\" and port=\"%u\".\n",
            s->local_host, s->local_port);

    if(is_multicast_address(sa_p, s->local_stg.ss_family)) {
        if(mcast_join(s->fd, sa_p)) {
            neb_sock_close(s);
            return NULL;
        }
        s->flags |= IS_MULTICAST;
    }

    return s;
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

    if(s->flags & IS_MULTICAST) {
        if (s->remote_host)
            mcast_leave(s->fd,(struct sockaddr *) &(s->remote_stg));
        else
            mcast_leave(s->fd,(struct sockaddr *) &(s->local_stg));
    }

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
    char remote_host[128]; /*Unix Domain is largest*/
    char local_host[128]; /*Unix Domain is largest*/
    int sockfd = -1;
    struct sockaddr *sa_p = NULL;
    socklen_t sa_len = 0;
    int local_port;
    int remote_port;

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
    } else if (!(s = calloc(1, sizeof(Sock)))) {
        neb_log(NEB_LOG_FATAL, "Unable to allocate a Sock struct in neb_sock_connect().\n");
        close (sockfd);
        return NULL;
    }

    s->fd = sockfd;
    s->socktype = socktype;
    s->flags = 0;

    sa_p = (struct sockaddr *) &(s->local_stg);
    sa_len = sizeof(struct sockaddr_storage);

    if(getsockname(s->fd, sa_p, &sa_len))
        {
            neb_log(NEB_LOG_DEBUG,
                    "Unable to get remote port in neb_sock_connect().\n");
            neb_sock_close(s);
            return NULL;
        }

    if(!sock_ntop_host(sa_p, local_host, sizeof(local_host)))
        memset(local_host, 0, sizeof(local_host));

    if (!(s->local_host = strdup(local_host))) {
        neb_log(NEB_LOG_FATAL,
                "Unable to allocate local host in neb_sock_connect().\n");
        neb_sock_close(s);
        return NULL;
    }

    local_port = sock_get_port(sa_p);

    if(local_port < 0) {
        neb_log(NEB_LOG_DEBUG,
                "Unable to get local port in neb_sock_connect().\n");
        neb_sock_close(s);
        return NULL;
    } else
        s->local_port = ntohs(local_port);

    sa_p = (struct sockaddr *) &(s->remote_stg);
    sa_len = sizeof(struct sockaddr_storage);

    if(getpeername(s->fd, sa_p, &sa_len))
        {
            neb_log(NEB_LOG_DEBUG,
                    "Unable to get remote address in neb_sock_connect().\n");
            neb_sock_close(s);
            return NULL;
        }

    if(!sock_ntop_host(sa_p, remote_host, sizeof(remote_host)))
        memset(remote_host, 0, sizeof(remote_host));

    if (!(s->remote_host = strdup(remote_host))) {
        neb_log(NEB_LOG_FATAL,
                "Unable to allocate remote host in neb_sock_connect().\n");
        neb_sock_close(s);
        return NULL;
    }

    remote_port = sock_get_port(sa_p);
    if(remote_port < 0) {
        neb_log(NEB_LOG_DEBUG,
                "Unable to get remote port in neb_sock_connect().\n");
        neb_sock_close(s);
        return NULL;
    } else
        s->remote_port = ntohs(remote_port);

    neb_log(NEB_LOG_DEBUG,
            "Socket connected between local=\"%s\":%u and remote=\"%s\":%u.\n",
            s->local_host, s->local_port, s->remote_host, s->remote_port);

    if(is_multicast_address(sa_p, s->remote_stg.ss_family)) {
        //fprintf(stderr,"IS MULTICAST\n");
        if(mcast_join(s->fd, sa_p)!=0) {
            neb_sock_close(s);
            return NULL;
        }
        s->flags |= IS_MULTICAST;
    }

    return s;
}

int neb_sock_listen(Sock *s, int backlog)
{
    if (!s)
        return -1;
    return listen(s->fd, backlog);
}

int neb_sock_read(Sock *s, void *buffer, int nbytes, void *protodata, int flags)
{

    socklen_t sa_len = sizeof(struct sockaddr_storage);

    if(!s)
        return -1;

    switch(s->socktype) {
    case UDP:
        if (!protodata) {
            protodata = &s->remote_stg;
        }
        return recvfrom(s->fd, buffer, nbytes, flags,
                        (struct sockaddr *) protodata, &sa_len);
        break;
    case TCP:
        return recv(s->fd, buffer, nbytes, flags);
        break;
    case SCTP:
#ifdef ENABLE_SCTP
        if (!protodata) {
            return -1;
        }
        return sctp_recvmsg(s->fd, buffer, nbytes, NULL,
                            0, (struct sctp_sndrcvinfo *) protodata, &flags);
        /* flags is discarted: usage may include detection of
         * incomplete packet due to small buffer or detection of
         * notification data (this last should be probably
         * handled inside netembryo).
         */
#endif
        break;
    case LOCAL:
        return recv(s->fd, buffer, nbytes, flags);
        break;
    default:
        break;
    }

    return -1;
}

/**
 * Convenience function to alias to tcp and local cases.
 *
 */
int neb_sock_write(Sock *s,
                   const void *buffer,
                   size_t nbytes,
                   int flags) {
    assert(s != NULL);

    switch(s->socktype) {
    case TCP:
    case LOCAL:
        return send(s->fd, buffer, nbytes, flags);
    case UDP:
        return sendto(s->fd, buffer, nbytes, flags,
                      (struct sockaddr*)&s->remote_stg,
                      sizeof(struct sockaddr_storage));

#ifdef ENABLE_SCTP
    case SCTP:
        return neb_sock_write_stream(s,
                                     buffer, nbytes,
                                     flags, 0);
#endif

    default:
        assert(1 == 0);
        return -1;
    }
}

#ifdef ENABLE_SCTP
int neb_sock_write_stream(Sock *s,
                          const void *buffer, size_t nbytes,
                          int flags, int stream)
{
    const struct sctp_sndrcvinfo sctp_info = {
        .sinfo_stream = stream
    };

    assert(s != NULL && s->socktype == SCTP);

    return sctp_send(s->fd,
                     buffer, nbytes,
                     &sctp_info, flags);
}
#endif
