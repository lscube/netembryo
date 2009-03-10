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

static void test_url_decoding(const char *original, const char *compare)
{
  /* For good peace, double the size */
  char *result = malloc(strlen(original)*2);

  fail_if(Url_decode(result, original, strlen(original)*2) == -1,
	  "Invalid URL provided: %s", original);

  fail_if(strcmp(result, compare) != 0,
	  "URL '%s' decoded to different string: '%s'",
	  original, result);

  free(result);
}

START_TEST (test_space_decoding)
{
  test_url_decoding("a+b+c+d", "a b c d");
}
END_TEST

START_TEST (test_space_decoding_2)
{
  test_url_decoding("a%20b%20c%20d", "a b c d");
}
END_TEST

START_TEST (test_colon_decoding)
{
  test_url_decoding("a%3ab%3ac%3ad", "a:b:c:d");
}
END_TEST

START_TEST (test_percent_decoding)
{
  test_url_decoding("a%25b%25c%25d", "a%b%c%d");
}
END_TEST

START_TEST (test_hash_decoding)
{
  test_url_decoding("a%23b%23c%23d", "a#b#c#d");
}
END_TEST

START_TEST (test_full_decoding)
{
  /* These are all the characters that can be left unencoded */
  test_url_decoding(
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

  fail_unless(Url_decode(result, "\a", 4) == -1,
	      "Control character accepted");
}
END_TEST

START_TEST (test_non_ascii)
{
  char result[4];

  fail_unless(Url_decode(result, "\x85", 4) == -1,
	      "Non-ASCII character accepted");
}
END_TEST

START_TEST (test_too_long)
{
  char result[5];
  
  fail_unless(Url_decode(result, "%20%20%20", 3) == -1,
	      "Decoding happending on too small space");
}
END_TEST

START_TEST (test_null_source)
{
  char result[5];
  
  fail_unless(Url_decode(result, NULL, 3) == -1,
	      "Decoding happending for NULL source");
}
END_TEST

START_TEST (test_zero_destination)
{
  char result[5];

  fail_unless(Url_decode(result, "%20", 0) == -1,
	      "Decoding happending for zero-sized destination");
}
END_TEST

START_TEST (test_null_destination)
{
  fail_unless(Url_decode(NULL, "%20", 10) == -1,
	      "Decoding happending for NULL destination");
}
END_TEST

void add_testcases_url_decode(Suite *s) {
  TCase *tc = tcase_create("URL decoding tests");
  
  tcase_add_test(tc, test_space_decoding);
  tcase_add_test(tc, test_colon_decoding);
  tcase_add_test(tc, test_percent_decoding);
  tcase_add_test(tc, test_hash_decoding);
  tcase_add_test(tc, test_full_decoding);
  tcase_add_test(tc, test_control_character);
  tcase_add_test(tc, test_non_ascii);
  tcase_add_test(tc, test_too_long);
  tcase_add_test(tc, test_null_source);
  tcase_add_test(tc, test_zero_destination);
  tcase_add_test(tc, test_null_destination);

  suite_add_tcase(s, tc);
}
