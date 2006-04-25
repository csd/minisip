# AC_MINISIP_CHECK_WITH_ARG(MACRO, NAME, LIBS)
# --------------------------------------
AC_DEFUN([AC_MINISIP_CHECK_WITH_ARG],[
		if test "x${withval}" = "xyes"; then
			# proceed with default installation
		 	$1_NEEDS_PKG_CHECK=yes
		else
			if test -d "${withval}/lib"; then
				# specific installation
				$1_LDFLAGS="-L${withval}/lib"
			elif test -d "${withval}/.libs"; then
				# in-tree development
				$1_LDFLAGS="-L${withval}/.libs"
			elif test -d "../$2/.libs"; then
				# out-of-tree development
				$1_LDFLAGS="-L`pwd`/../$2/.libs"
			else
				AC_MSG_ERROR([dnl
Unable to find the required libraries in any of the following locations:
	${withval}/lib
	${withval}/.libs
	../$2/.libs

Maybe you forgot to compile $2 first?
])
			fi
			$1_CFLAGS="-I${withval}/include"
			$1_LIBS="${$1_LDFLAGS} $3"
			AC_SUBST($1_CFLAGS)
			AC_SUBST($1_LIBS)
		fi
	])
# End of AC_MINISIP_CHECK_WITH_ARG
#

# AC_MINISIP_MAYBE_WITH_ARG(MACRO, WITHARG, NAME, TYPE, LIBS)
# ----------------------------------------------------------------
AC_DEFUN([AC_MINISIP_MAYBE_WITH_ARG],[
		if test "x${withval}" = "no"; then
			AC_MINISIP_$4_LIB($1, $3)
		else
			AC_MINISIP_CHECK_WITH_ARG($1, $3, [-l$2 $5])
		fi
	])
# End of AC_MINISIP_MAYBE_WITH_ARG
#

# AC_MINISIP_WITH_ARG(MACRO, WITHARG, NAME, VERSION, TYPE, LIBS)
# ----------------------------------------------------------------
AC_DEFUN([AC_MINISIP_WITH_ARG],[
		AC_ARG_WITH($2,
			AS_HELP_STRING([--with-$2=PATH], [location of $3]),
			[ AC_MINISIP_MAYBE_WITH_ARG($1, $2, $3, $5, $6) ],
			[ $1_NEEDS_PKG_CHECK=yes ])
		if test "x${$1_NEEDS_PKG_CHECK}" = "xyes"; then
			PKG_CHECK_MODULES($1, [$3 >= $4])
		fi
		MINISIP_CFLAGS="${$1_CFLAGS} ${MINISIP_CFLAGS}"
		MINISIP_LIBS="${$1_LIBS} ${MINISIP_LIBS}"
	])
# End of AC_MINISIP_WITH_ARG
#

# AC_MINISIP_OPTIONAL_LIB(MACRO, NAME)
# ------------------------------------
AC_DEFUN([AC_MINISIP_OPTIONAL_LIB], [ dnl
		AC_MSG_NOTICE([$1 is not present or disabled.]) ])
# End of AC_MINISIP_OPTIONAL_LIB
#

# AC_MINISIP_REQUIRED_LIB(MACRO, NAME)
# ------------------------------------
AC_DEFUN([AC_MINISIP_REQUIRED_LIB], [ dnl
		AC_MSG_ERROR([$1 is required.]) ])
# End of AC_MINISIP_REQUIRED_LIB
#

# AC_MINISIP_CHECK_LIBRARY(NAME, HEADER, LIB)
# -------------------------------------------
AC_DEFUN([AC_MINISIP_CHECK_LIBRARY], [
		save_CPPFLAGS="${CPPFLAGS}"
		save_LIBS="${LIBS}"
		CPPFLAGS="${MINISIP_CFLAGS} ${CPPFLAGS}"
		LIBS="${MINISIP_LIBS} ${LIBS}"
		AC_CHECK_HEADER([$1/$2],[],[
			AC_MSG_ERROR([You need the $1 headers/library.
Try installing the $1-devel package for your distribution."])])

		AC_CHECK_LIB([$3], [main], [], [ dnl
				AC_MSG_ERROR([Could not find $1. dnl
Please install the corresponding package.]) dnl
			])
				
		CPPFLAGS="${save_CPPFLAGS}"
		LIBS="${save_LIBS}"
	])
# End of AC_MINISIP_CHECK_LIBRARY
#

# AC_MINISIP_CHECK_COMPLETE()
# ---------------------------
AC_DEFUN([AM_MINISIP_CHECK_COMPLETE],[ 
		MINISIP_CFLAGS="-I\$(top_srcdir)/include ${MINISIP_CFLAGS}"
		AC_SUBST(MINISIP_CFLAGS)
		AC_SUBST(MINISIP_LIBS)
	])
# End of AC_MINISIP_CHECK_COMPLETE
#

# AM_MINISIP_CHECK_LIBMUTIL(VERSION)
# -------------------------------
AC_DEFUN([AM_MINISIP_CHECK_LIBMUTIL],[ 
	AC_MINISIP_WITH_ARG(MUTIL, mutil, libmutil, $1, [REQUIRED])
	AC_MINISIP_CHECK_LIBRARY(libmutil, libmutil_config.h, mutil)
  ])
# End of AM_MINISIP_CHECK_LIBMUTIL
#
