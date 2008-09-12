dnl                                               -*- Autoconf -*-
dnl Process this file with autoconf to produce a configure script.

dnl AC_PREREQ(2.57)

m4_MINISIP_PACKAGE_VERSION(libmikey, mikey, 0, 8, 0)
m4_MINISIP_PACKAGE_CONTACT([Johan Bilien], [jobi@via.ecp.fr])
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

AM_MINISIP_CHECK_LIBMUTIL([0.8.0])
AM_MINISIP_CHECK_LIBMNETUTIL([0.8.0])
AM_MINISIP_CHECK_LIBMCRYPTO([0.8.0])
AM_MINISIP_CHECK_COMPLETE

dnl G-D HW authentication token support
AC_ARG_ENABLE(scsim,
    AS_HELP_STRING([--enable-scsim],
            [enable G-D HW authentication token (SmartCard) support (default disabled)]))
    if test "${enable_scsim}" = "yes" 
    then
        AC_DEFINE(SCSIM_SUPPORT, [], [SCSIM support])
    fi
AM_CONDITIONAL(SCSIM_SUPPORT, test "${enable_scsim}" = "yes")


dnl Checks for libraries.

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([malloc.h stdlib.h string.h unistd.h])

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_HEADER_TIME

AC_CONFIG_FILES([
		Makefile 
		libmikey.pc 
		include/Makefile 
		m4/Makefile 
		debian/Makefile 
		tests/Makefile
		win32/Makefile 
		win32/libmikey-res.rc
	])
AC_OUTPUT
