/* *
 *  This file is part of Feng
 *
 * Copyright (C) 2009 by LScube team <team@lscube.org>
 * See AUTHORS for more details
 *
 * Feng is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * Feng is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Feng; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * */

/**
 * @file ssl.c
 * SSL support
 */

#include "ssl.h"
#include "netembryo/wsocket.h"

/**
 * The Function establishes a new ssl connection
 * @param : the socket descriptor
 * @return: pointer to new structure ssl
 */

SSL *SSL_sock_accept(int sockfd, SSL_CTX * global_ctx) {

    SSL *ssl;

    ssl = SSL_new(global_ctx);
    if(!ssl) {
        net_log(NET_LOG_ERR, "Sock SSL_connect: SSL_new() failed.\n");
        return 0;
    }

    if((SSL_set_fd(ssl,sockfd))==0) {
        net_log(NET_LOG_ERR,"Operation failed\n");
        return 0;
    }

    if((SSL_accept(ssl)<=0)) {
        net_log(NET_LOG_ERR,"SSL accept error");
        return 0;
    }

    return(ssl);
}


/**
 * It closes a ssl connection
 * @param : pointer to structure ssl
 * @param : the socket descriptor
 * @return: '1' success operation, otherwise '0'
 */

int SSL_close_connection(SSL *ssl, int sockfd) {

    int exit;

    exit = SSL_shutdown(ssl);
    if(!exit) {
        shutdown(sockfd,SHUT_WR);
        SSL_shutdown(ssl);
    } else {
        net_log(NET_LOG_ERR,"Shutdown failed");
        return 0;
    }
    SSL_free(ssl);
    return 1;
}


/**
 * This function reads at most nbyte from the connection ssl
 * @param : pointer to structure ssl
 * @param : buffer to store bytes that are read.
 * @param : maximum number of bytes to write to the buffer
 * @return: 'number of bytes read' success operation, otherwise '0'
 */

int SSL_sock_read(SSL *ssl, void *buffer, int nbyte) {

    if(ssl)
        return(SSL_read(ssl,buffer,nbyte));
    else
        return 0;
}


/**
 * This function writes up to nbyte from buffer to socket ssl
 * @param : pointer to structure ssl
 * @param : buffer to store bytes that are read.
 * @param : maximum number of bytes to write to the buffer
 * @return: Number of bytes written
 */

int SSL_sock_write(SSL *ssl, const void *buffer, int nbyte) {
    return(SSL_write(ssl,buffer,nbyte));
}

int sock_SSL_connect(SSL **ssl_con, int sockfd, SSL_CTX * ssl_ctx)
{
    int ssl_err;

    if(!ssl_ctx) {
        net_log(NET_LOG_ERR, "sock_SSL_connect: !ssl_ctx\n");
        return WSOCK_ERROR;
    }

    *ssl_con = SSL_new(ssl_ctx);

    if(!(*ssl_con)) {
        net_log(NET_LOG_ERR, "sock_SSL_connect: SSL_new() failed.\n");
        SSL_CTX_free(ssl_ctx);
        return WSOCK_ERROR;
    }

    SSL_set_fd (*ssl_con, sockfd);
    SSL_set_connect_state(*ssl_con);
    ssl_err = SSL_connect(*ssl_con);

    if(ssl_err < 0)
        SSL_set_shutdown(*ssl_con, SSL_SENT_SHUTDOWN);
    if(ssl_err <= 0) {
        net_log(NET_LOG_ERR, "sock_SSL_connect: SSL_connect() failed.\n");
        SSL_free(*ssl_con);
        SSL_CTX_free(ssl_ctx);
        return WSOCK_ERROR;
    }

    return WSOCK_OK;
}
