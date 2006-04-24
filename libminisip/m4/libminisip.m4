# AM_MINISIP_CHECK_LIBMINISIP(VERSION)
# ----------------------------------
AC_DEFUN([AM_MINISIP_CHECK_LIBMINISIP],[ 
	AC_REQUIRE([AM_MINISIP_CHECK_LIBMIKEY]) dnl
	AC_REQUIRE([AM_MINISIP_CHECK_LIBMSIP]) dnl
	AC_ARG_WITH([minisip],
		AS_HELP_STRING([--with-minisip=PATH], [location of libminisip]),
		[
			if test "x${withval}" = "no"
			then
				AC_MSG_ERROR([libminisip is required.])
			fi
			LIBMINISIP_CFLAGS="-I${withval}/include"
			LIBMINISIP_LIBS="-L${withval}/lib -lminisip"
			AC_SUBST(LIBMINISIP_CFLAGS)
			AC_SUBST(LIBMINISIP_LIBS)
		],
		[
			PKG_CHECK_MODULES(LIBMINISIP, [libminisip >= $1])
		])
	
	save_CPPFLAGS="${CPPFLAGS}"
	save_LIBS="${LIBS}"
	CPPFLAGS="${CPPFLAGS} ${LIBMINISIP_CFLAGS} ${MSIP_CFLAGS} ${MNETUTIL_CFLAGS} ${MCRYPTO_CFLAGS} ${MUTIL_CFLAGS}"
	LIBS="${MUTIL_LIBS} ${LIBMCRYPTO_LIBS} ${MNETUTIL_LIBS} ${MSIP_LIBS} ${LIBMINISIP_LIBS} ${LIBS}"

	AC_CHECK_HEADER(libminisip/libminisip_config.h,[],[
		AC_MSG_ERROR([You need the libminisip headers/library.  Try installing the libminisip-devel package for your distribution."])])

	AC_CHECK_LIB([minisip], [main], [], [
			AC_MSG_ERROR([Could not find libminisip. Please install the corresponding package.])
		])
			
	CPPFLAGS="${save_CPPFLAGS}"
	LIBS="${save_LIBS}"
  ])
# End of AM_MINISIP_CHECK_LIBMINISIP
#
