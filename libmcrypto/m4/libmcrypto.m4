# AM_LIBMCRYPTO_CHECK_OPENSSL(VERSION)
# ------------------------------------
AC_DEFUN([AM_LIBMCRYPTO_CHECK_OPENSSL], [
dnl OpenSSL libcrypto
dnl library check
AC_CHECK_LIB([crypto], [SSLeay], [
		OPENSSL_LIBS="-lcrypto"
		HAVE_OPENSSL=1
	],[
		AC_MSG_ERROR([Could not find libcrypto. Please install the corresponding package (provided by the openssl project).])
	])

dnl header check
AC_CHECK_HEADER([openssl/crypto.h],[], [
		AC_MSG_ERROR([Could not the development files for the libcrypto library. Please install the corresponding package (provided by the openssl project).])
	])

dnl Check for AES support.
AC_CHECK_HEADER([openssl/aes.h],[have_aes_h=yes])
if test x$have_aes_h = xyes; then
   AC_DEFINE([HAVE_OPENSSL_AES_H], 1, [Define to 1 if you have the <openssl/aes.h> header file.])
fi

dnl OpenSSL libssl
dnl RedHat fix
AC_DEFINE(OPENSSL_NO_KRB5, [], [No Kerberos in OpenSSL])
AC_CHECK_LIB([ssl], [SSL_library_init],
       [
               OPENSSL_LIBS="${OPENSSL_LIBS} -lssl"
       ],[
               AC_MSG_ERROR([Could not find libssl. Please install the correspo
nding package.])
       ])
AC_CHECK_HEADER([openssl/ssl.h], [],
       [
               AC_MSG_ERROR([Could not find libssl header files. Please install
 the corresponding development package.])
       ])
AM_CONDITIONAL(HAVE_OPENSSL, test "x${HAVE_OPENSSL}" = "x1")
AC_SUBST(OPENSSL_LIBS)
])
# End of AM_LIBMCRYPTO_CHECK_OPENSSL
#

# AM_LIBMCRYPTO_CHECK_GNUTLS(VERSION)
# ------------------------------------
AC_DEFUN([AM_LIBMCRYPTO_CHECK_GNUTLS], [
AC_CHECK_LIB([gcrypt], [main],[],[])
AC_CHECK_LIB([gnutls], [main],[
		AC_CHECK_HEADER(gnutls/x509.h)
		AC_MSG_NOTICE([Sorry, but gnutls support is not complete.])
dnl		AC_DEFINE([HAVE_GNUTLS], 1, [Define to 1 if you have gnutls.])
dnl		HAVE_GNUTLS=yes
	],[])
AM_CONDITIONAL(HAVE_GNUTLS, test "x${HAVE_GNUTLS}" = "xyes")
])
# End of AM_LIBMCRYPTO_CHECK_GNUTLS
#

# AM_MINISIP_CHECK_LIBMCRYPTO(VERSION)
# ------------------------------------
AC_DEFUN([AM_MINISIP_CHECK_LIBMCRYPTO],[ 
	AC_REQUIRE([AM_MINISIP_CHECK_LIBMUTIL]) dnl
dnl	AC_REQUIRE([AM_MINISIP_CHECK_OPENSSL]) dnl
	AC_MINISIP_WITH_ARG(MCRYPTO, mcrypto, libmcrypto, $1, [REQUIRED], [dnl
dnl if HAVE_OPENSSL
			-lssl -lcrypto dnl
dnl endif
dnl if HAVE_GNUTLS
dnl			-lgnutls
dnl endif
		])
	AC_MINISIP_CHECK_LIBRARY(MCRYPTO, libmcrypto, config.h, mcrypto)
  ])
# End of AM_MINISIP_CHECK_LIBMCRYPTO
#
