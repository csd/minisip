dnl                                               -*- Autoconf -*-
dnl Process this file with autoconf to produce a configure script.

dnl AC_PREREQ(2.57)

m4_define([mnetutil_major_version], [0])
m4_define([mnetutil_minor_version], [3])
m4_define([mnetutil_micro_version], [1])
m4_define([mnetutil_version],
          [mnetutil_major_version.mnetutil_minor_version.mnetutil_micro_version])

dnl
dnl Libtool style version info
dnl update before each new release 
dnl See node "Updating version info::" in info libtool
dnl
m4_define([lt_current], [0])
m4_define([lt_revision], [0])
m4_define([lt_age], [0])

AC_INIT(libmnetutil, [mnetutil_major_version.mnetutil_minor_version],
        eliasson@imit.kth.se)
AC_CONFIG_AUX_DIR(config)
AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE(libmnetutil, [mnetutil_version])
AM_MAINTAINER_MODE

AM_CONFIG_HEADER(include/compilation_config.h)

MNETUTIL_MAJOR_VERSION=mnetutil_major_version
MNETUTIL_MINOR_VERSION=mnetutil_minor_version
MNETUTIL_MICRO_VERSION=mnetutil_micro_version
LT_VERSION_INFO="lt_current:lt_revision:lt_age"
LT_CURRENT_MINUS_AGE=m4_eval(lt_current - lt_age)
AC_SUBST(MNETUTIL_MAJOR_VERSION)
AC_SUBST(MNETUTIL_MINOR_VERSION)
AC_SUBST(MNETUTIL_MICRO_VERSION)
AC_SUBST(LT_VERSION_INFO)
AC_SUBST(LT_CURRENT_MINUS_AGE)

case $host_os in
     *mingw* )
	     os_win=yes
	     ;;
esac

AM_CONDITIONAL(OS_WIN, test x$os_win = xyes)

dnl Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
dnl AC_PROG_LEX
dnl AM_PROG_LEX
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_YACC
dnl AC_PROG_RANLIB
AC_LIBTOOL_WIN32_DLL
AC_PROG_LIBTOOL

AC_LANG(C++)

if test x$os_win = xyes; then
    AC_CHECK_TOOL(WINDRES, [windres])
    if test x$WINDRES = x; then
        AC_MSG_ERROR([Could not find windres in your PATH.])
    fi
fi

AC_C_BIGENDIAN

dnl STL
dnl For now STL is made mandatory

dnl AC_ARG_ENABLE(stl,
dnl     [ --enable-stl enables the use of C++ STL (default enabled)],
dnl     [ if test "${enable_stl}" = "yes"
dnl       then
        AC_DEFINE(USE_STL, [], [STL enabled])
dnl       fi ])

dnl debug flag
AC_ARG_ENABLE(debug,
    AS_HELP_STRING([--enable-debug],
        [enables debug output (default disabled)]),
    [ if test "${enable_debug}" = "yes"
      then
        AC_DEFINE(DEBUG_OUTPUT, [], [Debug output])
      else
      	AC_DEFINE(NDEBUG, [], [No debug output])
      fi ])

dnl
dnl IPv6 support
dnl
AC_CHECK_HEADERS([netinet/in.h ws2tcpip.h])
AC_CHECK_TYPES([struct sockaddr_in6],,,[
#ifdef HAVE_NETINET_IN_H
# include <netinet/in.h>
#endif
#ifdef HAVE_WS2TCPIP_H
# include <ws2tcpip.h>
#endif
])

dnl
dnl mingw support
dnl
AC_CHECK_LIB([wsock32], [main])
AC_CHECK_LIB([iphlpapi], [main])
AC_CHECK_LIB([dnsapi], [main])

dnl OpenSSL libcrypto
dnl library check
AC_CHECK_LIB([crypto], [SSLeay],
	[
		LIBS="-lcrypto ${LIBS}"
	],[
		AC_MSG_ERROR([Could not find libcrypto. Please install the corresponding package (provided by the openssl project).])
	])
dnl header check
AC_CHECK_HEADER([openssl/crypto.h],[],
	[
		AC_MSG_ERROR([Could not the development files for the libcrypto library. Please install the corresponding package (provided by the openssl project).])
	])

dnl
dnl libssl
dnl
dnl Damn RedHat
AC_DEFINE(OPENSSL_NO_KRB5, [], [No Kerberos in OpenSSL])
AC_CHECK_LIB([ssl], [SSL_library_init],
	[
		LIBS="-lssl ${LIBS}"
	],[
		AC_MSG_ERROR([Could not find libssl. Please install the corresponding package.])
	])
AC_CHECK_HEADER([openssl/ssl.h], [],
	[
		AC_MSG_ERROR([Could not find libssl header files. Please install the corresponding development package.])
	])

MINISIP_CHECK_LIBMUTIL([0.3.1])

AC_CHECK_LIB([resolv], [main])

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([malloc.h stdlib.h string.h unistd.h sys/types.h sys/socket.h netdb.h netinet/in.h arpa/inet.h netinet/tcp.h linux/sockios.h arpa/nameser_compat.h])

dnl Checks for typedefs, structures, and compiler characteristics.
dnl AC_HEADER_STDBOOL
AC_C_CONST
AC_HEADER_TIME
dnl AC_C_VOLATILE

dnl Checks for library functions.
dnl AC_FUNC_ERROR_AT_LINE
dnl AC_PROG_GCC_TRADITIONAL
dnl AC_FUNC_MALLOC
dnl AC_TYPE_SIGNAL
dnl AC_CHECK_FUNCS([bzero gettimeofday inet_ntoa memmove memset pow socket sqrt strtol])

dnl AC_SUBST(CPPFLAGS)
dnl AC_SUBST(LDFLAGS)

AC_CONFIG_FILES([Makefile include/Makefile debian/Makefile win32/Makefile libmnetutil.pc win32/libmnetutil-res.rc])
AC_OUTPUT
