# AM_MINISIP_CHECK_LIBMCRYPTO(VERSION)
# ------------------------------------
AC_DEFUN([AM_MINISIP_CHECK_LIBMCRYPTO],[ 
	AC_REQUIRE([AM_MINISIP_CHECK_LIBMUTIL]) dnl
dnl	AC_REQUIRE([AM_MINISIP_CHECK_OPENSSL]) dnl
	AC_MINISIP_WITH_ARG(MCRYPTO, mcrypto, libmcrypto, $1, [REQUIRED], [dnl
dnl if HAVE_OPENSSL
			-lssl dnl
dnl endif
dnl if HAVE_GNUTLS
dnl			-lgnutls
dnl endif
		])
	AC_MINISIP_CHECK_LIBRARY(MCRYPTO, libmcrypto, config.h, mcrypto)
  ])
# End of AM_MINISIP_CHECK_LIBMCRYPTO
#
