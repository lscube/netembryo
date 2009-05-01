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

#include "netembryo/url.h"
#include <check.h>

static void test_url(char * url, char * protocol, char * hostname, char * port, char * path)
{
    Url turl;

    Url_init(&turl, url);

    if ( protocol && !turl.protocol )
      fail("Expected protocol specified, but no protocol identified");
    else if ( !protocol && turl.protocol )
      fail("No protocol expected, but protocol identified: '%s'", turl.protocol);
    else if ( protocol && protocol )
      fail_unless(strcmp(turl.protocol, protocol) == 0,
		  "Protocol identified differs from specified: '%s' vs '%s'", turl.protocol, protocol);

    if ( hostname && !turl.hostname )
      fail("Expected hostname specified, but no hostname identified");
    else if ( !hostname && turl.hostname )
      fail("No hostname expected, but hostname identified: '%s'", turl.hostname);
    else if ( hostname && hostname )
      fail_unless(strcmp(turl.hostname, hostname) == 0,
		  "Hostname identified differs from specified: '%s' vs '%s'", turl.hostname, hostname);

    if ( port && !turl.port )
      fail("Expected port specified, but no port identified");
    else if ( !port && turl.port )
      fail("No port expected, but port identified: '%s'", turl.port);
    else if ( port && port )
      fail_unless(strcmp(turl.port, port) == 0,
		  "Port identified differs from specified: '%s' vs '%s'", turl.port, port);

    if ( path && !turl.path )
      fail("Expected path specified, but no path identified");
    else if ( !path && turl.path )
      fail("No path expected, but path identified: '%s'", turl.path);
    else if ( path && path )
      fail_unless(strcmp(turl.path, path) == 0,
		  "Path identified differs from specified: '%s' vs '%s'", turl.path, path);

    Url_destroy(&turl);
}

START_TEST (long_url)
{
    test_url("rtsp://this.is.a.very.long.url:this_should_be_the_port/this/is/a/path/to/file.wmv",
             "rtsp", "this.is.a.very.long.url", "this_should_be_the_port", "this/is/a/path/to/file.wmv");
}
END_TEST

START_TEST (split_url)
{
    test_url("rtsp://this.is.the.host/this/is/the/path.avi", "rtsp", "this.is.the.host", NULL, "this/is/the/path.avi");
}
END_TEST

START_TEST (no_proto)
{
    test_url("host:80/file.wmv", NULL, "host", "80", "file.wmv");
}
END_TEST

START_TEST (no_path)
{
    test_url("host/file.wmv", NULL, "host", NULL, "file.wmv");
}
END_TEST

START_TEST (just_host)
{
    test_url("host", NULL, "host", NULL, NULL);
}
END_TEST

START_TEST (just_proto_host)
{
    test_url("rtsp://host", "rtsp", "host", NULL, NULL);
}
END_TEST

START_TEST (just_proto_host_port)
{
    test_url("rtsp://host:port", "rtsp", "host", "port", NULL);
}
END_TEST

void add_testcases_url(Suite *s) {
  TCase *tc = tcase_create("URL handling tests");

  tcase_add_test(tc, long_url);
  tcase_add_test(tc, split_url);
  tcase_add_test(tc, no_proto);
  tcase_add_test(tc, no_path);
  tcase_add_test(tc, just_host);
  tcase_add_test(tc, just_proto_host);
  tcase_add_test(tc, just_proto_host_port);

  suite_add_tcase(s, tc);
}
