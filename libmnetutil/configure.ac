dnl                                               -*- Autoconf -*-
dnl Process this file with autoconf to produce a configure script.

dnl AC_PREREQ(2.57)
AC_INIT(libmnetutil, 0.3, eliasson@imit.kth.se)
AC_CANONICAL_SYSTEM
dnl AC_CONFIG_HEADER([config.h])
dnl AM_CONFIG_HEADER([config.h])

AM_INIT_AUTOMAKE(libmnetutil, 0.3.1)


AM_CONFIG_HEADER(include/compilation_config.h)

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
AC_PROG_LIBTOOL

AC_LANG(C++)

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
    [ --enable-debug enables debug output (default disabled)],
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

dnl
dnl libmutil
dnl
AC_CHECK_HEADER(libmutil/itoa.h,,[AC_MSG_ERROR(["You need the libmutil
	headers/library. Try installing the libmutil-devel package for your
	distribution."])])
AC_CHECK_LIB([mutil], [main],
	[
		LIBS="-lmutil ${LIBS}"
	],[
		AC_MSG_ERROR([Could not find libmutil. Please install the corresponding package.])
	])
		

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

AC_OUTPUT([Makefile include/Makefile debian/Makefile])

