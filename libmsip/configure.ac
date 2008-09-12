dnl                                               -*- Autoconf -*-
dnl Process this file with autoconf to produce a configure script.

dnl AC_PREREQ(2.57)

m4_MINISIP_PACKAGE_VERSION(libmsip, msip, 0, 8, 0)
m4_MINISIP_PACKAGE_CONTACT([Erik Eliasson], [ere@kth.se])
dnl m4_MINISIP_PACKAGE_RELEASED
m4_MINISIP_LIBRARY_VERSION(0, 0, 0)

AC_INIT(MINISIP_PACKAGE_NAME, MINISIP_PACKAGE_VERSION, MINISIP_PACKAGE_CONTACT)
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_MACRO_DIR(m4)
AM_MINISIP_PACKAGE_INIT
AM_CONFIG_HEADER(include/compilation_config.h)

AM_MINISIP_LIBTOOL_EXTRAS
AC_PROG_LIBTOOL

AM_MINISIP_ENABLE_DEBUG
AM_MINISIP_ENABLE_TEST_SUITE


dnl debug flag
AC_ARG_ENABLE(dropemul,
		AS_HELP_STRING([--enable-dropemul],
			[enables simulated packet dropping of UDP packets (for debugging only - default disabled)]),
		[ if test "${enable_dropemul}" = "yes"
		then
		AC_DEFINE(DEBUG_UDPPACKETDROPEMUL, [], [Debug autocalling])
		fi ])


dnl Damn RedHat
AC_DEFINE(OPENSSL_NO_KRB5, [], [No Kerberos in OpenSSL])


dnl
dnl mingw support
dnl
AC_CHECK_LIB([wsock32], [main])

AM_MINISIP_CHECK_LIBMUTIL([0.8.0])
AM_MINISIP_CHECK_LIBMNETUTIL([0.8.0])
AM_MINISIP_CHECK_LIBMCRYPTO([0.8.0])
AM_MINISIP_CHECK_COMPLETE

dnl Checks for SCTP support in libmnetutil
AM_MINISIP_CHECK_LIBMNETUTIL_SCTP([have_sctp=yes], [have_sctp=no])
AM_CONDITIONAL(HAVE_SCTP, test "${have_sctp}" = "yes")

dnl Checks for DTLS support in libmcrypto
AM_MINISIP_CHECK_LIBMCRYPTO_DTLS([have_dtls=yes], [have_dtls=no])
AM_CONDITIONAL(HAVE_DTLS, test "${have_dtls}" = "yes")

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([malloc.h stdlib.h string.h unistd.h])

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_HEADER_TIME

AC_CONFIG_FILES([
		Makefile 
		libmsip.pc 
		include/Makefile 
		m4/Makefile 
		debian/Makefile 
		tests/Makefile 
		win32/Makefile 
		win32/libmsip-res.rc
	])
AC_OUTPUT
