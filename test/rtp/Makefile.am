
netembryoincludedir = $(top_srcdir)/include

noinst_LTLIBRARIES = librtp.la

librtp_la_SOURCES = 	rtcp-packet.c \
			rtp-packet.c
librtp_la_LDFLAGS = -static 

INCLUDES = -I$(netembryoincludedir) -I$(top_srcdir) 
