# AM_MINISIP_CHECK_LIBMSIP(VERSION)
# ----------------------------------
AC_DEFUN([AM_MINISIP_CHECK_LIBMSIP],[ 
	AC_REQUIRE([AM_MINISIP_CHECK_LIBMNETUTIL]) dnl
	AC_ARG_WITH([msip],
		AS_HELP_STRING([--with-msip=PATH], [location of libmsip]),
		[
			if test "x${withval}" = "no"
			then
				AC_MSG_ERROR([libmsip is required.])
			fi
			MSIP_CFLAGS="-I${withval}/include"
			MSIP_LIBS="-L${withval}/lib -lmsip"
			AC_SUBST(MSIP_CFLAGS)
			AC_SUBST(MSIP_LIBS)
		],
		[
			PKG_CHECK_MODULES(MSIP, [libmsip >= $1])
		])
	
	save_CPPFLAGS="${CPPFLAGS}"
	save_LIBS="${LIBS}"
	CPPFLAGS="${CPPFLAGS} ${MSIP_CFLAGS} ${MNETUTIL_CFLAGS} ${MCRYPTO_CFLAGS} ${MUTIL_CFLAGS}"
	LIBS="${MUTIL_LIBS} ${MCRYPTO_LIBS} ${MNETUTIL_LIBS} ${MSIP_LIBS} ${LIBS}"

	AC_CHECK_HEADER(libmsip/libmsip_config.h,[],[
		AC_MSG_ERROR([You need the libmsip headers/library.  Try installing the libmsip-devel package for your distribution."])])

	AC_CHECK_LIB([msip], [main], [], [
			AC_MSG_ERROR([Could not find libmsip. Please install the corresponding package.])
		])
			
	CPPFLAGS="${save_CPPFLAGS}"
	LIBS="${save_LIBS}"
  ])
# End of AM_MINISIP_CHECK_LIBMSIP
#
