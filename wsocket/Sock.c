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

/**
 * @file Sock.c
 * High level inteface
 */

#include <config.h>
#include <sys/types.h>
#include <string.h>
#include "wsocket.h"
#if HAVE_SSL
#include <openssl/ssl.h>
#endif

Sock * Sock_accept(Sock *s)
{
    int res = -1;
    char remote_host[128]; /*Unix Domain is largest*/
    char local_host[128]; /*Unix Domain is largest*/
    int32_t remote_port = -1;
    int32_t local_port = -1;
    Sock *new_s = NULL;
    struct sockaddr *sa_p = NULL;
    socklen_t sa_len = 0;
#if HAVE_SSL
    SSL *ssl_con = NULL;
#endif

    if (!s)
        return NULL;

    if ((res = sock_accept(s->fd)) < 0) {
        net_log(NET_LOG_ERR, "System error in sock_accept().\n");
        return NULL;
    }

#if HAVE_SSL
    if(s->flags & IS_SSL) {
        if(sock_SSL_accept(&ssl_con,res)) {
            net_log(NET_LOG_ERR, "Unable to accept SSL connection.\n");
            sock_close(res);
            return NULL;
        }
    }
#endif

    if (!(new_s = calloc(1, sizeof(Sock)))) {
        net_log(NET_LOG_FATAL,
                "Unable to allocate a Sock struct in Sock_accept().\n");
#if HAVE_SSL
        if(s->flags & IS_SSL) 
            sock_SSL_close(ssl_con);
#endif
        sock_close(res);
        return NULL;
    }

    new_s->fd = res;
    new_s->socktype = s->socktype;
    new_s->flags = s->flags;

#if HAVE_SSL
    if(s->flags & IS_SSL) 
        new_s->ssl = ssl_con;
#endif

    sa_p = (struct sockaddr *) &(new_s->remote_stg);
    sa_len = sizeof(struct sockaddr_storage);

    if(getpeername(res, sa_p, &sa_len))
    {
        net_log(NET_LOG_ERR,
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
        net_log(NET_LOG_ERR, "Unable to get remote port in Sock_accept().\n");
        Sock_close(new_s);
        return NULL;
    }
    else
        new_s->remote_port = ntohs(remote_port);

    sa_p = (struct sockaddr *) &(new_s->remote_stg);
    sa_len = sizeof(struct sockaddr_storage);

    if(getsockname(res, sa_p, &sa_len))
    {
        net_log(NET_LOG_ERR, "Unable to get remote port in Sock_accept().\n");
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
        net_log(NET_LOG_ERR, "Unable to get local port in Sock_accept().\n");
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

Sock * Sock_bind(char *host, char *port,
                 sock_type socktype, sock_flags ssl_flag)
{

    Sock *s = NULL;
    int sockfd = -1;
    struct sockaddr *sa_p;
    socklen_t sa_len;
    char local_host[128];
    int32_t local_port;

#if HAVE_SSL
    if ((ssl_flag & IS_SSL)) {
        if(socktype != TCP) {
            net_log(NET_LOG_ERR, "SSL can't work on this protocol.\n");
            return NULL;
        }
    }
#endif

    if (sock_bind(host, port, &sockfd, socktype)) {
        net_log(NET_LOG_ERR, "Error in low level sock_bind().\n");
        return NULL;
    }

    if (!(s = calloc(1, sizeof(Sock)))) {
        net_log(NET_LOG_FATAL,
                "Unable to allocate a Sock struct in Sock_bind().\n");
        sock_close(sockfd);
        return NULL;
    }

    s->fd = sockfd;
    s->socktype = socktype;

    s->flags = ssl_flag;

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
        net_log(NET_LOG_ERR, "Unable to get local port in Sock_bind().\n");
        Sock_close(s);
        return NULL;
    } else
        s->local_port = ntohs(local_port);

    net_log(NET_LOG_DEBUG,
            "Socket bound with addr=\"%s\" and port=\"%u\".\n",
            s->local_host, s->local_port);

    if(is_multicast_address(sa_p, s->local_stg.ss_family)) {
        if(mcast_join(s->fd, sa_p, NULL, 0, &(s->addr) )) {
            Sock_close(s);
            return NULL;
        }
        s->flags |= IS_MULTICAST;
    }

    return s;
}

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

#if HAVE_SSL
    if(s->flags & IS_SSL)
        sock_SSL_close(s->ssl);
#endif

    res = sock_close(s->fd);

    if (s->remote_host) free(s->remote_host);
    if (s->local_host) free(s->local_host);
    free(s);

    return res;
}

int Sock_compare(Sock *p, Sock *q)
{
    return memcmp(p, q, sizeof(Sock));
}


Sock * Sock_connect(char *host, char *port, Sock *binded,
                    sock_type socktype, sock_flags ssl_flag)
{
    Sock *s;
    char remote_host[128]; /*Unix Domain is largest*/
    char local_host[128]; /*Unix Domain is largest*/
    int sockfd = -1;
    struct sockaddr *sa_p = NULL;
    socklen_t sa_len = 0;
    int32_t local_port;
    int32_t remote_port;
#if HAVE_SSL
    SSL *ssl_con;
#endif

    if(binded) {
        sockfd = binded->fd;
    }

    if (sock_connect(host, port, &sockfd, socktype)) {
            net_log(NET_LOG_ERR, "Sock_connect() failure.\n");
            return NULL;
    }

#if HAVE_SSL
    if(ssl_flag & IS_SSL) {
        if (sock_SSL_connect(&ssl_con))
            net_log (NET_LOG_ERR, "Sock_connect() failure in SSL init.\n");
            sock_close(sockfd);
            return NULL;
    }
    else
#endif

    if (binded) {
        s = binded;
        free(s->local_host);
        s->local_host = NULL;
        free(s->remote_host);
        s->remote_host = NULL;
    } else if (!(s = calloc(1, sizeof(Sock)))) {
        net_log(NET_LOG_FATAL, "Unable to allocate a Sock struct in Sock_connect().\n");
#if HAVE_SSL
        if(ssl_flag & IS_SSL) 
            sock_SSL_close(ssl_con);
#endif
        sock_close (sockfd);
        return NULL;
    }

    s->fd = sockfd;
    s->socktype = socktype;
#if HAVE_SSL
    if(ssl_flag & IS_SSL) 
        s->ssl = ssl_con;
#endif
    s->flags = ssl_flag;

    sa_p = (struct sockaddr *) &(s->local_stg);
    sa_len = sizeof(struct sockaddr_storage);

    if(getsockname(s->fd, sa_p, &sa_len))
    {
        net_log(NET_LOG_ERR,
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
        net_log(NET_LOG_ERR,
                "Unable to get local port in Sock_connect().\n");
        Sock_close(s);
        return NULL;
    } else
        s->local_port = ntohs(local_port);

    sa_p = (struct sockaddr *) &(s->remote_stg);
    sa_len = sizeof(struct sockaddr_storage);

    if(getpeername(s->fd, sa_p, &sa_len))
    {
        net_log(NET_LOG_ERR,
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
        net_log(NET_LOG_ERR,
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
        if(mcast_join(s->fd, sa_p, NULL, 0, &(s->addr))!=0) {
            Sock_close(s);
            return NULL;
        }
        s->flags |= IS_MULTICAST;
    }

    return s;
}

int Sock_create_ssl_connection(Sock *s)
{
    if(!s)
        return WSOCK_ERROR;

#if HAVE_SSL
    if(s->flags & IS_SSL) {
        s->ssl = get_ssl_connection(s->fd);
        if(!(s->ssl))
            return WSOCK_ERROR;
    }
#endif
    return WSOCK_OK;
}

static void net_log_default(int level, const char *fmt, va_list args)
{
    switch (level) {
        case NET_LOG_FATAL:
            fprintf(stderr, "[fatal error] ");
            break;
        case NET_LOG_ERR:
            fprintf(stderr, "[error] ");
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


void Sock_init(void (*log_func)(int, const char*, va_list))
{
#if HAVE_SSL
    SSL_library_init();
    SSL_load_error_strings();
#endif

    if (log_func)
        net_vlog = log_func;

    return;
}

int Sock_listen(Sock *s, int backlog)
{
    if (!s)
        return -1;
    return sock_listen(s->fd, backlog);
}

int Sock_read(Sock *s, void *buffer, int nbytes, void *protodata, int flags)
{

    socklen_t sa_len = sizeof(struct sockaddr_storage);

    if(!s)
        return -1;

#if HAVE_SSL
    if(s->flags & IS_SSL)
        n = sock_SSL_read(s->ssl,buffer,nbytes);
    else {
#endif
        switch(s->socktype) {
        case UDP:
            if (!protodata) {
                return -1;
            }
            return recvfrom(s->fd, buffer, nbytes, flags,
                (struct sockaddr *) protodata, &sa_len);
            break;
        case TCP:
            return recv(s->fd, buffer, nbytes, flags);
            break;
        case SCTP:
#ifdef HAVE_LIBSCTP
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
#if HAVE_SSL
    }
#endif

    return -1;
}

int Sock_set_dest(Sock *s, struct sockaddr *sa) {

    if (!s)
        return -1;

    if (s->socktype != UDP) {
        net_log(NET_LOG_FATAL,
                "Only UDP socket can change destination address\n");
        return -1;
    }

    switch (sa->sa_family) {
    case AF_INET:
        memcpy(&(s->remote_stg), sa, sizeof(struct sockaddr_in));
        break;
    case AF_INET6:
        memcpy(&(s->remote_stg), sa, sizeof(struct sockaddr_in6));
        break;
    default:
        break;
    }
    return 0;
}


int Sock_set_props(Sock *s, int request, int *on)
{
    if (!s)
        return -1;
    return ioctl(s->fd, request, on);
}

int Sock_socketpair(Sock *pair[]) {

    int sdpair[2], i, res;

    if (!pair)
        return -1;

    if ((res = socketpair(AF_UNIX, SOCK_DGRAM, 0, sdpair)) < 0) {
        net_log(NET_LOG_ERR, "Sock_socketpair() failure.\n");
        return res;
    }
    
    if (!(pair[0] = calloc(1, sizeof(Sock)))) {
        net_log(NET_LOG_FATAL,
                "Unable to allocate first Sock struct in Sock_socketpair().\n");
        close (sdpair[0]);
        close (sdpair[1]);
        return -1;
    }
    if (!(pair[1] = calloc(1, sizeof(Sock)))) {
        net_log(NET_LOG_FATAL,
                "Unable to allocate second Sock struct in Sock_socketpair().\n");
        close (sdpair[0]);
        close (sdpair[1]);
        free(pair[0]);
        return -1;
    }

    for (i = 0; i < 2; i++) {
        pair[i]->fd = sdpair[i];
        pair[i]->socktype = LOCAL;
    }

    return res;
}

int Sock_write(Sock *s, void *buffer, int nbytes, void *protodata, int flags)
{
#ifdef HAVE_LIBSCTP
    struct sctp_sndrcvinfo sinfo;
#endif

    if (!s)
        return -1;

#if HAVE_SSL
    if(s->flags & IS_SSL)
        return sock_SSL_write(s->ssl, buffer, nbytes);
    else {
#endif        
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
#ifdef HAVE_LIBSCTP
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
#if HAVE_SSL
    }
#endif        
    return -1;
}