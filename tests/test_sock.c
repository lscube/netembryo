/* *
 * * This file is part of NetEmbryo
 *
 * Copyright (C) 2008 by LScube team <team@streaming.polito.it>
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

#include "netembryo/wsocket.h"
#include <glib.h>
#include "gtest-extra.h"

/* Change this if the lscube.org server changes */
static const char test_host[] = "www.lscube.org";
static const char test_port[] = "80";
static const char test_port_name[] = "http";
static const char test_ipv4[] = "194.116.73.70";

void test_connect_lscube()
{
  Sock *socket = neb_sock_connect(test_host, test_port, NULL, TCP);

  g_assert(socket);

  neb_sock_close(socket);
}

void test_remote_host_lscube()
{
  Sock *socket = neb_sock_connect(test_host, test_port, NULL, TCP);

  g_assert_cmpstr(socket->remote_host, ==, test_ipv4);

  neb_sock_close(socket);
}

void test_remote_port_lscube()
{
  Sock *socket = neb_sock_connect(test_host, test_port, NULL, TCP);

  g_assert_cmpint(neb_sa_get_port((struct sockaddr*)&socket->remote_stg), ==, 80);

  neb_sock_close(socket);
}

void test_remote_port_name_lscube()
{
  Sock *socket = neb_sock_connect(test_host, test_port_name, NULL, TCP);

  g_assert_cmpint(neb_sa_get_port((struct sockaddr*)&socket->remote_stg), ==, 80);

  neb_sock_close(socket);
}

void test_local_port_lscube()
{
  Sock *socket = neb_sock_connect(test_host, test_port, NULL, TCP);

  g_assert_cmpint(neb_sa_get_port((struct sockaddr*)&socket->local_stg), !=, 0);

  neb_sock_close(socket);
}

void test_type_lscube()
{
  Sock *socket = neb_sock_connect(test_host, test_port, NULL, TCP);

  g_assert_cmphex(socket->socktype, ==, TCP);

  neb_sock_close(socket);
}

void test_random_port()
{
    Sock *sock = neb_sock_bind("localhost", NULL, NULL, TCP);
    g_assert(sock);

    g_assert_cmpuint(neb_sa_get_port((struct sockaddr*)&socket->local_stg), !=, 0);

    neb_sock_close(sock);
}

void test_all_interfaces()
{
    Sock *sock = neb_sock_bind(NULL, "65123", NULL, TCP);
    g_assert(sock);

    neb_sock_close(sock);
}
