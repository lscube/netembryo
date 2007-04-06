.NOTPARALLEL: vorbis_streamer

netembryoincludedir = $(top_srcdir)/include

bin_PROGRAMS = vorbis_streamer 

vorbis_streamer_SOURCES =	main.c \
				vorbis.c

vorbis_streamer_LDADD =		$(top_srcdir)/rtp/librtp.la 
vorbis_streamer_LDADD +=	$(top_srcdir)/wsocket/libwsocket.la 

vorbis_streamer_LDADD += @GLIB_LIBS@ @VORBIS_LIBS@


INCLUDES = -I$(netembryoincludedir) -I$(top_srcdir)

$(OBJECTS): libtool
libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck

all:
	@echo incomplete for now
