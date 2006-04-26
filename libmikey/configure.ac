dnl                                               -*- Autoconf -*-
dnl Process this file with autoconf to produce a configure script.

dnl AC_PREREQ(2.57)

m4_MINISIP_PACKAGE_VERSION(libmikey, mikey, 0, 4, 1)
m4_MINISIP_PACKAGE_CONTACT([Johan Bilien], [jobi@via.ecp.fr])
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
dnl Check for libssl 0.9.7
AC_CHECK_HEADERS([openssl/aes.h],[],[])

AM_MINISIP_CHECK_LIBMUTIL([0.3.1])
AM_MINISIP_CHECK_COMPLETE

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([malloc.h stdlib.h string.h unistd.h])

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_HEADER_TIME


AC_CONFIG_FILES([
		Makefile 
		libmikey.pc 
		debian/Makefile 
		include/Makefile 
		m4/Makefile 
		win32/Makefile 
		win32/libmikey-res.rc
	])
AC_OUTPUT
