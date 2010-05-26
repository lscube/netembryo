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


/**
 * Create a new socket accepting a new connection from a listening socket.
 * @param s Listening socket.
 * @return the newly allocated Sock
 */
Sock * Sock_accept(Sock *s)
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

    if ((res = sock_accept(s->fd)) < 0) {
        net_log(NET_LOG_DEBUG, "System error in sock_accept().\n");
        return NULL;
    }

    if (!(new_s = calloc(1, sizeof(Sock)))) {
        net_log(NET_LOG_FATAL,
                "Unable to allocate a Sock struct in Sock_accept().\n");
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
            net_log(NET_LOG_DEBUG,
                    "Unable to get remote address in Sock_accept().\n");
            Sock_close(new_s);
            return NULL;
        }

    if(!sock_ntop_host(sa_p, remote_host, sizeof(remote_host)))
        memset(remote_host, 0, sizeof(remote_host));

    if (!(new_s->remote_host = strdup(remote_host))) {
        net_log(NET_LOG_FATAL,
                "Unable to allocate remote host in Sock_accept().\n");
        Sock_close(new_s);
        return NULL;
    }

    remote_port = sock_get_port(sa_p);
    if(remote_port < 0) {
        net_log(NET_LOG_DEBUG, "Unable to get remote port in Sock_accept().\n");
        Sock_close(new_s);
        return NULL;
    }
    else
        new_s->remote_port = ntohs(remote_port);

    sa_p = (struct sockaddr *) &(new_s->remote_stg);
    sa_len = sizeof(struct sockaddr_storage);

    if(getsockname(res, sa_p, &sa_len))
        {
            net_log(NET_LOG_DEBUG, "Unable to get remote port in Sock_accept().\n");
            Sock_close(new_s);
            return NULL;
        }

    if(!sock_ntop_host(sa_p, local_host, sizeof(local_host)))
        memset(local_host, 0, sizeof(local_host));

    if (!(new_s->local_host = strdup(local_host))) {
        net_log(NET_LOG_FATAL,
                "Unable to allocate local host in Sock_accept().\n");
        Sock_close(new_s);
        return NULL;
    }

    local_port = sock_get_port(sa_p);
    if(local_port < 0) {
        net_log(NET_LOG_DEBUG, "Unable to get local port in Sock_accept().\n");
        Sock_close(new_s);
        return NULL;
    }
    else
        new_s->local_port = ntohs(local_port);

    net_log(NET_LOG_DEBUG, "Socket accepted between local=\"%s\":%u and "
            "remote=\"%s\":%u.\n", new_s->local_host, new_s->local_port,
            new_s->remote_host, new_s->remote_port);

    return new_s;
}

/**
 * Create a new socket and binds it to an address/port.
 * @param host Local address to be used by this socket, if NULL the socket will
 *        be bound to all interfaces.
 * @param port Local port to be used by this socket, if NULL a random port will
 *        be used.
 * @param sock Pointer to a pre-created socket
 * @param socktype The type of socket to be created.
 */

Sock * Sock_bind(char const *host, char const *port, Sock *sock,
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

    if (sock_bind(host, port, &sockfd, socktype)) {
        net_log(NET_LOG_DEBUG, "Error in low level sock_bind().\n");
        return NULL;
    }

    if (!(s = calloc(1, sizeof(Sock)))) {
        net_log(NET_LOG_FATAL,
                "Unable to allocate a Sock struct in Sock_bind().\n");
        close(sockfd);
        return NULL;
    }

    s->fd = sockfd;
    s->socktype = socktype;
    s->flags = 0;

    sa_p = (struct sockaddr *)&(s->local_stg);
    sa_len = sizeof(struct sockaddr_storage);

    if(getsockname(s->fd, sa_p, &sa_len) < 0) {
        Sock_close(s);
        return NULL;
    }

    if(!sock_ntop_host(sa_p, local_host, sizeof(local_host)))
        memset(local_host, 0, sizeof(local_host));

    if (!(s->local_host = strdup(local_host))) {
        net_log(NET_LOG_FATAL,
                "Unable to allocate local host in Sock_bind().\n");
        Sock_close(s);
        return NULL;
    }

    local_port = sock_get_port(sa_p);

    if(local_port < 0) {
        net_log(NET_LOG_DEBUG, "Unable to get local port in Sock_bind().\n");
        Sock_close(s);
        return NULL;
    } else
        s->local_port = ntohs(local_port);

    net_log(NET_LOG_DEBUG,
            "Socket bound with addr=\"%s\" and port=\"%u\".\n",
            s->local_host, s->local_port);

    if(is_multicast_address(sa_p, s->local_stg.ss_family)) {
        if(mcast_join(s->fd, sa_p)) {
            Sock_close(s);
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

int Sock_close(Sock *s)
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

/**
 * Establish a connection to a remote host.
 * @param host Remote host to connect to (may be a hostname).
 * @param port Remote port to connect to.
 * @param binded Pointer to a pre-binded socket (useful for connect from
 *        a specific interface/port), if NULL a new socket will be created.
 * @param socktype The type of socket to be created.
 */

Sock * Sock_connect(char const *host, char const *port, Sock *binded,
                    sock_type socktype)
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

    if (sock_connect(host, port, &sockfd, socktype)) {
        net_log(NET_LOG_DEBUG, "Sock_connect() failure.\n");
        return NULL;
    }

    if (binded) {
        s = binded;
        free(s->local_host);
        s->local_host = NULL;
        free(s->remote_host);
        s->remote_host = NULL;
    } else if (!(s = calloc(1, sizeof(Sock)))) {
        net_log(NET_LOG_FATAL, "Unable to allocate a Sock struct in Sock_connect().\n");
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
            net_log(NET_LOG_DEBUG,
                    "Unable to get remote port in Sock_connect().\n");
            Sock_close(s);
            return NULL;
        }

    if(!sock_ntop_host(sa_p, local_host, sizeof(local_host)))
        memset(local_host, 0, sizeof(local_host));

    if (!(s->local_host = strdup(local_host))) {
        net_log(NET_LOG_FATAL,
                "Unable to allocate local host in Sock_connect().\n");
        Sock_close(s);
        return NULL;
    }

    local_port = sock_get_port(sa_p);

    if(local_port < 0) {
        net_log(NET_LOG_DEBUG,
                "Unable to get local port in Sock_connect().\n");
        Sock_close(s);
        return NULL;
    } else
        s->local_port = ntohs(local_port);

    sa_p = (struct sockaddr *) &(s->remote_stg);
    sa_len = sizeof(struct sockaddr_storage);

    if(getpeername(s->fd, sa_p, &sa_len))
        {
            net_log(NET_LOG_DEBUG,
                    "Unable to get remote address in Sock_connect().\n");
            Sock_close(s);
            return NULL;
        }

    if(!sock_ntop_host(sa_p, remote_host, sizeof(remote_host)))
        memset(remote_host, 0, sizeof(remote_host));

    if (!(s->remote_host = strdup(remote_host))) {
        net_log(NET_LOG_FATAL,
                "Unable to allocate remote host in Sock_connect().\n");
        Sock_close(s);
        return NULL;
    }

    remote_port = sock_get_port(sa_p);
    if(remote_port < 0) {
        net_log(NET_LOG_DEBUG,
                "Unable to get remote port in Sock_connect().\n");
        Sock_close(s);
        return NULL;
    } else
        s->remote_port = ntohs(remote_port);

    net_log(NET_LOG_DEBUG,
            "Socket connected between local=\"%s\":%u and remote=\"%s\":%u.\n",
            s->local_host, s->local_port, s->remote_host, s->remote_port);

    if(is_multicast_address(sa_p, s->remote_stg.ss_family)) {
        //fprintf(stderr,"IS MULTICAST\n");
        if(mcast_join(s->fd, sa_p)!=0) {
            Sock_close(s);
            return NULL;
        }
        s->flags |= IS_MULTICAST;
    }

    return s;
}

static void net_log_default(int level, const char *fmt, va_list args)
{
    switch (level) {
    case NET_LOG_FATAL:
        fprintf(stderr, "[fatal error] ");
        break;
    case NET_LOG_WARN:
        fprintf(stderr, "[warning] ");
        break;
    case NET_LOG_DEBUG:
#ifdef DEBUG
        fprintf(stderr, "[debug] ");
#else
        return;
#endif
        break;
    case NET_LOG_VERBOSE:
#ifdef VERBOSE
        fprintf(stderr, "[verbose debug] ");
#else
        return;
#endif
        break;
    case NET_LOG_INFO:
        fprintf(stderr, "[info] ");
        break;
    default:
        fprintf(stderr, "[unk] ");
        break;
    }

    vfprintf(stderr, fmt, args);
}

static void (*net_vlog)(int, const char*, va_list) = net_log_default;

void net_log(int level, const char *fmt, ...)
{
    va_list vl;
    va_start(vl, fmt);
    net_vlog(level, fmt, vl);
    va_end(vl);
}

/**
 * Init internal structures.
 * @param log_func Pointer to a proper log function, if NULL messages will
 * be sent to stderr.
 */

void Sock_init(void (*log_func)(int, const char*, va_list))
{
    if (log_func)
        net_vlog = log_func;

    return;
}

/**
 * Put a socket in listening state.
 * @param s Existing socket.
 * @param backlog Number of connection that may wait to be accepted.
 */

int Sock_listen(Sock *s, int backlog)
{
    if (!s)
        return -1;
    return listen(s->fd, backlog);
}

/**
 * Read data from a socket.
 * @param s Existing socket.
 * @param buffer Buffer reserved for receiving data.
 * @param nbytes Size of the buffer.
 * @param protodata Pointer to data depending from socket protocol, if NULL a
 *        suitable default value will be used.
 * @param flags Flags to be passed to posix recv() function.
 */

int Sock_read(Sock *s, void *buffer, int nbytes, void *protodata, int flags)
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
 * Read data to a socket
 * @param s Existing socket.
 * @param buffer Buffer of data to be sent.
 * @param nbytes Amount of data to be sent.
 * @param protodata Pointer to data depending from socket protocol, if NULL a
 *        suitable default value will be used.
 * @param flags Flags to be passed to posix send() function.
 */

int Sock_write(Sock *s, const void *buffer, int nbytes, void *protodata, int flags)
{
#ifdef ENABLE_SCTP
    struct sctp_sndrcvinfo sinfo;
#endif

    if (!s)
        return -1;

    switch (s->socktype) {
    case TCP:
        return send(s->fd, buffer, nbytes, flags);
        break;
    case UDP:
        if (!protodata) {
            protodata = &(s->remote_stg);
        }
        return sendto(s->fd, buffer, nbytes, flags, (struct sockaddr *)
                      protodata, sizeof(struct sockaddr_storage));
        break;
    case SCTP:
#ifdef ENABLE_SCTP
        if (!protodata) {
            protodata = &sinfo;
            memset(protodata, 0, sizeof(struct sctp_sndrcvinfo));
        }
        return sctp_send(s->fd, buffer, nbytes,
                         (struct sctp_sndrcvinfo *) protodata, flags);
#endif
        break;
    case LOCAL:
        return send(s->fd, buffer, nbytes, flags);
        break;
    default:
        break;
    }
    return -1;
}
