SUBDIRS =	wsocket

#		sound \
#		bufferpool \
#		randomize \
#		rtp mpglib \
#		mp3receiver \
#		mp3streamer \
#		mpeg12streamer \
#		src \
#		var 

ACLOCAL_AMFLAGS = -I m4

#if HAVE_VORBIS
#SUBDIRS += 	vorbis_receiver \
#		vorbis_streamer 
#endif
#if HAVE_THEORA
#SUBDIRS +=	theora_streamer \
#		theora_receiver
#endif

EXTRA_DIST = 	config \
		include 

lib_LTLIBRARIES = libnetembryo.la

libincludedir = $(includedir)/netembryo
libinclude_HEADERS =    include/netembryo/wsocket.h

libnetembryo_la_SOURCES =

libnetembryo_la_LIBADD =   wsocket/libwsocket.la

##fixme add the pkg-config
#pkgconfigdir = $(libdir)/pkgconfig
#pkgconfig_DATA = libnetembryo.pc

libnetembryo_la_LDFLAGS = -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE)

install-exec-hook:

dist-hook:
	@for svn in `find $(distdir) -iname .svn`; do \
		rm -rf $$svn; \
	done

uninstall-local:

	
distclean-local:
	@if test -d autom4te.cache; then \
		rm -rf autom4te.cache; \
	fi

MAINTAINER_FILES = acinclude.m4 aclocal.m4 config.h.in* configure Makefile.in mkinstalldirs error.log m4/* 
MAINTAINER_DIRS = config

maintainer-clean-local:
	@for mfile in $(MAINTAINER_FILES); do \
		test -f $$mfile && rm -f $$mfile; \
	done;\
	for mdir in $(MAINTAINER_DIRS); do \
		test -d $$mdir && rm -rf $$mdir; \
	done;\
	find -iname Makefile.in -exec rm \{} \;

svnclean: maintainer-clean
