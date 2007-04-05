dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT([netembryo],[0.0.1],[team@streaming.polito.it])
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_HEADERS([config.h])
AC_CANONICAL_TARGET([])
AM_INIT_AUTOMAKE(dist-bzip2 check-news)

AC_PREFIX_DEFAULT("/usr")

AC_ARG_ENABLE(debug,
[  --enable-debug          enable gcc dubugging flags [[default=no]]],,
        enable_debug="no")

AC_ARG_ENABLE(ipv6,
[  --enable-ipv6            enable IPv6 support [[default=autodetected]]],,
        enable_ipv6="yes")

dnl Check for LibTool Dynamic Loading
m4_undefine([AC_PROG_CXX])
m4_defun([AC_PROG_CXX],[])
m4_undefine([AC_PROG_F77])
m4_defun([AC_PROG_F77],[])
AC_PROG_LIBTOOL
LIBTOOL="$LIBTOOL --silent"
AC_SUBST([LIBTOOL_DEPS])

AC_SUBST(HAVE_SSL)

dnl Check for Endianess
AC_C_BIGENDIAN([BIGENDIAN="Big Endian"] AC_DEFINE([WORDS_BIGENDIAN], 1, [Define if manchine is big-endian]),[BIGENDIAN="Little Endian"] )

dnl Checks for libraries.
AC_CHECK_LIB(pthread,pthread_create)

AC_CHECK_LIBM

dnl Checks for header files.
AC_HEADER_STDC
AC_HEADER_TIME
AC_CHECK_HEADERS(sys/errno.h fcntl.h getopt.h sys/time.h unistd.h)

dnl soundcard for sound oss module, used in mp3receiver
AC_CHECK_HEADER(linux/soundcard.h)

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T
AC_C_INLINE
AC_CHECK_TYPES(struct sockaddr_storage,,, [#include <sys/socket.h>])

#init CFLAG and PKG_LIB
AC_SUBST(PACKAGE_CFLAGS)
AC_SUBST(PACKAGE_LIBS)

dnl Checks for library functions.
AC_FUNC_ALLOCA
AC_FUNC_MEMCMP
AC_FUNC_MMAP
AC_FUNC_VPRINTF
AC_CHECK_FUNCS(socket getaddrinfo getopt_long gettimeofday uname)

dnl test enable
if test "x$enable_ipv6" = "xyes"; then
        AC_CHECK_TYPE(struct sockaddr_in6, AC_DEFINE([IPV6], 1, [Define IPv6 support]), , [#include <netinet/in.h>])
else
        AC_MSG_WARN([IPv6 disabled: check skipped.])
fi

AM_CONDITIONAL(NETEMBRYO_DEBUG, /bin/false)
if test "x$enable_debug" = "xyes"; then
        CFLAGS="$CFLAGS -g -ggdb"
        AM_CONDITIONAL(NETEMBRYO_DEBUG, /bin/true) \
        AC_DEFINE([ENABLE_DEBUG], 1,[Debug enabled])
        debug_msg="
   Debuging flags enabled: welcome developer!!!"
fi

case "$enable_errors" in
        pedantic)
                CFLAGS="$CFLAGS -pedantic-errors -Werror"
        ;;
        normal | yes)
                CFLAGS="$CFLAGS -Werror"
        ;;
        none | *)
                CFLAGS="$CFLAGS"
        ;;
esac

AM_CONDITIONAL(HAVE_SSL, /bin/false)

netembryo_bin=$PACKAGE_NAME
AC_SUBST(netembryo_bin)

AC_CONFIG_FILES([Makefile])
#AC_CONFIG_FILES([src/Makefile])
#AC_CONFIG_FILES([bufferpool/Makefile])
#AC_CONFIG_FILES([sound/Makefile])
#AC_CONFIG_FILES([mpglib/Makefile])
#AC_CONFIG_FILES([mp3streamer/Makefile])
#AC_CONFIG_FILES([mp3receiver/Makefile])
#AC_CONFIG_FILES([vorbis_streamer/Makefile])
#AC_CONFIG_FILES([vorbis_receiver/Makefile])
#AC_CONFIG_FILES([mpeg12streamer/Makefile])
#AC_CONFIG_FILES([rtp/Makefile])
#AC_CONFIG_FILES([randomize/Makefile])
AC_CONFIG_FILES([wsocket/Makefile])
#AC_CONFIG_FILES([var/Makefile])

AC_OUTPUT

AC_MSG_NOTICE([config files successfully generated!

NetEmbryo Install prefix .......... : $prefix
NetEmbryo Binary path ............. : $exec_prefix/bin/$netembyo_bin

Byte order ..................... : this is a $BIGENDIAN machine

$debug_msg

 'make' will now compile NetEmbryo and 'su -c make install' will install it.
 Note: On non-Linux systems you might need to use 'gmake' instead of 'make'.
])
dnl
