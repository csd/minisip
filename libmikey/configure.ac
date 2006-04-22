dnl                                               -*- Autoconf -*-
dnl Process this file with autoconf to produce a configure script.

dnl AC_PREREQ(2.57)

m4_define([mikey_major_version], [0])
m4_define([mikey_minor_version], [4])
m4_define([mikey_micro_version], [1])
m4_define([mikey_version],
          [mikey_major_version.mikey_minor_version.mikey_micro_version])

dnl
dnl Libtool style version info
dnl update before each new release 
dnl See node "Updating version info::" in info libtool
dnl
m4_define([lt_current], [0])
m4_define([lt_revision], [0])
m4_define([lt_age], [0])

AC_INIT(libmikey, [mikey_major_version.mikey_minor_version], jobi@via.ecp.fr)
AC_CONFIG_AUX_DIR(config)

AM_INIT_AUTOMAKE(libmikey, [mikey_version])
AM_MAINTAINER_MODE

AM_CONFIG_HEADER(include/compilation_config.h)

MIKEY_MAJOR_VERSION=mikey_major_version
MIKEY_MINOR_VERSION=mikey_minor_version
MIKEY_MICRO_VERSION=mikey_micro_version
LT_VERSION_INFO="lt_current:lt_revision:lt_age"
LT_CURRENT_MINUS_AGE=m4_eval(lt_current - lt_age)
AC_SUBST(MIKEY_MAJOR_VERSION)
AC_SUBST(MIKEY_MINOR_VERSION)
AC_SUBST(MIKEY_MICRO_VERSION)
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
AC_LANG(C++)

if test x$os_win = xyes; then
    AC_CHECK_TOOL(WINDRES, [windres])
    if test x$WINDRES = x; then
        AC_MSG_ERROR([Could not find windres in your PATH.])
    fi
fi

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
dnl Damn RedHat
AC_DEFINE(OPENSSL_NO_KRB5, [], [No Kerberos in OpenSSL])

dnl
dnl libmutil
dnl
PKG_CHECK_MODULES(MUTIL, [libmutil >= 0.3.1])

save_CPPFLAGS="${CPPFLAGS}"
save_LIBS="${LIBS}"
CPPFLAGS="${CPPFLAGS} ${MUTIL_CFLAGS}"
LIBS="${MUTIL_LIBS} ${LIBS}"

AC_CHECK_LIB([mutil], [main],
	[
		LIBS="-lmutil ${LIBS}"
	],[
		AC_MSG_ERROR([Could not find libmutil. Please install the corresponding package.])
	])

dnl libmutil headers check
AC_CHECK_HEADER(libmutil/itoa.h,,[AC_MSG_ERROR(["You need the libmutil headers/library. Try installing the libmutil-devel package for your distribution."])])

CPPFLAGS="${save_CPPFLAGS}"
LIBS="${save_LIBS}"

dnl Check for libssl 0.9.7
AC_CHECK_HEADERS([openssl/aes.h],[],[])

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([malloc.h stdlib.h string.h unistd.h])

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_HEADER_TIME


AC_CONFIG_FILES([Makefile include/Makefile debian/Makefile win32/Makefile libmikey.pc win32/libmikey-res.rc])
AC_OUTPUT
