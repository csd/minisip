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
AM_MINISIP_CHECK_LIBMNETUTIL([0.3.1])
AM_MINISIP_CHECK_COMPLETE

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([malloc.h stdlib.h string.h unistd.h])

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
