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
 * @file socket.c
 * low level socket management
 */

#ifdef ENABLE_SCTP
/* FreeBSD and Mac OS X don't have SOL_SCTP and re-use IPPROTO_SCTP
   for setsockopt() */
# if !defined(SOL_SCTP)
#  define SOL_SCTP IPPROTO_SCTP
# endif
#endif

static struct addrinfo *_neb_sock_getaddrinfo(const char const *host,
                                              const char *const port,
                                              sock_type socktype)
{
    struct addrinfo *res;
    struct addrinfo hints;
    int n;

    memset(&hints, 0, sizeof(hints));

    if (host == NULL)
        hints.ai_flags = AI_PASSIVE;
    else
        hints.ai_flags = AI_CANONNAME;

#ifdef IPV6
    hints.ai_family = AF_UNSPEC;
#else
    hints.ai_family = AF_INET;
#endif

    switch (socktype) {
    case SCTP:
#ifdef ENABLE_SCTP
        hints.ai_socktype = SOCK_SEQPACKET;
#else
        neb_log(NET_LOG_ERR, "SCTP protocol not compiled in\n");
        return WSOCK_ERROR;
#endif
        break;
    case TCP:
        hints.ai_socktype = SOCK_STREAM;
        break;
    case UDP:
        hints.ai_socktype = SOCK_DGRAM;
        break;
    default:
        neb_log(NEB_LOG_ERR, "Unknown socket type specified\n");
        return NULL;
        break;
    }

    if ((n = getaddrinfo(host, port, &hints, &res)) != 0) {
        neb_log(NEB_LOG_ERR, "%s\n", gai_strerror(n));
        return NULL;
    }

    return res;
}

static int _neb_sock_sctp_setparams(int sd)
{
#ifdef ENABLE_SCTP
    struct sctp_initmsg initparams;
    struct sctp_event_subscribe subscribe;

    // Enable the propagation of packets headers
    memset(&subscribe, 0, sizeof(subscribe));
    subscribe.sctp_data_io_event = 1;
    if (setsockopt(sd, SOL_SCTP, SCTP_EVENTS, &subscribe,
                   sizeof(subscribe)) < 0) {
        neb_log(NEB_LOG_ERR, "setsockopts(SCTP_EVENTS) error in sock_connect.\n");
        return WSOCK_ERROR;
    }

    // Setup number of streams to be used for SCTP connection
    memset(&initparams, 0, sizeof(initparams));
    initparams.sinit_max_instreams = NETEMBRYO_MAX_SCTP_STREAMS;
    initparams.sinit_num_ostreams = NETEMBRYO_MAX_SCTP_STREAMS;
    if (setsockopt(sd, SOL_SCTP, SCTP_INITMSG, &initparams,
                   sizeof(initparams)) < 0) {
        neb_log(NEB_LOG_ERR, "setsockopts(SCTP_INITMSG) error in sock_connect.\n");
        return WSOCK_ERROR;
    }

    return WSOCK_OK;
#else
    return WSOCK_ERROR;
#endif
}

/**
 * bind wrapper
 */

static int _neb_sock_bind(const char const *host, const char const *port, int *sock, sock_type socktype)
{
    int bind_new;
    struct addrinfo *res, *ressave;

    if ( (ressave = res = _neb_sock_getaddrinfo(host, port, socktype)) == NULL ) {
        return WSOCK_ERRADDR;
    }

    bind_new = (*sock < 0);

    do {
        if (bind_new && (*sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) < 0)
            continue;

        if ( socktype == SCTP &&
             _neb_sock_sctp_setparams(*sock) < 0 )
            continue;

        if (bind(*sock, res->ai_addr, res->ai_addrlen) == 0)
            break;

        if (bind_new) {
            if (close(*sock) < 0)
                return WSOCK_ERROR;
            else
                *sock = -1;
        }

    } while ((res = res->ai_next) != NULL);

    freeaddrinfo(ressave);

    if ( !res )
        return WSOCK_ERROR;

    return 0;
}

/**
 * wraps connect
 */

static int _neb_sock_connect(const char const *host, const char const *port, int *sock, sock_type socktype)
{
    int connect_new;
    struct addrinfo *res, *ressave;

    if ( (ressave = res = _neb_sock_getaddrinfo(host, port, socktype)) == NULL ) {
        return WSOCK_ERRADDR;
    }

    connect_new = (*sock < 0);

    do {
        if (connect_new && (*sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) < 0)
            continue;

        if ( socktype == SCTP &&
             _neb_sock_sctp_setparams(*sock) < 0 )
            continue;

        if (connect(*sock, res->ai_addr, res->ai_addrlen) == 0)
            break;

        if (connect_new) {
            if (close(*sock) < 0)
                return WSOCK_ERROR;
            else
                *sock = -1;
        }

    } while ((res = res->ai_next) != NULL);

    freeaddrinfo(ressave);

    if ( !res )
        return WSOCK_ERROR;

    return WSOCK_OK;
}
