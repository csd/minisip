# AM_MINISIP_CHECK_LIBMCRYPTO(VERSION)
# ------------------------------------
AC_DEFUN([AM_MINISIP_CHECK_LIBMCRYPTO],[ 
	AC_REQUIRE([AM_MINISIP_CHECK_LIBMUTIL]) dnl
dnl	AC_REQUIRE([AM_MINISIP_CHECK_OPENSSL]) dnl
	AC_ARG_WITH([mcrypto],
		AS_HELP_STRING([--with-mcrypto=PATH], [location of libmcrypto]),
		[
			if test "x${withval}" = "no"
			then
				AC_MSG_ERROR([libmcrypto is required.])
			fi
			MCRYPTO_CFLAGS="-I${withval}/include"
			MCRYPTO_LIBS="-L${withval}/lib -lmcrypto -lssl"
			AC_SUBST(MCRYPTO_CFLAGS)
			AC_SUBST(MCRYPTO_LIBS)
		],
		[
			PKG_CHECK_MODULES(MCRYPTO, [libmcrypto >= $1])
		])
	
	save_CPPFLAGS="${CPPFLAGS}"
	save_LIBS="${LIBS}"
	CPPFLAGS="${CPPFLAGS} ${MCRYPTO_CFLAGS} ${MUTIL_CFLAGS}"
	LIBS="${MUTIL_LIBS} ${MCRYPTO_LIBS} ${LIBS}"

	AC_CHECK_HEADER(libmcrypto/config.h,[],[
		AC_MSG_ERROR([You need the libmcrypto headers/library.
Try installing the libmcrypto-devel package for your distribution."])])

	AC_CHECK_LIB([mcrypto], [main], [], [
			AC_MSG_ERROR([Could not find libmcrypto.  dnl
Please install the corresponding package.])
		])
			
	CPPFLAGS="${save_CPPFLAGS}"
	LIBS="${save_LIBS}"
  ])
# End of AM_MINISIP_CHECK_LIBMCRYPTO
#
