
netembryoincludedir = $(top_srcdir)/include

noinst_LTLIBRARIES = libwsocket.la

libwsocket_la_LDFLAGS = 

libwsocket_la_SOURCES =		gethostinfo.c \
				multicast.c \
				socket.c \
				sock_ntop_host.c \
				get_info.c \
				Sock.c

#if HAVE_SSL 
#libwsocket_la_LDFLAGS += $(SSL_LDFLAGS)
#libwsocket_la_LIBADD  = $(SSL_LIBS)
#endif

INCLUDES = -I$(netembryoincludedir) -I$(top_srcdir)

#if HAVE_SSL
#INCLUDES += $(SSL_CFLAG)
#endif
