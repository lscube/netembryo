EXTRA_DIST = Doxyfile

dist_doc_DATA = AUTHORS ChangeLog README TODO

if CHANGELOG_REGEN
ChangeLog:
	$(AM_V_GEN)GIT_DIR="${srcdir}/.git" git log --pretty > $@

# Always regenerate!
.PHONY: ChangeLog
endif

lib_LTLIBRARIES = libnetembryo.la

pkginclude_HEADERS =    netembryo/wsocket.h \
			netembryo/wsocket-internal.h \
			netembryo/url.h \
			netembryo/rtsp.h

libnetembryo_la_SOURCES = \
			utils/url.c \
			utils/rtsp.c \
			\
			wsocket/multicast.c \
			wsocket/socket.c \
			wsocket/sock_ntop_host.c \
			wsocket/get_info.c \
			wsocket/Sock.c

libnetembryo_la_CFLAGS = $(GLIB_CFLAGS)
libnetembryo_la_LIBADD = $(GLIB_LIBS)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = pkgconfig/libnetembryo.pc

noinst_DATA =

pkgconfig/libnetembryo-sctp%: pkgconfig/libnetembryo%
	@$(mkdir_p) $(dir $@)
	$(AM_V_GEN)$(LN_S) -f $(notdir $<) $@

if ENABLE_SSL
libnetembryo_la_SOURCES += \
			wsocket/ssl.c \
			wsocket/ssl.h
libnetembryo_la_CFLAGS += $(OPENSSL_CFLAGS)
libnetembryo_la_LIBADD += $(OPENSSL_LIBS)
endif

if HAVE_SCTP
libnetembryo_la_LIBADD += $(SCTP_LIBS)

pkgconfig_DATA += pkgconfig/libnetembryo-sctp.pc
noinst_DATA += pkgconfig/libnetembryo-sctp-uninstalled.pc
endif

libnetembryo_la_LDFLAGS = -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE)

ACLOCAL_AMFLAGS = -I m4

if BUILD_TESTS
TESTS = tests/testsuite

check_PROGRAMS = $(TESTS)
endif

BUILT_SOURCES = tests/gtestmain.c

tests_testsuite_SOURCES = \
	tests/test_hostname.c \
	tests/test_url.c \
	tests/test_sock.c \
	tests/gtest-extra.h
tests_testsuite_CFLAGS = $(GTESTER_CFLAGS)
tests_testsuite_LDADD = libnetembryo.la $(GTESTER_LIBS)

dist_tests_testsuite_SOURCES = tests/gtestmain.c

tests/gtestmain.c: $(tests_testsuite_SOURCES)
	@$(mkdir_p) $(dir $@)
	$(AM_V_GEN)$(EXUBERANT_CTAGS) -f - --file-scope=no $^ | \
		sort -k 2 | $(EGREP) '^test_' | \
		$(GAWK) -f $(srcdir)/tests/genmain.awk \
		> $@

DISTCLEANFILES = $(BUILT_SOURCES)
CLEANFILES = libnetembryo-sctp.pc libnetembryo-sctp-uninstalled.pc
