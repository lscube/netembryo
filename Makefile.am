EXTRA_DIST = Doxyfile

dist_doc_DATA = AUTHORS ChangeLog README TODO

if CHANGELOG_REGEN
ChangeLog:
	-$(AM_V_GEN)GIT_DIR="${srcdir}/.git" $(GIT) log --pretty > $@

# Always regenerate!
.PHONY: ChangeLog
endif

distcheck-hook:
	@test -s "${srcdir}"/ChangeLog || { echo "distcheck: empty ChangeLog found; make sure you have GIT installed" >/dev/stderr; false; }

lib_LTLIBRARIES = libnetembryo.la

pkginclude_HEADERS =    netembryo/wsocket.h

libnetembryo_la_SOURCES = \
			utils/log.c \
			\
			wsocket/wsocket-internal.h \
			wsocket/Sock.c

EXTRA_DIST += wsocket/socket.c wsocket/sock_ntop_host.c

libnetembryo_la_CFLAGS = $(GLIB_CFLAGS)
libnetembryo_la_LIBADD = $(GLIB_LIBS)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = pkgconfig/libnetembryo.pc

noinst_DATA =

pkgconfig/libnetembryo-sctp%: pkgconfig/libnetembryo%
	@$(mkdir_p) $(dir $@)
	$(AM_V_GEN)$(LN_S) -f $(notdir $<) $@

if ENABLE_SCTP
libnetembryo_la_LIBADD += $(SCTP_LIBS)

pkgconfig_DATA += pkgconfig/libnetembryo-sctp.pc
noinst_DATA += pkgconfig/libnetembryo-sctp-uninstalled.pc
endif

libnetembryo_la_LDFLAGS = -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE)

ACLOCAL_AMFLAGS = -I m4

if BUILD_TESTS
TESTS = tests/testsuite

check_PROGRAMS = $(TESTS)

BUILT_SOURCES = tests/gtestmain.c
endif

tests_testsuite_SOURCES = \
	tests/test_sock.c \
	tests/utils.c \
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
