netembryoincludedir=$(top_srcdir)/include

lib_LTLIBRARIES = libmpglib.la

libmpglib_la_SOURCES = 	common.c \
		dct64_i386.c \
		decode_i386.c \
		layer1.c \
		layer2.c \
		layer3.c \
		tabinit.c \
		interface.c 

noinst_HEADERS =	huffman.h \
		l2tables.h \
		mpg123.h \
		mpglib.h 

libmpglib_la_LDFLAGS = -static 

INCLUDES = -I$(top_srcdir) -I$(netembryoincludedir)
