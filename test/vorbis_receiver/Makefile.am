.NOTPARALLEL: vorbis_receiver

netembryoincludedir = $(top_srcdir)/include

bin_PROGRAMS = vorbis_receiver

vorbis_receiver_SOURCES =	main.c \
				read_side.c \
				write_side.c 


vorbis_receiver_LDADD =		$(top_srcdir)/rtp/librtp.la 
vorbis_receiver_LDADD +=	$(top_srcdir)/wsocket/libwsocket.la 
vorbis_receiver_LDADD +=	$(top_srcdir)/bufferpool/libbuffpool.la 
# vorbis_receiver_LDADD +=	$(top_srcdir)/xiph_rtp/libxiph_rtp.la

if !ENABLE_LIBAO
vorbis_receiver_LDADD +=		$(top_srcdir)/sound/libsound.la 
endif

vorbis_receiver_LDADD += @GLIB_LIBS@ @VORBIS_LIBS@


INCLUDES = -I$(netembryoincludedir) -I$(top_srcdir)

$(OBJECTS): libtool
libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck

all:
	@echo Incomplete!
