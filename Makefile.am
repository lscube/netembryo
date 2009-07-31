SUBDIRS =	pkgconfig \
		. \
		tests

EXTRA_DIST = Doxyfile

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

if ENABLE_SSL
libnetembryo_la_SOURCES += \
			wsocket/ssl.c
			wsocket/ssl.h
libnetembryo_la_CFLAGS += $(OPENSSL_CFLAGS)
libnetembryo_la_LIBADD += $(OPENSSL_LIBS)
endif

if HAVE_SCTP
libnetembryo_la_LIBADD += $(SCTP_LIBS)
endif

libnetembryo_la_LDFLAGS = -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE)

ACLOCAL_AMFLAGS = -I m4
