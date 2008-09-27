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
#include <assert.h>
#include <string.h>
#include <check.h>

START_TEST (test_connect_lscube)
{
  Sock *socket = Sock_connect("live.polito.it", "80", NULL, TCP, NULL);
  
  fail_if(socket == NULL,
	  "Unable to connect");

  Sock_close(socket);
}
END_TEST

START_TEST (test_remote_host_lscube)
{
  Sock *socket = Sock_connect("live.polito.it", "80", NULL, TCP, NULL);
  char *remote_host = get_remote_host(socket);

  fail_if(strcmp(remote_host, "130.192.86.145") != 0,
	  "Remote host is not 130.192.86.145: %s",
	  remote_host);

  Sock_close(socket);
}
END_TEST

START_TEST (test_local_host_lscube)
{
  Sock *socket = Sock_connect("live.polito.it", "80", NULL, TCP, NULL);
  char actual_local_host[1024];
  char *local_host = get_local_host(socket);

  /* There's a test for this already so just accept it to be fine */
  assert(get_local_hostname(actual_local_host, sizeof(actual_local_host)) != -1);

  fail_if(strcmp(local_host, actual_local_host) != 0,
	  "Local host is not %s: %s",
	  actual_local_host, local_host);

  Sock_close(socket);
}
END_TEST

START_TEST (test_remote_port_lscube)
{
  Sock *socket = Sock_connect("live.polito.it", "80", NULL, TCP, NULL);
  in_port_t remote_port = get_remote_port(socket);

  fail_if(remote_port != 80,
	  "Remote port is not 80: %u",
	  remote_port);

  Sock_close(socket);
}
END_TEST

START_TEST (test_local_port_lscube)
{
  Sock *socket = Sock_connect("live.polito.it", "80", NULL, TCP, NULL);
  in_port_t local_port = get_local_port(socket);

  fail_if(local_port == 0,
	  "Null local port: %u", local_port);

  Sock_close(socket);
}
END_TEST

START_TEST (test_flags_lscube)
{
  Sock *socket = Sock_connect("live.polito.it", "80", NULL, TCP, NULL);
  fail_if(Sock_flags(socket) != 0x00,
	  "Non-null socket flags: %x", Sock_flags(socket));

  Sock_close(socket);
}
END_TEST

START_TEST (test_type_lscube)
{
  Sock *socket = Sock_connect("live.polito.it", "80", NULL, TCP, NULL);
  fail_if(Sock_type(socket) != TCP,
	  "Non-TCP socket: %x", Sock_type(socket));

  Sock_close(socket);
}
END_TEST

void add_testcases_sock(Suite *s) {
  TCase *tc = tcase_create("Socket interface tests");
  
  tcase_add_test(tc, test_connect_lscube);
  tcase_add_test(tc, test_remote_host_lscube);
  /* this has to be fixed, as it reports the local host IP rather than
     name and we haven't an easy way to compare that, yet;
  */
  // tcase_add_test(tc, test_local_host_lscube);
  tcase_add_test(tc, test_remote_port_lscube);
  tcase_add_test(tc, test_local_port_lscube);
  tcase_add_test(tc, test_flags_lscube);
  tcase_add_test(tc, test_type_lscube);

  suite_add_tcase(s, tc);
}
