dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT([netembryo],[0.0.1],[federico.ridolfo@polito.it])
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_HEADERS([config.h])
AC_CANONICAL_TARGET([])
AM_INIT_AUTOMAKE(dist-bzip2 check-news)

AC_PREFIX_PROGRAM(netembryo)

test "x$prefix" = x. || test "x$prefix" = xNONE && prefix=/usr/local
test "x$exec_prefix" = xNONE && exec_prefix=$prefix
test "x$sysconfdir" = 'x${prefix}/etc' && sysconfdir="/etc"
test "x$localstatedir" = 'x${prefix}/var' && localstatedir="/var"
test "x$mandir" = 'x${prefix}/man' && mandir="${prefix}/share/man"

dnl AM_CONFIG_HEADER(config.h)


 

AC_ARG_ENABLE(debug,
[  --enable-debug          enable gcc dubugging flags [[default=no]]],,
        enable_debug="no")

AC_ARG_ENABLE(ipv6,
[  --enable-ipv6            enable IPv6 support [[default=autodetected]]],,
        enable_ipv6="yes")

AM_CONDITIONAL(HAVE_SSL, /bin/false)
AC_DEFUN([CHECK_SSL],
[AC_MSG_CHECKING(if ssl is wanted)
AC_ARG_WITH(ssl,
[  --with-ssl enable ssl [will check /usr/local/ssl
                            /usr/lib/ssl /usr/ssl /usr/pkg /usr/local /usr. [Default certificate = /var/local/lib/netembryo/certs/cert.pem]]
],
[   AC_MSG_RESULT(yes)
    for dir in $withval /usr/local/ssl /usr/lib/ssl /usr/ssl /usr/pkg /usr/local /usr; do
        ssldir="$dir"
        if test -f "$dir/include/openssl/ssl.h"; then
            found_ssl="yes";
	    CFLAGS="$CFLAGS -I$ssldir/include/openssl -DHAVE_SSL";
	    break;
        fi
        if test -f "$dir/include/ssl.h"; then
            found_ssl="yes";
	    CFLAGS="$CFLAGS -I$ssldir/include/openssl -DHAVE_SSL";
            break
        fi
    done
    if test x_$found_ssl != x_yes; then
        AC_MSG_ERROR(Cannot find ssl libraries)
	AC_DEFINE([HAVE_SSL],[0],[Disable SSl])
dnl	AC_DEFINE([HAVE_SSL],[0],[Disable SSl])
    else
        printf "OpenSSL found in $ssldir\n";
	LIBS="$LIBS -lssl -lcrypto";
        LDFLAGS="$LDFLAGS -L$ssldir/lib";
	AM_CONDITIONAL(HAVE_SSL, /bin/true)
    	AC_DEFINE([HAVE_SSL],[1],[Enable SSl])
    fi
dnl	AC_SUBST(HAVE_SSL)
],
[
    with_ssl="${localstatedir}/local/lib/netembryo/certs/cert.pem"
    AC_MSG_RESULT(no)
])
])dnl

dnl Check for programs.
AC_PROG_CC
AC_PROG_GCC_TRADITIONAL
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
dnl Check for LibTool Dynamic Loading
AC_PROG_LIBTOOL
LIBTOOL="$LIBTOOL --silent"
AC_SUBST([LIBTOOL_DEPS])

AC_SUBST(HAVE_SSL)

dnl Check for Endianess
AC_C_BIGENDIAN([BIGENDIAN="Big Endian"] AC_DEFINE([WORDS_BIGENDIAN], 1, [Define if manchine is big-endian]),[BIGENDIAN="Little Endian"] )

dnl Checks for libraries.

dnl Replace `main' with a function in -lm:
AC_CHECK_LIB(m, main)

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
AC_FUNC_ALLOCA
AC_FUNC_MEMCMP
AC_FUNC_MMAP
AC_FUNC_VPRINTF
AC_CHECK_FUNCS(socket getaddrinfo getopt_long gettimeofday uname)
CHECK_SSL
dnl CFLAGS = "$CFLAGS $SSL_CFLAG"
if test "x$with_ssl" = "xyes"; then
	netembryo_cert="${localstatedir}/local/lib/netembryo/certs/cert.pem"
else
	netembryo_cert=$with_ssl
fi
AC_SUBST(netembryo_cert)
netembryo_state_dir=${localstatedir}/local/lib/netembryo/certs
AC_SUBST(netembryo_state_dir)

AC_DEFINE_UNQUOTED(NETEMBRYO_CERT_FILE_DEFAULT, $netembryo_cert, [Define default file for Certificate (pem format)])
AC_DEFINE_UNQUOTED(NETEMBRYO_CERT_FILE_DEFAULT_STR, "$netembryo_cert", [Define default string file for Certificate (pem format])

AC_DEFINE_UNQUOTED(NETEMBRYO_STATE_DIR, $netembryo_state_dir, [Define default dir for Certificate (pem format)])
AC_DEFINE_UNQUOTED(NETEMBRYO_STATE_DIR_STR, "$netembryo_state_dir", [Define default string dir for Certificate (pem format)])

AM_PATH_GLIB_2_0(2.0.0,CFLAGS="$CFLAGS $GLIB_CFLAGS",AC_MSG_ERROR([NetEmbryo uses Glib. Please install it (you need devel files).]),)

dnl test enable
if test "x$enable_ipv6" = "xyes"; then
        AC_CHECK_TYPE(struct sockaddr_in6, AC_DEFINE([IPV6], 1, [Define IPv6 support]), , [#include <netinet/in.h>])
else
        AC_MSG_WARN([IPv6 disabled: check skipped.])
fi

AM_CONDITIONAL(NETEMBRYO_DEBUG, /bin/false)
if test "$enable_debug" = "yes"; then
        CFLAGS="$CFLAGS -g -ggdb"
        AM_CONDITIONAL(NETEMBRYO_DEBUG, /bin/true) \
        AC_DEFINE([ENABLE_DEBUG], 1,[Debug enabled])
        debug_msg="
   Debuging flags enabled: welcome developer!!!"
fi


case "$enable_warnings" in
        pedantic)
                CFLAGS="$CFLAGS -W -Wall -pedantic"
        ;;
        normal | yes)
                CFLAGS="$CFLAGS -W -Wall"
        ;;
        none | *)
                CFLAGS="$CFLAGS"
        ;;
esac

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

case "$enable_optimize" in
        poor)
                CFLAGS="$CFLAGS -O1"
        ;;
        full)
                CFLAGS="$CFLAGS -O3"
        ;;
        super)
                CFLAGS="$CFLAGS -O3 -funroll-loops -fstrength-reduce -fomit-frame-pointer -ffast-math -malign-functions=2 -malign-jumps=2 -malign-loops=2"
        ;;
        none)
                CFLAGS="$CFLAGS"
        ;;
        normal | *)
                CFLAGS="$CFLAGS -O2"
        ;;
esac

ARCH_DEFINES=""
case "$host" in
        i386-*-* | i486-*-* | i586-*-* | i686-*-* | i86pc-*-*)
                dnl CFLAGS="$CFLAGS -mcpu=i486"
                CFLAGS="$CFLAGS -march=i386"
                ARCH_DEFINES="-DI386_ASSEM"
                arch_type=ix86
        ;;
        ppc-*-*)
                arch_type=ppc
        ;;
        *-hpux-*)
                ARCH_DEFINES="-DHPUX"
        ;;
        *-sun-*)
                have_solaris=yes
        ;;
esac


AC_SUBST(ARCH_DEFINES)
AM_CONDITIONAL(ARCH_X86, test "x$arch_type" = "xix86")
AM_CONDITIONAL(ARCH_PPC, test "x$arch_type" = "xppc")
AM_CONDITIONAL(HAVE_SOLARIS, test "x$have_solaris" = xyes)

netembryo_bin=$PACKAGE_NAME
AC_SUBST(netembryo_bin)

AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([src/Makefile])
AC_CONFIG_FILES([mp3streamer/Makefile])
AC_CONFIG_FILES([mpeg12streamer/Makefile])
AC_CONFIG_FILES([rtp/Makefile])
AC_CONFIG_FILES([randomize/Makefile])
AC_CONFIG_FILES([wsocket/Makefile])
AC_CONFIG_FILES([var/Makefile])

AC_OUTPUT

ssl_msg="";
if test x_$found_ssl == x_yes; then
ssl_msg="libssl activeted ..................... : yes"
fi

AC_MSG_NOTICE([config files successfully generated!

NetEmbryo Install prefix .......... : $prefix
NetEmbryo Binary path ............. : $exec_prefix/bin/$netembyo_bin

Byte order ..................... : this is a $BIGENDIAN machine
$ssl_msg

$debug_msg

 'make' will now compile NetEmbryo and 'su -c make install' will install it.
 Note: On non-Linux systems you might need to use 'gmake' instead of 'make'.
])
dnl
