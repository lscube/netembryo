
netembryoincludedir = $(top_srcdir)/include

noinst_LTLIBRARIES = libwsocket.la

libwsocket_la_LDFLAGS = 

libwsocket_la_SOURCES =		gethostinfo.c \
				multicast.c \
				sock_accept.c \
				sock_bind.c \
				sock_close.c \
				sock_connect.c \
				sock_get_port.c \
				sock_listen.c \
				sock_ntop_host.c \
				sockfd_to_family.c \
				get_info.c \
				Sock.c

if HAVE_SSL
libwsocket_la_SOURCES +=	create_ssl_ctx.c \
				get_ssl_connection.c \
				sock_SSL_accept.c \
				sock_SSL_close.c \
				sock_SSL_connect.c \
				sock_SSL_read.c \
				sock_SSL_write.c

endif

#if HAVE_SSL 
#libwsocket_la_LDFLAGS += $(SSL_LDFLAGS)
#libwsocket_la_LIBADD  = $(SSL_LIBS)
#endif

INCLUDES = -I$(netembryoincludedir) -I$(top_srcdir)

#if HAVE_SSL
#INCLUDES += $(SSL_CFLAG)
#endif
