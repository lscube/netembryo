
netembryoincludedir = $(top_srcdir)/netembryo

noinst_LTLIBRARIES = libwsocket.la

libwsocket_la_LDFLAGS = 

libwsocket_la_SOURCES =		multicast.c \
				socket.c \
				sock_ntop_host.c \
				get_info.c \
				Sock.c \
				wsocket-internal.h

if ENABLE_SSL
libwsocket_la_SOURCES += ssl.c ssl.h
endif

INCLUDES = -I$(netembryoincludedir) -I$(top_srcdir)
