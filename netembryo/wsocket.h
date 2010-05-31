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

/** flags definition*/
typedef enum {
    /** ssl flags */
    IS_SSL = 0x1,
    IS_TLS = 0x3, /**< setting this will also set IS_SSL */
} sock_flags;

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
    struct sockaddr_storage local_stg;    ///< low level address storage from getsockname
    struct sockaddr_storage remote_stg;    ///< low level address storage from getpeername
    /** flags */
    sock_flags flags;
    /** human readable data */
    char *remote_host; ///< remote host stored as dinamic string
    char *local_host; ///< local host stored as dinamic string
    in_port_t remote_port;    ///< remote port stored in host order
    in_port_t local_port;    ///< local port stored in host order
    void *ssl; ///< stores ssl context information
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
 * Establish a connection to a remote host.
 * @param host Remote host to connect to (may be a hostname).
 * @param port Remote port to connect to.
 * @param binded Pointer to a pre-binded socket (useful for connect from
 *        a specific interface/port), if NULL a new socket will be created.
 * @param socktype The type of socket to be created.
 */
Sock * neb_sock_connect(const char const *host,
                        const char const *port,
                        Sock *binded,
                        sock_type socktype);

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

/**
 * Put a socket in listening state.
 * @param s Existing socket.
 * @param backlog Number of connection that may wait to be accepted.
 */
int neb_sock_listen(Sock *s, int backlog);

/**
 * Read data from a socket.
 * @param s Existing socket.
 * @param buffer Buffer reserved for receiving data.
 * @param nbytes Size of the buffer.
 * @param protodata Pointer to data depending from socket protocol, if NULL a
 *        suitable default value will be used.
 * @param flags Flags to be passed to posix recv() function.
 */
int neb_sock_read(Sock *s, void *buffer, int nbytes, void *protodata, int flags);

/**
 * @brief Send data to a socket
 *
 * @param s The existing socket
 * @param buffer Buffer of data to be sent
 * @param nbytes Size of data to be sent
 * @param flags Flags to use during write operationg
 */
int neb_sock_write(Sock *s, const void *buffer, size_t nbytes, int flags);

/**
 * @brief Send data to an open multiplexed socket
 *
 * @param s The existing socket
 * @param buffer Buffer of data to be sent
 * @param nbytes Size of data to be sent
 * @param flags Flags to use during write operation
 * @param stream SCTP stream to send the data to
 */
int neb_sock_write_stream(Sock *s, const void *buffer, size_t nbytes, int flags, int stream);

int neb_sock_close(Sock *s);

/**
 * @}
 */

#endif // NETEMBRYO_WSOCKET_H
