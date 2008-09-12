dnl                                               -*- Autoconf -*-
dnl Process this file with autoconf to produce a configure script.


m4_MINISIP_PACKAGE_VERSION(libmstun, mstun, 0, 8, 0)
m4_MINISIP_PACKAGE_CONTACT([The Minisip developer team], [minisip-users@minisip.org])
dnl m4_MINISIP_PACKAGE_RELEASED
m4_MINISIP_LIBRARY_VERSION(0, 0, 0)

dnl AC_PREREQ(2.57)
AC_INIT(MINISIP_PACKAGE_NAME, MINISIP_PACKAGE_VERSION, MINISIP_PACKAGE_CONTACT)
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_MACRO_DIR(m4)
AM_MINISIP_PACKAGE_INIT
AM_CONFIG_HEADER(include/compilation_config.h)

AM_MINISIP_LIBTOOL_EXTRAS
AC_PROG_LIBTOOL

AM_MINISIP_ENABLE_DEBUG
AM_MINISIP_ENABLE_TEST_SUITE

AM_MINISIP_CHECK_LIBMUTIL([0.8.0])
AM_MINISIP_CHECK_LIBMNETUTIL([0.8.0])
AM_MINISIP_CHECK_COMPLETE

dnl Checks for libraries.
AC_CHECK_LIB([wsock32], [main])

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([malloc.h stdlib.h string.h unistd.h netinet/in.h])

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_HEADER_TIME

AC_CONFIG_FILES([
		Makefile
		libmstun.pc
		m4/Makefile
		debian/Makefile
		include/Makefile
		include/libmstun/Makefile
		source/Makefile
		win32/Makefile 
		win32/libmstun-res.rc
	])
AC_OUTPUT
