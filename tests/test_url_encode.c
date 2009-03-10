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

#include "netembryo/url.h"
#include <stdlib.h>
#include <check.h>

static void test_url_encoding(const char *original, const char *compare)
{
  /* For good peace, double the size */
  char *result = malloc(strlen(compare)*2);

  fail_if(Url_encode(result, original, strlen(compare)*2) == -1,
	  "Invalid URL provided: %s", original);

  fail_if(strcmp(result, compare) != 0,
	  "URL '%s' encoded to different string: '%s'",
	  original, result);

  free(result);
}

START_TEST (test_space_encoding)
{
  test_url_encoding("a b c d", "a+b+c+d");
}
END_TEST

START_TEST (test_colon_encoding)
{
  test_url_encoding("a:b:c:d", "a%3ab%3ac%3ad");
}
END_TEST

START_TEST (test_percent_encoding)
{
  test_url_encoding("a%b%c%d", "a%25b%25c%25d");
}
END_TEST

START_TEST (test_hash_encoding)
{
  test_url_encoding("a#b#c#d", "a%23b%23c%23d");
}
END_TEST

START_TEST (test_full_encoding)
{
  /* These are all the characters that can be left unencoded */
  test_url_encoding(
		    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		    "abcdefghijklmnopqrstuvwxyz"
		    "01234567890"
		    "$-_.+!*'(),\"",
		    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		    "abcdefghijklmnopqrstuvwxyz"
		    "01234567890"
		    "$-_.+!*'(),\"");
}
END_TEST

START_TEST (test_control_character)
{
  char result[4];

  fail_unless(Url_encode(result, "\a", 4) == -1,
	      "Control character accepted");
}
END_TEST

START_TEST (test_non_ascii)
{
  char result[4];

  fail_unless(Url_encode(result, "\x85", 4) == -1,
	      "Non-ASCII character accepted");
}
END_TEST

START_TEST (test_too_long)
{
  char result[5];
  
  fail_unless(Url_encode(result, "&", 3) == -1,
	      "Encoding happending on too small space");
}
END_TEST

START_TEST (test_null_source)
{
  char result[5];
  
  fail_unless(Url_encode(result, NULL, 3) == -1,
	      "Encoding happending for NULL source");
}
END_TEST

START_TEST (test_zero_destination)
{
  char result[5];

  fail_unless(Url_encode(result, "=", 0) == -1,
	      "Encoding happending for NULL source");
}
END_TEST

START_TEST (test_null_destination)
{
  fail_unless(Url_encode(NULL, "=", 10) == -1,
	      "Encoding happending for NULL source");
}
END_TEST

void add_testcases_url_encode(Suite *s) {
  TCase *tc = tcase_create("URL encoding tests");
  
  tcase_add_test(tc, test_space_encoding);
  tcase_add_test(tc, test_colon_encoding);
  tcase_add_test(tc, test_percent_encoding);
  tcase_add_test(tc, test_hash_encoding);
  tcase_add_test(tc, test_full_encoding);
  tcase_add_test(tc, test_control_character);
  tcase_add_test(tc, test_non_ascii);
  tcase_add_test(tc, test_too_long);
  tcase_add_test(tc, test_null_source);
  tcase_add_test(tc, test_zero_destination);
  tcase_add_test(tc, test_null_destination);

  suite_add_tcase(s, tc);
}
