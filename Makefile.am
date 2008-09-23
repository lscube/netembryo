SUBDIRS =	wsocket \
		utils

EXTRA_DIST = Doxyfile

lib_LTLIBRARIES = libnetembryo.la

pkginclude_HEADERS =    netembryo/wsocket.h \
			netembryo/url.h \
			netembryo/rtsp_errors.h

libnetembryo_la_SOURCES =

libnetembryo_la_LIBADD =   wsocket/libwsocket.la
libnetembryo_la_LIBADD +=   utils/libutils.la

#pkg-config
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = pkgconfig/libnetembryo.pc

libnetembryo_la_LDFLAGS = -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE)

ACLOCAL_AMFLAGS = -I m4