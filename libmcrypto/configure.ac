dnl                                               -*- Autoconf -*-
dnl Process this file with autoconf to produce a configure script.

dnl AC_PREREQ(2.57)

m4_MINISIP_PACKAGE_VERSION(libmcrypto, mcrypto, 0, 8, 0)
m4_MINISIP_PACKAGE_CONTACT([The Minisip developer team], [minisip-users@minisip.org])
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
AM_LIBMCRYPTO_ENABLE_FAST_AES

AM_MINISIP_CHECK_LIBMUTIL([0.8.0])
AM_MINISIP_CHECK_LIBMNETUTIL([0.8.0])
AM_MINISIP_CHECK_COMPLETE

dnl Checks for libraries.
AC_ARG_WITH(gnutls,
    AS_HELP_STRING([--with-gnutls=PATH], [Use GNU TLS library [default=no]]),
    [], [with_gnutls="no"])

if test "${with_gnutls}" = "yes"; then
    AM_LIBMCRYPTO_CHECK_GNUTLS([HAVE_GNUTLS=yes],[HAVE_GNUTLS=no])
    PKG_CHECK_MODULES([GNUTLS_EXTRA],[gnutls-extra],[HAVE_GNUTLS_EXTRA=yes],[HAVE_GNUTLS_EXTRA=no])
    have_sha256=${HAVE_GNUTLS}
fi
AM_CONDITIONAL(HAVE_GNUTLS, test "x${HAVE_GNUTLS}" = "xyes")

AC_ARG_WITH(openssl,
    AS_HELP_STRING([--with-openssl=PATH], [Use OpenSSL library [default=yes]]),
    [], 
    [if test "${HAVE_GNUTLS}" = "yes"; then
	with_openssl="no"
     else
	with_openssl="yes"
     fi])

if test "${with_openssl}" = "yes"; then
    AM_LIBMCRYPTO_CHECK_OPENSSL([HAVE_OPENSSL=yes],[HAVE_OPENSSL=no])
    save_LIBS=${LIBS}
    LIBS="${LIBS} ${OPENSSL_LIBS}"
    AC_CHECK_FUNC([EVP_sha256], [have_sha256=yes], [])
    LIBS=$save_LIBS

    AM_LIBMCRYPTO_CHECK_OPENSSL_DTLS([HAVE_OPENSSL_DTLS=yes],[HAVE_OPENSSL_DTLS=no])
fi
AM_CONDITIONAL(HAVE_OPENSSL, test "${HAVE_OPENSSL}" = "yes")
AM_CONDITIONAL(HAVE_OPENSSL_DTLS, test "${HAVE_OPENSSL_DTLS}" = "yes")
AM_CONDITIONAL(HAVE_EVP_SHA256, test x${have_sha256} = xyes)

AC_ARG_ENABLE(scsim,
    AS_HELP_STRING([--enable-scsim],
        [enables hardware authentication token support]),
    [ if test "${enable_scsim}" = "yes"
      then
        AM_LIBMCRYPTO_CHECK_SCSIM
        AC_DEFINE(ENABLE_SCSIM, [], [hardware SIM support is enabled])
        ENABLE_SCSIM="yes"
      fi
      ])
AM_CONDITIONAL(ENABLE_SCSIM, test "${ENABLE_SCSIM}" = "yes" )

AC_ARG_ENABLE(srp,
    AS_HELP_STRING([--enable-srp],
        [enables TLS-SRP support]),
    [ if test "${enable_srp}" = "yes"
      then
        if test "$HAVE_GNUTLS_EXTRA" = "yes"; then
          AC_DEFINE(ENABLE_SRP, [], [TLS-SRP support is enabled])
	  ENABLE_SRP="yes"
        else
          AC_MSG_ERROR([TLS-SRP support requires GNU TLS extra])
        fi
      fi  
      ])
AM_CONDITIONAL(ENABLE_SRP, test "${ENABLE_SRP}" = "yes" )




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
		debian/Makefile
		source/Makefile
		source/gnutls/Makefile
		source/openssl/Makefile
		source/uuid/Makefile
		tests/Makefile
		win32/Makefile
		win32/libmcrypto-res.rc
	])
AC_OUTPUT
