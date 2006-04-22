# MINISIP_CHECK_LIBMUTIL(VERSION)
# -------------------------------
AC_DEFUN([MINISIP_CHECK_LIBMUTIL],[ 
	AC_ARG_WITH([mutil],
		AS_HELP_STRING([--with-mutil=PATH], [location of libmutil]),
		[
			if test "x${withval}" = "no"
			then
				AC_MSG_ERROR([libmutil is required. Nice try.])
			fi
			MUTIL_CFLAGS="-I${withval}/include"
			MUTIL_LIBS="-L${withval}/lib -lmutil"
			AC_SUBST(MUTIL_CFLAGS)
			AC_SUBST(MUTIL_LIBS)
		],
		[
			PKG_CHECK_MODULES(MUTIL, [libmutil >= $1])
		])

	save_CPPFLAGS="${CPPFLAGS}"
	save_LIBS="${LIBS}"
	CPPFLAGS="${CPPFLAGS} ${MUTIL_CFLAGS}"
	LIBS="${MUTIL_LIBS} ${LIBS}"

	AC_CHECK_HEADER(libmutil/itoa.h,,[
		AC_MSG_ERROR(["You need the libmutil headers/library. 
			Try installing the libmutil-devel package for your
			distribution."])])
	AC_CHECK_LIB([mutil], [main],
		[
			LIBS="-lmutil ${LIBS}"
		],[
			AC_MSG_ERROR([Could not find libmutil. Please install the corresponding package.])
		])
			
	CPPFLAGS="${save_CPPFLAGS}"
	LIBS="${save_LIBS}"
  ])
#
# End of MINISIP_CHECK_LIBMUTIL
#
