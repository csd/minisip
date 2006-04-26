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
AC_CHECK_LIB([iphlpapi], [main])
AC_CHECK_LIB([dnsapi], [main])

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

AM_MINISIP_CHECK_LIBMUTIL([0.3.1])
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
		include/Makefile 
		m4/Makefile 
		examples/Makefile
		win32/Makefile 
		win32/libmnetutil-res.rc
	])
AC_OUTPUT
