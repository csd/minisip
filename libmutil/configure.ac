dnl                                               -*- Autoconf -*-
dnl Process this file with autoconf to produce a configure script.

dnl AC_PREREQ(2.57)

m4_define([mutil_major_version], [0])
m4_define([mutil_minor_version], [3])
m4_define([mutil_micro_version], [1])
m4_define([mutil_version],
          [mutil_major_version.mutil_minor_version.mutil_micro_version])

dnl
dnl Libtool style version info
dnl update before each new release 
dnl See node "Updating version info::" in info libtool
dnl
m4_define([lt_current], [0])
m4_define([lt_revision], [0])
m4_define([lt_age], [0])

AC_INIT(libmutil, [mutil_major_version.mutil_minor_version],
        eliasson@imit.kth.se)
dnl AC_CONFIG_HEADER([config.h])
dnl AM_CONFIG_HEADER([config.h])

AM_INIT_AUTOMAKE([libmutil], [mutil_version])

AM_CONFIG_HEADER(include/compilation_config.h)

MUTIL_MAJOR_VERSION=mutil_major_version
MUTIL_MINOR_VERSION=mutil_minor_version
MUTIL_MICRO_VERSION=mutil_micro_version
LT_VERSION_INFO="lt_current:lt_revision:lt_age"
LT_CURRENT_MINUS_AGE=m4_eval(lt_current - lt_age)
AC_SUBST(MUTIL_MAJOR_VERSION)
AC_SUBST(MUTIL_MINOR_VERSION)
AC_SUBST(MUTIL_MICRO_VERSION)
AC_SUBST(LT_VERSION_INFO)
AC_SUBST(LT_CURRENT_MINUS_AGE)

AC_CANONICAL_HOST
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
    [ --enable-debug enables debug output (default disabled)],
    [ if test "${enable_debug}" = "yes"
      then
        AC_DEFINE(DEBUG_OUTPUT, [], [Debug output])
      else
      	AC_DEFINE(NDEBUG, [], [No debug output])
      fi ])

dnl debug flag
AC_ARG_ENABLE(memdebug,
    [ --enable-memdebug enables memory debugging code. (default disabled)],
    [ if test "${enable_memdebug}" = "yes"
      then
        AC_DEFINE(MDEBUG, [], [Debug memory])
      fi ])



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
dnl REDHAT fix
AC_DEFINE(OPENSSL_NO_KRB5, [], [No Kerberos in OpenSSL])
dnl Check for AES support.
AC_CHECK_HEADER([openssl/aes.h],[have_aes_h=yes])
if test x$have_aes_h = xyes; then
   HAVE_OPENSSL_AES_H="#define LIBMUTIL_HAVE_OPENSSL_AES_H 1"
   AC_DEFINE([HAVE_OPENSSL_AES_H], 1, [Define to 1 if you have the <openssl/aes.h> header file.])
else
   HAVE_OPENSSL_AES_H="/* #undef LIBMUTIL_HAVE_OPENSSL_AES_H */"
fi
AC_SUBST(HAVE_OPENSSL_AES_H)


dnl FIXME: Replace `main' with a function in `-lpthread':
AC_CHECK_LIB([pthread], [pthread_attr_init])
dnl FIXME: Replace `main' with a function in `-ldl':
AC_CHECK_LIB([dl], [dlopen])


dnl Check for stack trace support
AC_CHECK_HEADER([execinfo.h],
	[
		AC_DEFINE(HAVE_EXECINFO_H, [], [Stack trace functionality found])
		HAVE_EXECINFO_H="yes"
	]
)
AM_CONDITIONAL(HAVE_EXECINFO_H, test "${HAVE_EXECINFO}" = "yes")


dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([malloc.h stdlib.h string.h unistd.h pthread.h termios.h semaphore.h])

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

AC_CONFIG_FILES([Makefile include/Makefile debian/Makefile win32/Makefile libmutil.pc win32/libmutil-res.rc])
AC_OUTPUT

