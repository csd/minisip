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

# AM_LIBMCRYPTO_CHECK_OPENSSL_DTLS([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
# -----------------------------
AC_DEFUN([AM_LIBMCRYPTO_CHECK_OPENSSL_DTLS], [
dnl OpenSSL DTLS

mcrypto_save_LIBS="${LIBS}"
LIBS="${OPENSSL_LIBS} ${LIBS}"
mcrypto_save_CPPFLAGS="${CPPFLAGS}"
CPPFLAGS="${CPPFLAGS} ${OPENSSL_CFLAGS}"

dnl Check for DTLS, requires OpenSSL 0.9.8f or later.
have_dtls=yes
AC_CHECK_HEADER([openssl/dtls1.h], , [have_dtls=no], [
#include <openssl/ssl.h>
])

dnl Check DTLS version magic
AC_MSG_CHECKING([for DTLS version 1.0])
AC_COMPILE_IFELSE([
#include<openssl/ssl.h>
#include<openssl/dtls1.h>

#ifdef DTLS1_VERSION
# if DTLS1_VERSION != 0xFEFF
#  error Bad DTLS1 version
# endif
#else
# error  No DTLS1 version
#endif

int main()
{
    return 0;
}
], [dtls1=yes],[have_dtls=no;dtls1=no])
AC_MSG_RESULT([$dtls1])

 AC_CHECK_FUNC([DTLSv1_method], , [have_dtls=no])
 if test x$have_dtls = xyes; then
 	AC_DEFINE(USE_DTLS, "1", [Define to 1 if you have OpenSSL 0.9.8f or later])dnl
 	ifelse([$1], , :, [$1])
 else
 	ifelse([$2], , :, [$2])
 fi

dnl AM_CONDITIONAL(HAVE_OPENSSL, test "x${HAVE_OPENSSL}" = "x1")

LIBS="${mcrypto_save_LIBS}"
CPPFLAGS="${mcrypto_save_CPPFLAGS}"
])
# End of AM_LIBMCRYPTO_CHECK_OPENSSL_DTLS
#

AC_DEFUN([AM_LIBMCRYPTO_CHECK_SCSIM], [
PKG_CHECK_MODULES([SCSIM], [libpcsclite])

mcrypto_save_LIBS="${LIBS}"
LIBS="${SCSIM_LIBS} ${LIBS}"
AC_CHECK_FUNCS([SCardTransmit],,[AC_MSG_ERROR([PCSC lite not found])])
LIBS="${mcrypto_save_LIBS}"
])

# AM_LIBMCRYPTO_CHECK_GNUTLS([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
# ----------------------------
AC_DEFUN([AM_LIBMCRYPTO_CHECK_GNUTLS], [
AC_CHECK_LIB([gcrypt], [main],[
		GNUTLS_LIBS="-lgcrypt"
	],[])
AC_CHECK_LIB([gnutls], [main],[
		AC_CHECK_HEADER(gnutls/x509.h)
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

# AM_MINISIP_CHECK_LIBMCRYPTO(VERSION [,ACTION-IF-FOUND [,ACTION-IF-NOT-FOUND]]))
# ------------------------------------
AC_DEFUN([AM_MINISIP_CHECK_LIBMCRYPTO],[ 
	AC_REQUIRE([AM_MINISIP_CHECK_LIBMNETUTIL]) dnl
	mcrypto_found=yes
dnl	AC_REQUIRE([AM_MINISIP_CHECK_OPENSSL]) dnl
	AC_MINISIP_WITH_ARG(MCRYPTO, mcrypto, libmcrypto, $1, ifelse([$3], , [REQUIRED], [OPTIONAL]), [dnl
dnl if HAVE_OPENSSL
dnl			-lssl -lcrypto dnl
dnl endif
dnl if HAVE_GNUTLS
dnl			-lgnutls
dnl endif
		], ,[mcrypto_found=no])
	if test ! "${mcrypto_found}" = "no"; then
		AC_MINISIP_CHECK_LIBRARY(MCRYPTO, libmcrypto, config.h, mcrypto)
	fi
	if test "${mcrypto_found}" = "yes"; then
		ifelse([$2], , :, [$2])
	else
		ifelse([$3], , [AC_MINISIP_REQUIRED_LIB(MCRYPTO, libmcrypto)], [$3])
	fi
  ])
# End of AM_MINISIP_CHECK_LIBMCRYPTO
#

# AM_MINISIP_CHECK_LIBMCRYPTO_DTLS([ACTION-IF-FOUND [,ACTION-IF-NOT-FOUND]]))
# ------------------------------------
AC_DEFUN([AM_MINISIP_CHECK_LIBMCRYPTO_DTLS],[ 
	AC_REQUIRE([AM_MINISIP_CHECK_LIBMCRYPTO]) dnl
	mcrypto_dtls_found=yes

dnl Checks for DTLS support in libmcrypto
	mcrypto_save_LIBS="$LIBS"
	mcrypto_save_LDFLAGS="$LDFLAGS"
	mcrypto_save_CPPFLAGS="$CPPFLAGS"
	LDFLAGS="$LDFLAGS $LIBMCRYPTO_LDFLAGS"
	LIBS="$MINISIP_LIBS $LIBS"
	CPPFLAGS="$CPPFLAGS $MINISIP_CFLAGS"
	AM_MINISIP_CHECK_WINFUNC(["DTLSSocket::create(0,0,0)"],[mcrypto_dtls_found=yes], [mcrypto_dtls_found=no],[ #include<libmcrypto/DtlsSocket.h> ])
	LIBS="$mcrypto_save_LIBS"
	LDFLAGS="$mcrypto_save_LDFLAGS"
	CPPFLAGS="$mcrypto_save_CPPFLAGS"

	if test "${mcrypto_dtls_found}" = "yes"; then
		AC_DEFINE([HAVE_DTLS], 1, [Define to 1 if you have libmcrypto with DTLS support])
		ifelse([$1], , :, [$1])
	else
		ifelse([$2], , :, [$2])
	fi
  ])
# End of AM_MINISIP_CHECK_LIBMCRYPTO_DTLS
#
