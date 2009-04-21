pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libnetembryo.pc

CLEANFILES = libnetembryo-sctp.pc libnetembryo-sctp-uninstalled.pc

libnetembryo-sctp.pc: libnetembryo.pc
	ln -s $< $@

libnetembryo-sctp-uninstalled.pc: libnetembryo-uninstalled.pc
	ln -s $< $@
