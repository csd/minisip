dnl                                               -*- Autoconf -*-
dnl Process this file with autoconf to produce a configure script.

dnl AC_PREREQ(2.57)

m4_MINISIP_PACKAGE_VERSION(libmnetutil, mnetutil, 0, 3, 1)
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
AM_MINISIP_ENABLE_IPV6

dnl networking headers
AC_CHECK_HEADERS([netinet/in.h ws2tcpip.h])
AM_MINISIP_CHECK_IPV6

dnl
dnl mingw support
dnl
AC_CHECK_LIB([wsock32], [main])
AC_CHECK_LIB([iphlpapi], [main])
AC_CHECK_LIB([dnsapi], [main])

AM_MINISIP_CHECK_LIBMUTIL([0.3.1])
AM_MINISIP_CHECK_LIBMCRYPTO([0.3.1])
AM_MINISIP_CHECK_COMPLETE

AC_CHECK_LIB([resolv], [main])

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([malloc.h stdlib.h string.h unistd.h sys/types.h sys/socket.h netdb.h netinet/in.h arpa/inet.h netinet/tcp.h linux/sockios.h arpa/nameser_compat.h])

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_HEADER_TIME

AC_CONFIG_FILES([
		Makefile 
		libmnetutil.pc 
		debian/Makefile 
		examples/Makefile
		include/Makefile 
		m4/Makefile 
		tests/Makefile
		win32/Makefile 
		win32/libmnetutil-res.rc
	])
AC_OUTPUT
