.NOTPARALLEL: mpeg12streamer

netembryoincludedir = $(top_srcdir)/include

bin_PROGRAMS = mpeg12streamer 

mpeg12streamer_SOURCES =	main.c \
				mpeg12.c 

mpeg12streamer_LDADD =		$(top_srcdir)/wsocket/libwsocket.la 

mpeg12streamer_LDADD += @GLIB_LIBS@


INCLUDES = -I$(netembryoincludedir) -I$(top_srcdir)

$(OBJECTS): libtool
libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck

