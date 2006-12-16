# AM_LIBMCRYPTO_ENABLE_FAST_AES()
# -------------------------------
AC_DEFUN([AM_LIBMCRYPTO_ENABLE_FAST_AES], [
AC_ARG_ENABLE(fast-aes,
	AS_HELP_STRING([--enable-fast-aes],
		[enables built-in Rijndael/AES algorithm. (default disabled)]), 
	[], [])
])
# End of AM_LIBMCRYPTO_ENABLE_FAST_AES
#

# AM_LIBMCRYPTO_CHECK_OPENSSL([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
# -----------------------------
AC_DEFUN([AM_LIBMCRYPTO_CHECK_OPENSSL], [
dnl OpenSSL libcrypto
dnl library check
PKG_CHECK_MODULES(OPENSSL, [openssl])
mcrypto_save_LIBS="${LIBS}"
LIBS="${OPENSSL_LIBS} ${LIBS}"
mcrypto_save_CPPFLAGS="${CPPFLAGS}"
CPPFLAGS="${CPPFLAGS} ${OPENSSL_CFLAGS}"
AC_SEARCH_LIBS([SSLeay], [], [
		HAVE_OPENSSL=1
	],[
		AC_MSG_ERROR([Could not find libcrypto. Please install the corresponding package (provided by the openssl project).])
	])


dnl header check
AC_CHECK_HEADER([openssl/crypto.h],[], [
		AC_MSG_ERROR([Could not the development files for the libcrypto library. Please install the corresponding package (provided by the openssl project).])
	])

# disable OpenSSL AES if user did not ask to use our built-in algorithm
if test x$enable_fast_aes != xyes; then
	AC_CHECK_HEADER([openssl/aes.h],[AC_DEFINE([HAVE_OPENSSL_AES_H], 1, 
		[Define to 1 if you have the <openssl/aes.h> header file.])])
fi

dnl AC_CHECK_FUNC([EVP_sha256], [have_sha256=yes], [])
dnl AM_CONDITIONAL(HAVE_EVP_SHA256, test x${have_sha256} = xyes)

dnl OpenSSL libssl
dnl RedHat fix
AC_DEFINE(OPENSSL_NO_KRB5, [], [No Kerberos in OpenSSL])
AC_SEARCH_LIBS([SSL_library_init], [],
       [
       ],[
               AC_MSG_ERROR([Could not find libssl. Please install the correspo
nding package.])
       ])
AC_CHECK_HEADER([openssl/ssl.h], [],
       [
               AC_MSG_ERROR([Could not find libssl header files. Please install
 the corresponding development package.])
       ])

if test "x${HAVE_OPENSSL}" = "x1"; then
	AC_DEFINE([HAVE_OPENSSL], 1, [Define to 1 if you have OpenSSL.])
	ifelse([$1], , :, [$1])
else
	ifelse([$2], , :, [$2])
fi

dnl AM_CONDITIONAL(HAVE_OPENSSL, test "x${HAVE_OPENSSL}" = "x1")

LIBS="${mcrypto_save_LIBS}"
CPPFLAGS="${mcrypto_save_CPPFLAGS}"

])
# End of AM_LIBMCRYPTO_CHECK_OPENSSL
#

AC_DEFUN([AM_LIBMCRYPTO_CHECK_SCSIM], [
AC_CHECK_LIB([pcsclite], [SCardTransmit],[
		SCSIM_LIBS="-lpcsclite"
	],[])
AC_SUBST(SCSIM_LIBS)
LIBS="${SCSIM_LIBS} ${LIBS}"
])

# AM_LIBMCRYPTO_CHECK_GNUTLS([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
# ----------------------------
AC_DEFUN([AM_LIBMCRYPTO_CHECK_GNUTLS], [
AC_CHECK_LIB([gcrypt], [main],[
		GNUTLS_LIBS="-lgcrypt"
	],[])
AC_CHECK_LIB([gnutls], [main],[
		AC_CHECK_HEADER(gnutls/x509.h)
		AC_MSG_NOTICE([Sorry, but gnutls support is not complete.])
		GNUTLS_LIBS="-lgnutls"
		AC_DEFINE([HAVE_GNUTLS], 1, [Define to 1 if you have gnutls.])
		HAVE_GNUTLS=yes
	],[])
if test "x${HAVE_GNUTLS}" = "xyes"; then
	ifelse([$1], , :, [$1])
else
	ifelse([$2], , :, [$2])
fi

dnl AM_CONDITIONAL(HAVE_GNUTLS, test "x${HAVE_GNUTLS}" = "xyes")
AC_SUBST(GNUTLS_LIBS)
])
# End of AM_LIBMCRYPTO_CHECK_GNUTLS
#

# AM_MINISIP_CHECK_LIBMCRYPTO(VERSION)
# ------------------------------------
AC_DEFUN([AM_MINISIP_CHECK_LIBMCRYPTO],[ 
	AC_REQUIRE([AM_MINISIP_CHECK_LIBMNETUTIL]) dnl
dnl	AC_REQUIRE([AM_MINISIP_CHECK_OPENSSL]) dnl
	AC_MINISIP_WITH_ARG(MCRYPTO, mcrypto, libmcrypto, $1, [REQUIRED], [dnl
dnl if HAVE_OPENSSL
dnl			-lssl -lcrypto dnl
dnl endif
dnl if HAVE_GNUTLS
dnl			-lgnutls
dnl endif
		])
	AC_MINISIP_CHECK_LIBRARY(MCRYPTO, libmcrypto, config.h, mcrypto)
  ])
# End of AM_MINISIP_CHECK_LIBMCRYPTO
#
