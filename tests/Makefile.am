if HAVE_CHECK
TESTS = testsuite

check_PROGRAMS = $(TESTS)
endif

INCLUDES = -I$(top_srcdir) $(CHECK_CFLAGS)
LDADD = ../libnetembryo.la $(CHECK_LIBS)

testsuite_SOURCES = \
	test_hostname.c \
	test_url.c \
	test_sock.c \
	suite_main.c
