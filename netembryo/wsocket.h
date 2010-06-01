/* *
 * This file is part of NetEmbryo
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
 * @file wsocket.h
 * socket wrapper
 */

#ifndef NETEMBRYO_WSOCKET_H
#define NETEMBRYO_WSOCKET_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>

#ifndef WIN32
#   include <unistd.h>
#   include <netinet/in.h>
#   include <arpa/inet.h>
#   include <sys/types.h>
#   include <sys/socket.h>
#   include <arpa/inet.h>
#   include <netdb.h>
#else
#   include <winsock2.h>
#   include <ws2tcpip.h>
#   include <stdint.h>
#endif

#ifdef WIN32
typedef unsigned short sa_family_t;
typedef unsigned short in_port_t;
typedef unsigned int in_addr_t;
#endif

/** socket type definition */
typedef enum {
    /** socket fd not valid */
    SOCK_NONE,
    /** IP based protcols */
    TCP,
    UDP,
    SCTP,
    /** Local socket (Unix) */
    LOCAL
} sock_type;

/**
 * Socket abstraction structure
 */
typedef struct {
    sock_type socktype; ///< socket type
    int fd;    ///< low level socket file descriptor
    struct sockaddr_storage remote_stg;    ///< low level address storage from getpeername
    struct sockaddr_storage local_stg;    ///< low level address storage from getsockname
    /** human readable data */
    char *remote_host; ///< remote host stored as dinamic string
    char *local_host; ///< local host stored as dinamic string
    in_port_t remote_port;    ///< remote port stored in host order
    in_port_t local_port;    ///< local port stored in host order
    void *data; ///< user data
} Sock;

#define WSOCK_ERRORPROTONOSUPPORT -5
#define WSOCK_ERRORIOCTL    -4
#define WSOCK_ERRORINTERFACE    -3
#define WSOCK_ERROR    -2
#define WSOCK_ERRFAMILYUNKNOWN    -1
#define WSOCK_OK 0
#define WSOCK_ERRSIZE    1
#define WSOCK_ERRFAMILY    2
#define WSOCK_ERRADDR    3
#define WSOCK_ERRPORT    4

#define NETEMBRYO_MAX_SCTP_STREAMS 15

/**
 * @defgroup neb_logging Logging facilities
 * @{
 */

/**
 * Levels used by the logging functions
 */
typedef enum {
    NEB_LOG_FATAL,
    NEB_LOG_ERR,
    NEB_LOG_WARN,
    NEB_LOG_INFO,
    NEB_LOG_DEBUG,
    NEB_LOG_VERBOSE,
    NEB_LOG_UNKNOWN
} NebLogLevel;

/**
 * @brief Variable-argument logging function
 *
 * @param level The level of the log message
 * @param fmt printf(3)-formatted format string
 * @param args arguments for the format string
 *
 * This function can be replaced by re-defining it in the final
 * executable or other library using netembryo; since what is
 * presented by netembryo is only a weak reference, the loader will
 * take care of making use of the later-defined function instead.
 */
void neb_vlog(NebLogLevel level, const char *fmt, va_list args);

/**
 * @}
 */

/** @defgroup NetEmbryo_Socket Sockets Access Interface
 *
 * @brief simple socket abstraction.
 *
 * @{
 */

/**
 * @brief Parse a sockaddr structure into mnemonic host and numeric port values
 *
 * @param sa The sockaddr structure to fetch the address from
 * @param[out] host_p A pointer to the pointer where to duplicate the
 *                    hostname string
 * @param[out] port_p A pointer to the variable to save the port to
 *
 * @retval 0 No error, @p host_p and @p port_p parameters are set.
 * @retval -1 Error deep within the parsing
 */
int neb_sock_parse_address(const struct sockaddr *sa,
                           char **host_p, in_port_t *port_p);

/**
 * Create a new socket and binds it to an address/port.
 * @param host Local address to be used by this socket, if NULL the socket will
 *        be bound to all interfaces.
 * @param port Local port to be used by this socket, if NULL a random port will
 *        be used.
 * @param sock Pointer to a pre-created socket
 * @param socktype The type of socket to be created.
 */
Sock * neb_sock_bind(const char const *host,
                     const char const *port,
                     Sock *sock,
                     sock_type socktype);

/**
 * Create a new socket accepting a new connection from a listening socket.
 * @param s Listening socket.
 * @return the newly allocated Sock
 */
Sock * neb_sock_accept(Sock *main);

int neb_sock_close(Sock *s);

/**
 * @}
 */

in_port_t neb_sa_get_port(struct sockaddr *sa);
void neb_sa_set_port(struct sockaddr *sa, in_port_t port);

#endif // NETEMBRYO_WSOCKET_H
