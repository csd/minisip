dnl                                               -*- Autoconf -*-
dnl Process this file with autoconf to produce a configure script.

dnl AC_PREREQ(2.57)

m4_MINISIP_PACKAGE_VERSION(libmcrypto, mcrypto, 0, 3, 1)
m4_MINISIP_PACKAGE_CONTACT([Zachary T Welch], [zw@superlucidity.net])
m4_MINISIP_LIBRARY_VERSION(0, 0, 0)

AC_INIT(MINISIP_PACKAGE_NAME, MINISIP_PACKAGE_VERSION, MINISIP_PACKAGE_CONTACT)
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_MACRO_DIR(m4)
AM_INIT_AUTOMAKE(MINISIP_PACKAGE_NAME, MINISIP_PACKAGE_VERSION)
AM_MAINTAINER_MODE
AM_CONFIG_HEADER(include/compilation_config.h)
AM_MINISIP_PACKAGE_INIT

AM_MINISIP_LIBTOOL_EXTRAS
AC_PROG_LIBTOOL

AM_MINISIP_ENABLE_DEBUG
AM_MINISIP_ENABLE_TEST_SUITE
AM_LIBMCRYPTO_ENABLE_FAST_AES

AM_MINISIP_CHECK_LIBMUTIL([0.3.1])
AM_MINISIP_CHECK_COMPLETE

dnl Checks for libraries.
AM_LIBMCRYPTO_CHECK_OPENSSL
AM_LIBMCRYPTO_CHECK_GNUTLS

# we need the pthread library as well
AC_CHECK_LIB([pthread], [pthread_mutex_init], [], AC_MSG_ERROR([pthread library not found.]))

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([malloc.h stdlib.h string.h unistd.h])

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_HEADER_TIME

AC_CONFIG_FILES([
		Makefile
		libmcrypto.pc
		include/Makefile
		include/libmcrypto/Makefile
		include/libmcrypto/gnutls/Makefile
		include/libmcrypto/openssl/Makefile
		m4/Makefile
		source/Makefile
		source/gnutls/Makefile
		source/openssl/Makefile
		tests/Makefile
		win32/Makefile
		win32/libmcrypto-res.rc
	])
AC_OUTPUT
