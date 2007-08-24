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

# AM_MINISIP_VERSION_GEN()
# ------------------------
AC_DEFUN([AM_MINISIP_VERSION_GEN],[
if test -e .version; then
  version=`cat .version`
elif test -e ${srcdir}/.version; then
  version=`cat ${srcdir}/.version`
else
  if test -e ${srcdir}/.svnrevision; then
    svnrevision=`cat ${srcdir}/.svnrevision`
  else
    svnrevision=`LANG=C svnversion -c ${srcdir} | cut -d: -f2`
  fi

  version="MINISIP_PACKAGE_VERSION+r${svnrevision}"
fi

  version_old=

  if test -e version; then
     version_old=`cat version`
  fi

  if test "$version" != "$version_old"; then
     cat > "include/version.h" <<EOF
#define PACKAGE_VERSION_FULL "$version"
#define PACKAGE_STRING_FULL "${PACKAGE_NAME} $version"
EOF

     cat > "version" <<EOF
$version
EOF
  else
     echo "config.status: include/version.h is unchanged"
  fi

  VERSION_FULL=$version
])
# End of AM_MINISIP_VERSION_GEN

# AM_MINISIP_VERSION_INIT()
# ---------------------------------
AC_DEFUN([AM_MINISIP_VERSION_INIT], [

test -d "include" || mkdir "include"

AC_CONFIG_COMMANDS([include/version.h], [
  AM_MINISIP_VERSION_GEN
],[
  PACKAGE_NAME=$PACKAGE_NAME
])

AC_DEFINE([HAVE_VERSION_H], [1], [Define to 1 if you have `include/version.h'])
AC_DEFINE([VERSION], [PACKAGE_VERSION_FULL], [Version number of package])
AC_DEFINE([PACKAGE_VERSION], [PACKAGE_VERSION_FULL], [Define to the version of this package.])
AC_DEFINE([PACKAGE_STRING], [PACKAGE_STRING_FULL], [Define to the full name and version of this package.])
AM_MINISIP_VERSION_GEN

VERSION=$VERSION_FULL
PACKAGE_VERSION=$VERSION_FULL
PACKAGE_STRING="${PACKAGE_NAME} ${VERSION_FULL}"
])
# End of AM_MINISIP_VERSION_INIT

# AM_MINISIP_PACKAGE_INIT()
# -------------------------
AC_DEFUN([AM_MINISIP_PACKAGE_INIT],[
AM_INIT_AUTOMAKE
AM_MAINTAINER_MODE
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
case $host_os in
     *mingw* )
	     os_win=yes
	     ;;
esac
AM_CONDITIONAL(OS_WIN, test x$os_win = xyes)

dnl Checks for programs.
AC_C_BIGENDIAN
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_LN_S
AC_PROG_MAKE_SET

if test x$os_win = xyes; then
    AC_CHECK_TOOL(WINDRES, [windres])
    if test x$WINDRES = x; then
        AC_MSG_ERROR([Could not find windres in your PATH.])
    fi
fi
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
AM_MINISIP_VERSION_INIT
dnl AM_MINISIP_CHECK_TIME
])
# End of AM_MINISIP_PACKAGE_INIT
#

dnl  =================================================================
dnl               minisip configure check helper macros

# AC_MINISIP_CHECK_LIBTOOL()
# --------------------------
AC_DEFUN([AC_MINISIP_CHECK_LIBTOOL],[
  # check for libtool >= 1.5.7
  minisip_ltvers="`libtoolize --version 2>&1 | \
	perl -e '$x = scalar(<STDIN>); $x =~ /1\.5\.(\d+)/ && print $[]1'`"
  minisip_ltvers_patched="`libtoolize --version 2>&1| \
	perl -e '$y = scalar(<STDIN>); $y =~ /1\.5\.6\.(\d+)/ && print $[]1'`"

  #AC_MSG_WARN([  DEBUG!!!! minisip_ltvers=$minisip_ltvers ])
  #AC_MSG_WARN([  DEBUG!!!! minisip_ltvers_patched=$minisip_ltvers_patched ])
  
  #if >= 1.5.7, or a 1.5.6.1 version (patched), then we won't have problems ...
  if test "${minisip_ltvers}" && test ${minisip_ltvers} -gt 6; then
    minisip_has_lt157=yes
  else
	if test "${minisip_ltvers_patched}" && test ${minisip_ltvers_patched} -gt 0; then
		AC_MSG_WARN([ Detected patched libtool/ltmain.sh (1.5.6.1) ])
		minisip_has_lt156_patched=yes
  	fi
  fi
])
# End of AC_MINISIP_CHECK_LIBTOOL
#

# AM_MINISIP_LIBTOOL_EXTRAS()
# ---------------------------
AC_DEFUN([AM_MINISIP_LIBTOOL_EXTRAS],[
AC_LIBTOOL_DLOPEN
AC_LIBTOOL_WIN32_DLL
AC_MINISIP_CHECK_LIBTOOL
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
AM_CONDITIONAL(DEBUG, test "${enable_debug}" = "yes")
	])
# End of AM_MINISIP_ENABLE_DEBUG
#

# AM_MINISIP_ENABLE_TEST_SUITE()
# ------------------------------
AC_DEFUN([AM_MINISIP_ENABLE_TEST_SUITE],[
AC_ARG_ENABLE(test-suite,
	AS_HELP_STRING([--enable-test-suite],
		[enables extended test suite. (default disabled)]))
if test x${enable_test_suite} = xyes; then
	AC_DEFINE(TEST_SUITE, [], [Build and run extended test suite])
fi 
AM_CONDITIONAL(TEST_SUITE, test x${enable_test_suite} = xyes)
	])
# End of AM_MINISIP_ENABLE_TEST_SUITE
#

# AM_MINISIP_CHECK_TIME
# ----------------------------------
AC_DEFUN([AM_MINISIP_CHECK_TIME],[
	AC_HEADER_TIME
	AC_CHECK_TYPE([struct timezone],[
		EXTERNAL_CFLAGS="$EXTERNAL_CFLAGS -DHAVE_STRUCT_TIMEZONE"
	],,[dnl
#if TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif
	])

	AC_CHECK_FUNC([gettimeofday],[
		EXTERNAL_CFLAGS="$EXTERNAL_CFLAGS -DHAVE_GETTIMEOFDAY"
	])

	AC_SUBST(EXTERNAL_CFLAGS)
])
# End of AM_MINISIP_CHECK_TIME
#

dnl  =================================================================
dnl               minisip `configure --with-m*` argument macros

# AC_MINISIP_CHECK_WITH_ARG(MACRO, NAME, LIBS [, ACTION-IF-NOT-FOUND])
# --------------------------------------------
AC_DEFUN([AC_MINISIP_CHECK_WITH_ARG],[
		#AC_MSG_WARN([  withval = ${withval} ])
		if test "x${withval}" = "xyes"; then
			# proceed with default installation
			$1_NEEDS_PKG_CHECK=yes
		else
			# work around for pre-1.5.7 libtool bug:
			#  1.5.6 adds .libs by mistake, so only add that
			#  portion of the path if we have 1.5.7 or later
			#  After this, we also have to fix the library check.
			if test -n "${minisip_has_lt157}"; then
				#AC_MSG_WARN([  minisip_has_lt157 ])
				minisip_lthack='/.libs'
			fi
			if test -n "${minisip_has_lt156_patched}"; then
				#AC_MSG_WARN([  minisip_has_lt156_patched ])
				minisip_lthack='/.libs'
			fi
			if test -d "${withval}/lib"; then
				# specific installation
				$1_LDFLAGS="-L${withval}/lib"
			elif test -d "${withval}/.libs"; then
				# in-tree development
				$1_LDFLAGS="-L${withval}${minisip_lthack}"
			elif test -d "../$2/.libs"; then
				# out-of-tree development
				$1_LDFLAGS="-L`pwd`/../$2${minisip_lthack}"
			else
				ifelse([$4], ,[
				AC_MSG_ERROR([dnl
Unable to find the required libraries in any of the following locations:
	${withval}/lib
	${withval}/.libs
	../$2/.libs

Maybe you forgot to compile $2 first?
])], [$4])
			fi
dnl 			AC_MSG_WARN([  DEBUG_INFO: param1=$1, param2=$2, param3=$3; $1_LDFLAGS=${$1_LDFLAGS} ])
			$1_CFLAGS="-I${withval}/include"
			$1_LIBS="${$1_LDFLAGS} $3"
			AC_SUBST($1_CFLAGS)
			AC_SUBST($1_LIBS)
		fi
	])
# End of AC_MINISIP_CHECK_WITH_ARG
#

# AC_MINISIP_MAYBE_WITH_ARG(MACRO, WITHARG, NAME, TYPE, LIBS [, ACTION-IF-NOT-FOUND]])
# -----------------------------------------------------------
AC_DEFUN([AC_MINISIP_MAYBE_WITH_ARG],[
		if test "x${withval}" = "no"; then
			ifelse([$6], , [AC_MINISIP_$4_LIB($1, $3)], [$6])
		else
			AC_MINISIP_CHECK_WITH_ARG($1, $3, [-l$2 $5], $6)
		fi
	])
# End of AC_MINISIP_MAYBE_WITH_ARG
#

# AC_MINISIP_WITH_ARG(MACRO, WITHARG, NAME, VERSION, TYPE, LIBS [, ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
# --------------------------------------------------------------
AC_DEFUN([AC_MINISIP_WITH_ARG],[
		AC_BEFORE([AM_MINISIP_CHECK_]m4_translit($3, 'a-z', 'A-Z'),
			[AM_MINISIP_CHECK_COMPLETE]) dnl
		AC_PROVIDE([AC_MINISIP_CHECK_PERFORMED]) dnl
		AC_ARG_WITH($2,
			AS_HELP_STRING([--with-$2=PATH], [location of $3]),
			[ AC_MINISIP_MAYBE_WITH_ARG($1, $2, $3, $5, $6, $8) ],
			[ $1_NEEDS_PKG_CHECK=yes ])
		if test "x${$1_NEEDS_PKG_CHECK}" = "xyes"; then
			ifelse([$8], , [dnl
				echo "Check $1 module"
				PKG_CHECK_MODULES($1, [$3 >= $4])
				ifelse([$7], , :, [$7])], [dnl
				PKG_CHECK_EXISTS([$3 >= $4], [dnl
					PKG_CHECK_MODULES($1, [$3 >= $4])
					$7],
					[$8])])
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

# AC_MINISIP_CHECK_LIBRARY(MACRO, NAME, HEADER, LIB [,ACTION-IF-NOT-FOUND])
# --------------------------------------------------
AC_DEFUN([AC_MINISIP_CHECK_LIBRARY], [
		save_CPPFLAGS="${CPPFLAGS}"
		save_LIBS="${LIBS}"
		CPPFLAGS="${$1_CFLAGS} ${CPPFLAGS}"
		LIBS="${$1_LIBS} ${LIBS}"
		AC_CHECK_HEADER([$2/$3],[],[
			AC_MSG_ERROR([You need the $2 headers/library.
Try installing the $2-devel package for your distribution."])])

		# fix library check for pre-1.5.7 libtool:
		#  add the correct path, since the double .libs bug doesn't
		#  seem to affect us here.
		if test -n "${minisip_lthack}"; then
			LIBS="${$1_LDFLAGS}/.libs ${LIBS}"
		fi
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

# AC_MINISIP_CHECK_PERFORMED()
# ------------------------------------
AC_DEFUN([AC_MINISIP_CHECK_PERFORMED], [ dnl
		AC_MSG_ERROR([MINISIP_CHECK_COMPLETE called before any MINISIP_CHECK_* calls]) ])
# End of AC_MINISIP_CHECK_PERFORMED
#

# AM_MINISIP_CHECK_NOTHING()
# --------------------------
AC_DEFUN([AM_MINISIP_CHECK_NOTHING], [ dnl
		AC_PROVIDE([AC_MINISIP_CHECK_PERFORMED]) ])
# End of AM_MINISIP_CHECK_NOTHING
#


# AC_MINISIP_CHECK_COMPLETE()
# ---------------------------
AC_DEFUN([AM_MINISIP_CHECK_COMPLETE],[ 
		AC_REQUIRE([AC_MINISIP_CHECK_PERFORMED])
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

# AM_MINISIP_CHECK_LIBMUTIL(VERSION [, ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
# ----------------------------------
AC_DEFUN([AM_MINISIP_CHECK_LIBMUTIL],[
	mutil_found=yes 
	AC_MINISIP_WITH_ARG(MUTIL, mutil, libmutil, $1, ifelse([$3], , [REQUIRED], [OPTIONAL]), ,[mutil_found=yes], [mutil_found=no])
	if test ! "${mutil_found}" = "no"; then
		AC_MINISIP_CHECK_LIBRARY(MUTIL, libmutil, libmutil_config.h, mutil,, mutil_found=no)
	fi
		
	if test "${mutil_found}" = "yes"; then
		ifelse([$2], , :, [$2])
	else
		ifelse([$3], , [AC_MINISIP_REQUIRED_LIB(MUTIL, libmutil)], [$3])
	fi
  ])
# End of AM_MINISIP_CHECK_LIBMUTIL
#
