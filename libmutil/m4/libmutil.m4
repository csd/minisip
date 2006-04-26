dnl  =================================================================
dnl                minisip configure initialization macros

# m4_MINISIP_PACKAGE_VERSION(NAME, PREFIX, MAJOR, MINOR, MICRO)
# -------------------------------------------------------------
m4_define([m4_MINISIP_PACKAGE_VERSION],[
		m4_define([$2_major_version], [$3])
		m4_define([$2_minor_version], [$4])
		m4_define([$2_micro_version], [$5])
		m4_define([$2_version],
			  [$2_major_version.$2_minor_version.$2_micro_version])
		m4_define([MINISIP_PACKAGE_NAME],[$1])
		m4_define([MINISIP_PACKAGE_PREFIX],[$2])
		m4_define([MINISIP_PACKAGE_MACRO],m4_translit([$2],[a-z],[A-Z]))
		m4_define([MINISIP_PACKAGE_VERSION],[$2_version])
	])
# End of m4_MINISIP_PACKAGE_VERSION
#

# m4_MINISIP_PACKAGE_CONTACT(NAME, EMAIL)
# ---------------------------------------
m4_define([m4_MINISIP_PACKAGE_CONTACT],[
		m4_define([MINISIP_PACKAGE_CONTACT],[$1 <$2>])
	])
# End of m4_MINISIP_PACKAGE_CONTACT
#

# m4_MINISIP_LIBRARY_VERSION(CURRENT, REVISION, AGE)
# --------------------------------------------------
m4_define([m4_MINISIP_LIBRARY_VERSION],[
		m4_define([lt_current], [$1])
		m4_define([lt_revision], [$2])
		m4_define([lt_age], [$3])
		m4_define([lt_minus_age], m4_eval(lt_current - lt_age))
	])
# End of m4_MINISIP_LIBRARY_VESRION
#

# AM_MINISIP_PACKAGE_INIT()
# -------------------------
AC_DEFUN([AM_MINISIP_PACKAGE_INIT],[
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
case $host_os in
     *mingw* )
	     os_win=yes
	     ;;
esac
AM_CONDITIONAL(OS_WIN, test x$os_win = xyes)

dnl Checks for programs.
AC_PROG_CXX
AC_PROG_CPP
AC_PROG_LN_S
AC_PROG_MAKE_SET

if test x$os_win = xyes; then
    AC_CHECK_TOOL(WINDRES, [windres])
    if test x$WINDRES = x; then
        AC_MSG_ERROR([Could not find windres in your PATH.])
    fi
fi
AC_C_BIGENDIAN
AC_LANG(C++)
dnl For now, STL is made mandatory
dnl AC_ARG_ENABLE(stl,
dnl	AS_HELP_STRING([--enable-stl],
dnl		[enables the use of C++ STL (default enabled)]),
dnl		[ 
dnl		if test "${enable_stl}" = "yes"; then
AC_DEFINE(USE_STL, [], [STL enabled])
dnl		fi ])
PKG_PROG_PKG_CONFIG
])
# End of AM_MINISIP_PACKAGE_INIT
#

dnl  =================================================================
dnl               minisip configure check helper macros

# AM_MINISIP_LIBTOOL_EXTRAS()
# ---------------------------
AC_DEFUN([AM_MINISIP_LIBTOOL_EXTRAS],[
AC_LIBTOOL_DLOPEN
AC_LIBTOOL_WIN32_DLL
])
# End of AM_MINISIP_LIBTOOL_EXTRAS
#

# AM_MINISIP_ENABLE_DEBUG()
# -------------------------
AC_DEFUN([AM_MINISIP_ENABLE_DEBUG],[
AC_ARG_ENABLE(debug,
	AS_HELP_STRING([--enable-debug],
		[enables debug output. (default disabled)]), [ 
if test "${enable_debug}" = "yes"
then
	AC_DEFINE(DEBUG_OUTPUT, [], [Debug output])
else
	AC_DEFINE(NDEBUG, [], [No debug output])
fi 
		])
	])
# End of AM_MINISIP_ENABLE_DEBUG
#

dnl  =================================================================
dnl               minisip `configure --with-m*` argument macros

# AC_MINISIP_CHECK_WITH_ARG(MACRO, NAME, LIBS)
# --------------------------------------------
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
# -----------------------------------------------------------
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
# --------------------------------------------------------------
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

# AC_MINISIP_CHECK_LIBRARY(MACRO, NAME, HEADER, LIB)
# --------------------------------------------------
AC_DEFUN([AC_MINISIP_CHECK_LIBRARY], [
		save_CPPFLAGS="${CPPFLAGS}"
		save_LIBS="${LIBS}"
		CPPFLAGS="${$1_CFLAGS} ${CPPFLAGS}"
		LIBS="${$1_LIBS} ${LIBS}"
		AC_CHECK_HEADER([$2/$3],[],[
			AC_MSG_ERROR([You need the $2 headers/library.
Try installing the $2-devel package for your distribution."])])

		AC_CHECK_LIB([$4], [main], [], [ dnl
				AC_MSG_ERROR([Could not find $2. dnl
Please install the corresponding package.]) dnl
			])
				
		CPPFLAGS="${save_CPPFLAGS}"
		LIBS="${save_LIBS}"
	])
# End of AC_MINISIP_CHECK_LIBRARY
#

dnl  =================================================================
dnl                   minisip configure completion macros

# AC_MINISIP_VERSION_SUBST(MACRO, PREFIX)
# ---------------------------------------
AC_DEFUN([AC_MINISIP_VERSION_SUBST], [
		$1_MAJOR_VERSION=$2_major_version
		$1_MINOR_VERSION=$2_minor_version
		$1_MICRO_VERSION=$2_micro_version
		AC_SUBST($1_MAJOR_VERSION) dnl
		AC_SUBST($1_MINOR_VERSION) dnl
		AC_SUBST($1_MICRO_VERSION) dnl
	])
# End of AM_MINISIP_VERSION_SUBST
#

# AC_MINISIP_CHECK_COMPLETE()
# ---------------------------
AC_DEFUN([AM_MINISIP_CHECK_COMPLETE],[ 
		MINISIP_CFLAGS="-I\$(top_srcdir)/include ${MINISIP_CFLAGS}"
		AC_SUBST(MINISIP_CFLAGS)
		AC_SUBST(MINISIP_LIBS)
		dnl process AM_MINISIP_PACKAGE_VERSION
		AC_MINISIP_VERSION_SUBST( 
			MINISIP_PACKAGE_MACRO,
			MINISIP_PACKAGE_PREFIX) dnl
		dnl process AM_MINISIP_LIBRARY_VERSION if it was invoked
		if test x"lt_current" != "xlt_current"; then
			LT_VERSION_INFO="lt_current:lt_revision:lt_age"
			LT_CURRENT_MINUS_AGE="lt_minus_age"
			AC_SUBST(LT_VERSION_INFO) dnl
			AC_SUBST(LT_CURRENT_MINUS_AGE) dnl
			m_no_undef="-Wl,--no-undefined -no-undefined"
			m_lt_version="-version-info ${LT_VERSION_INFO}"
			MINISIP_LIBRARY_LDFLAGS="${m_no_undef} ${m_lt_version}"
			AC_SUBST(MINISIP_LIBRARY_LDFLAGS) dnl
		fi
		AC_SUBST(ACLOCAL_FLAGS)
	])
# End of AC_MINISIP_CHECK_COMPLETE
#

dnl  =================================================================
dnl                          libmutil macros

# AM_MINISIP_CHECK_LIBMUTIL(VERSION)
# ----------------------------------
AC_DEFUN([AM_MINISIP_CHECK_LIBMUTIL],[ 
	AC_MINISIP_WITH_ARG(MUTIL, mutil, libmutil, $1, [REQUIRED])
	AC_MINISIP_CHECK_LIBRARY(MUTIL, libmutil, libmutil_config.h, mutil)
  ])
# End of AM_MINISIP_CHECK_LIBMUTIL
#
