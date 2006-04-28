dnl                                               -*- Autoconf -*-
dnl Process this file with autoconf to produce a configure script.

dnl AC_PREREQ(2.57)

m4_MINISIP_PACKAGE_VERSION(libmsip, msip, 0, 3, 1)
m4_MINISIP_PACKAGE_CONTACT([Erik Eliasson], [eliasson@imit.kth.se])
m4_MINISIP_LIBRARY_VERSION(0, 0, 0)

AC_INIT(MINISIP_PACKAGE_NAME, MINISIP_PACKAGE_VERSION, MINISIP_PACKAGE_CONTACT)
AC_CONFIG_AUX_DIR(config)
AM_INIT_AUTOMAKE(MINISIP_PACKAGE_NAME, MINISIP_PACKAGE_VERSION)
AM_MAINTAINER_MODE
AM_CONFIG_HEADER(include/compilation_config.h)
AM_MINISIP_PACKAGE_INIT

AM_MINISIP_LIBTOOL_EXTRAS
AC_PROG_LIBTOOL

AM_MINISIP_ENABLE_DEBUG
AM_MINISIP_ENABLE_TEST_SUITE

dnl debug flag
AC_ARG_ENABLE(memdebug,
    [ --enable-memdebug enables memory debugging code. WARNING: This changes
     the MemObject API that is used by both libraries and applications. Use
     with care (default disabled)],
    [ if test "${enable_memdebug}" = "yes"
      then
        AC_DEFINE(MINISIP_MEMDEBUG, [], [Debug memory])
      fi ])


dnl Damn RedHat
AC_DEFINE(OPENSSL_NO_KRB5, [], [No Kerberos in OpenSSL])


dnl
dnl mingw support
dnl
AC_CHECK_LIB([wsock32], [main])

AM_MINISIP_CHECK_LIBMUTIL([0.3.1])
AM_MINISIP_CHECK_LIBMCRYPTO([0.3.0])
AM_MINISIP_CHECK_LIBMNETUTIL([0.3.1])
AM_MINISIP_CHECK_COMPLETE

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([malloc.h stdlib.h string.h unistd.h])

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_HEADER_TIME

AC_CONFIG_FILES([
		Makefile 
		libmsip.pc 
		debian/Makefile 
		include/Makefile 
		m4/Makefile 
		win32/Makefile 
		win32/libmsip-res.rc
	])
AC_OUTPUT
