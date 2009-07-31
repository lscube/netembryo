dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)

dnl Process this file with autoconf to produce a configure script.
dnl
dnl immediately before every release do:
dnl ===================================
dnl if (the interface is totally unchanged from previous release)
dnl    *_lt_revision ++;
dnl else { /* interfaces have been added, removed or changed */
dnl    *_lt_revision = 0;
dnl    *_lt_current ++;
dnl    if (any interfaces have been _added_ since last release)
dnl       *_lt_age ++;
dnl    if (any interfaces have been _removed_ or _incompatibly changed_)
dnl       *_lt_age = 0;
dnl }
dnl
dnl If you want to know more about what you are doing, here are some details:
dnl  * *_lt_current is the current API version
dnl  * *_lt_revision is an internal revision number which is increased when
dnl    the API itself did not change
dnl  * *_lt_age is the number of previous API versions still supported by
dnl    this library
dnl  * libtool has its own numbering scheme, because local library numbering
dnl    schemes are platform dependent
dnl  * in Linux, the library will be named
dnl    libname.so.(*_lt_current - *_lt_age).*_lt_age.*_lt_revision

m4_define([netembryo_major_version], [0])
m4_define([netembryo_minor_version], [1])
m4_define([netembryo_micro_version], [1])
m4_define([netembryo_interface_age], [0])
m4_define([netembryo_binary_age],
          [m4_eval(100 * netembryo_minor_version + netembryo_micro_version)])
m4_define([netembryo_version],
          [netembryo_major_version.netembryo_minor_version.netembryo_micro_version])

m4_define([netembryo_lt_revision], [0])
m4_define([netembryo_lt_current], [9])
m4_define([netembryo_lt_age],     [0])

AC_INIT([netembryo], [netembryo_version], [http://www.lscube.org])

AC_CONFIG_AUX_DIR(config)
AC_CONFIG_MACRO_DIR(m4)
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([dist-bzip2 no-dist-gzip foreign subdir-objects])
LSC_INIT

dnl Check for LibTool Dynamic Loading
AC_PROG_LIBTOOL
LIBTOOL="$LIBTOOL --silent"
AC_SUBST([LIBTOOL_DEPS])

LSC_CHECK_IPV6
LSC_CHECK_SCTP

PKG_WITH_MODULES([OPENSSL], [openssl >= 0.9.8],
  [have_ssl="yes";
   OPENSSL_REQUIRES="openssl >= 0.9.8";
   AC_DEFINE(ENABLE_SSL, 1, [Define if ssl is enabled])], [have_ssl="no"],
  [Enable SSL support through OpenSSL], [yes])

AC_SUBST(OPENSSL_REQUIRES)
AM_CONDITIONAL(ENABLE_SSL, [test "$have_ssl" = "yes"])

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
AC_CONFIG_FILES([Doxyfile])

AC_OUTPUT

LSC_DEBUG_STATUS
AC_MSG_NOTICE([config files successfully generated!
NetEmbryo version is .............. : $VERSION
NetEmbryo Install prefix .......... : $prefix
NetEmbryo sctp support enabled .... : $have_sctp
NetEmbryo ssl support enabled ..... : $have_ssl
NetEmbryo tests ................... : $have_tests

 'make' will now compile NetEmbryo and 'su -c make install' will install it.
 Note: On non-Linux systems you might need to use 'gmake' instead of 'make'.
])
dnl
