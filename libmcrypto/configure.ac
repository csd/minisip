dnl                                               -*- Autoconf -*-
dnl Process this file with autoconf to produce a configure script.

dnl AC_PREREQ(2.57)
AC_INIT(libmcrypto, 0.1, jobi@via.ecp.fr)
AC_CONFIG_AUX_DIR(config)

AM_INIT_AUTOMAKE(libmcrypto, 0.2)

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


dnl STL
dnl For now STL is made mandatory

dnl AC_ARG_ENABLE(stl,
dnl     [ --enable-stl enables the use of C++ STL (default disabled)],
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


dnl Checks for libraries.
AC_CHECK_LIB([gcrypt], [main])

AC_CHECK_LIB([gnutls], [main])


dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([malloc.h stdlib.h string.h unistd.h gnutls/x509.h])

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

AC_OUTPUT([Makefile include/Makefile])

