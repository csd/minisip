# AM_MINISIP_CHECK_LIBMIKEY(VERSION)
# ----------------------------------
AC_DEFUN([AM_MINISIP_CHECK_LIBMIKEY],[ 
	AC_REQUIRE([AM_MINISIP_CHECK_LIBMUTIL]) dnl
dnl	AC_REQUIRE([AM_MINISIP_CHECK_LIBMCRYPTO]) dnl
	AC_ARG_WITH([mikey],
		AS_HELP_STRING([--with-mikey=PATH], [location of libmikey]),
		[
			if test "x${withval}" = "no"
			then
				AC_MSG_ERROR([libmikey is required.])
			fi
			MIKEY_CFLAGS="-I${withval}/include"
			MIKEY_LIBS="-L${withval}/lib -lmikey"
			AC_SUBST(MIKEY_CFLAGS)
			AC_SUBST(MIKEY_LIBS)
		],
		[
			PKG_CHECK_MODULES(MIKEY, [libmikey >= $1])
		])
	
	save_CPPFLAGS="${CPPFLAGS}"
	save_LIBS="${LIBS}"
	CPPFLAGS="${CPPFLAGS} ${MIKEY_CFLAGS} ${MCRYPTO_CFLAGS} ${MUTIL_CFLAGS}"
	LIBS="${MUTIL_LIBS} ${MCRYPTO_LIBS} ${MIKEY_LIBS} ${LIBS}"

	AC_CHECK_HEADER(libmikey/libmikey_config.h,[],[
		AC_MSG_ERROR([You need the libmikey headers/library.  Try installing the libmikey-devel package for your distribution."])])

	AC_CHECK_LIB([mikey], [main], [], [
			AC_MSG_ERROR([Could not find libmikey. Please install the corresponding package.])
		])
			
	CPPFLAGS="${save_CPPFLAGS}"
	LIBS="${save_LIBS}"
  ])
# End of AM_MINISIP_CHECK_LIBMIKEY
#
