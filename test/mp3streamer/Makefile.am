.NOTPARALLEL: mp3streamer

netembryoincludedir = $(top_srcdir)/include

bin_PROGRAMS = mp3streamer 

mp3streamer_SOURCES =	main.c \
			mpegaudio.c 

mp3streamer_LDADD =		$(top_srcdir)/rtp/librtp.la 
mp3streamer_LDADD +=		$(top_srcdir)/wsocket/libwsocket.la 

mp3streamer_LDADD += @GLIB_LIBS@


INCLUDES = -I$(netembryoincludedir) -I$(top_srcdir)

$(OBJECTS): libtool
libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck

