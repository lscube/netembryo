dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)

m4_define([netembryo_major_version], [0])
m4_define([netembryo_minor_version], [0])
m4_define([netembryo_micro_version], [8])
m4_define([netembryo_interface_age], [0])
m4_define([netembryo_binary_age],
          [m4_eval(100 * netembryo_minor_version + netembryo_micro_version)])
m4_define([netembryo_version],
          [netembryo_major_version.netembryo_minor_version.netembryo_micro_version])
m4_define([netembryo_lt_current],
          [m4_eval(100 * netembryo_minor_version + netembryo_micro_version - netembryo_interface_age)])
m4_define([netembryo_lt_revision], [netembryo_interface_age])
m4_define([netembryo_lt_age], [m4_eval(netembryo_binary_age - netembryo_interface_age)])

AC_INIT([netembryo], [netembryo_version], [http://streaming.polito.it])

AC_CONFIG_AUX_DIR(config)
AC_CONFIG_MACRO_DIR(m4)
AC_CONFIG_HEADERS([config.h])
AC_CANONICAL_HOST
AM_INIT_AUTOMAKE(dist-bzip2 no-dist-gzip foreign)

AC_PREFIX_DEFAULT("/usr")

dnl Check for LibTool Dynamic Loading
AC_PROG_LIBTOOL
LIBTOOL="$LIBTOOL --silent"
AC_SUBST([LIBTOOL_DEPS])

dnl Check for Endianess
AC_C_BIGENDIAN([BIGENDIAN="Big Endian"] AC_DEFINE([WORDS_BIGENDIAN], 1, [Define if manchine is big-endian]),[BIGENDIAN="Little Endian"] )

dnl Checks for header files.
AC_HEADER_STDC
AC_HEADER_TIME
AC_CHECK_HEADERS(sys/errno.h fcntl.h getopt.h sys/time.h unistd.h)

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T
AC_C_INLINE
AC_CHECK_TYPES(struct sockaddr_storage,,, [#include <sys/socket.h>])

#init CFLAG and PKG_LIB
AC_SUBST(PACKAGE_CFLAGS)
AC_SUBST(PACKAGE_LIBS)

dnl Checks for library functions.
AC_FUNC_MEMCMP
AC_CHECK_FUNCS(socket getaddrinfo getopt_long gettimeofday uname)

LSC_CHECK_IPV6
LSC_CHECK_SCTP

LSC_ERRORS
LSC_DEBUG

PKG_WITH_MODULES([OPENSSL], [openssl >= 0.9.8],
  [have_ssl="yes";
   OPENSSL_REQUIRES="openssl >= 0.9.8";
   AC_DEFINE(ENABLE_SSL, 1, [Define if ssl is enabled])], [have_ssl="no"],
  [Enable SSL support through OpenSSL], [yes])

AC_SUBST(OPENSSL_REQUIRES)
AM_CONDITIONAL(ENABLE_SSL, [test "$have_ssl" = "yes"])

PKG_HAVE_WITH_MODULES([CHECK], [check],
  [Enable unit testing with 'check'], [yes])

LT_CURRENT=netembryo_lt_current
LT_REVISION=netembryo_lt_revision
LT_AGE=netembryo_lt_age

AC_SUBST(LT_CURRENT)
AC_SUBST(LT_REVISION)
AC_SUBST(LT_AGE)
AC_SUBST(LIBS)

AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([wsocket/Makefile])
AC_CONFIG_FILES([utils/Makefile])
AC_CONFIG_FILES([tests/Makefile])
AC_CONFIG_FILES([pkgconfig/Makefile])
AC_CONFIG_FILES([pkgconfig/libnetembryo.pc])
AC_CONFIG_FILES([pkgconfig/libnetembryo-uninstalled.pc])

AC_OUTPUT

LSC_DEBUG_STATUS
AC_MSG_NOTICE([config files successfully generated!
Byte order ........................ : this is a $BIGENDIAN machine
NetEmbryo version is .............. : $VERSION
NetEmbryo Install prefix .......... : $prefix
NetEmbryo sctp support enabled .... : $enable_sctp
NetEmbryo ssl support enabled ..... : $have_ssl

 'make' will now compile NetEmbryo and 'su -c make install' will install it.
 Note: On non-Linux systems you might need to use 'gmake' instead of 'make'.
])
dnl
