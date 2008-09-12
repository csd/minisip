dnl                                               -*- Autoconf -*-
dnl Process this file with autoconf to produce a configure script.

dnl AC_PREREQ(2.57)

m4_MINISIP_PACKAGE_VERSION(libminisip-swig, minisip_swig, 0, 1, 0)
m4_MINISIP_PACKAGE_CONTACT([Mikael Magnusson], [mikma@users.sourceforge.net])

AC_INIT(MINISIP_PACKAGE_NAME, MINISIP_PACKAGE_VERSION, MINISIP_PACKAGE_CONTACT)
AC_CONFIG_AUX_DIR(config)
AM_INIT_AUTOMAKE(MINISIP_PACKAGE_NAME, MINISIP_PACKAGE_VERSION)
AM_MAINTAINER_MODE
AM_CONFIG_HEADER(include/compilation_config.h)
AM_MINISIP_PACKAGE_INIT

AM_MINISIP_LIBTOOL_EXTRAS
AC_PROG_LIBTOOL

dnl Swig
AC_PATH_PROG(SWIG, swig, no)

dnl Mono C# compiler
AC_PATH_PROG(CSC, csc, no)
AC_PATH_PROG(MCS, mcs, no)
AC_PATH_PROG(MONO, mono, no)

CS="C#"
if test "x$CSC" = "xno" -a "x$MCS" = "xno"  ; then
	AC_MSG_ERROR([No $CS compiler found])
fi

if test "x$MCS" = "xno" ; then
	MCS=$CSC
fi

AM_CONDITIONAL([SWIG], [test ! "x$SWIG" = xno])
AM_CONDITIONAL([MCS], [test ! "x$MCS" = xno])
AM_CONDITIONAL([MONO], [test ! "x$MONO" = xno])

AM_MINISIP_CHECK_LIBMUTIL([0.8.0])
AM_MINISIP_CHECK_LIBMNETUTIL([0.8.0])
AM_MINISIP_CHECK_LIBMCRYPTO([0.8.0])
AM_MINISIP_CHECK_LIBMSIP([0.8.0])
AM_MINISIP_CHECK_LIBMIKEY([0.8.0])
AM_MINISIP_CHECK_LIBMINISIP([0.8.0])
AM_MINISIP_CHECK_COMPLETE

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([malloc.h stdlib.h string.h unistd.h])

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_HEADER_TIME

AC_CONFIG_FILES([
		Makefile
		source/Makefile
		tests/Makefile
	])
AC_OUTPUT
