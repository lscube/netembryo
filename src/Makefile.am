.NOTPARALLEL: netembryo 

netembryoincludedir = $(top_srcdir)/include

bin_PROGRAMS = netembryo

netembryo_SOURCES =	main.c 

netembryo_LDADD =		$(top_srcdir)/wsocket/libwsocket.la 

netembryo_LDADD += @GLIB_LIBS@


INCLUDES = -I$(netembryoincludedir) -I$(top_srcdir)

$(OBJECTS): libtool
libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck

NETEMBRYO_BIN=@netembryo_bin@
