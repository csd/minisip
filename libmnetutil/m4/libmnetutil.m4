# AM_MINISIP_CHECK_LIBMNETUTIL(VERSION)
# -------------------------------------
AC_DEFUN([AM_MINISIP_CHECK_LIBMNETUTIL],[ 
	AC_REQUIRE([AM_MINISIP_CHECK_LIBMUTIL]) dnl
dnl	AC_REQUIRE([AM_MINISIP_CHECK_LIBMCRYPTO]) dnl
	AC_ARG_WITH([mnetutil],
		AS_HELP_STRING([--with-mnetutil=PATH], [location of libmnetutil]),
		[
			if test "x${withval}" = "no"
			then
				AC_MSG_ERROR([libmnetutil is required.])
			fi
			MNETUTIL_CFLAGS="-I${withval}/include"
			MNETUTIL_LIBS="-L${withval}/lib -lmnetutil"
			AC_SUBST(MNETUTIL_CFLAGS)
			AC_SUBST(MNETUTIL_LIBS)
		],
		[
			PKG_CHECK_MODULES(MNETUTIL, [libmnetutil >= $1])
		])
	
	save_CPPFLAGS="${CPPFLAGS}"
	save_LIBS="${LIBS}"
	CPPFLAGS="${CPPFLAGS} ${MNETUTIL_CFLAGS} ${MCRYPTO_CFLAGS} ${MUTIL_CFLAGS}"
	LIBS="${MUTIL_LIBS} ${MCRYPTO_LIBS} ${MNETUTIL_LIBS} ${LIBS}"

	AC_CHECK_HEADER(libmnetutil/libmnetutil_config.h,[],[
		AC_MSG_ERROR([You need the libmnetutil headers/library.  
Try installing the libmnetutil-devel package for your distribution."])])

	AC_CHECK_LIB([mnetutil], [main], [], [
			AC_MSG_ERROR([Could not find libmnetutil. dnl
Please install the corresponding package.])
		])
			
	CPPFLAGS="${save_CPPFLAGS}"
	LIBS="${save_LIBS}"
  ])
# End of AM_MINISIP_CHECK_LIBMNETUTIL
#
