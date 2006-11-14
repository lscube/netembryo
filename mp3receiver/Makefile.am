.NOTPARALLEL: mp3receiver

netembryoincludedir = $(top_srcdir)/include

bin_PROGRAMS = mp3receiver 

mp3receiver_SOURCES =	main.c \
			read_side.c \
			write_side.c

mp3receiver_LDADD =		$(top_srcdir)/rtp/librtp.la 
mp3receiver_LDADD +=		$(top_srcdir)/wsocket/libwsocket.la 
mp3receiver_LDADD +=		$(top_srcdir)/mpglib/libmpglib.la 
mp3receiver_LDADD +=		$(top_srcdir)/bufferpool/libbuffpool.la 
mp3receiver_LDADD +=		$(top_srcdir)/sound/libsound.la 

mp3receiver_LDADD += @GLIB_LIBS@


INCLUDES = -I$(netembryoincludedir) -I$(top_srcdir)

$(OBJECTS): libtool
libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck

