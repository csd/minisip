dnl                                               -*- Autoconf -*-
dnl Process this file with autoconf to produce a configure script.

dnl AC_PREREQ(2.57)

m4_MINISIP_PACKAGE_VERSION(ministun, ministun, 0, 5, 0)
m4_MINISIP_PACKAGE_CONTACT([The Minisip developer team], [minisip-users@minisip.org])
dnl m4_MINISIP_PACKAGE_RELEASED

AC_INIT(MINISIP_PACKAGE_NAME, MINISIP_PACKAGE_VERSION, MINISIP_PACKAGE_CONTACT)
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_MACRO_DIR(m4)
AM_MINISIP_PACKAGE_INIT
AM_CONFIG_HEADER(include/compilation_config.h)

AM_MINISIP_LIBTOOL_EXTRAS
AC_PROG_LIBTOOL

dnl
dnl Linker flags
dnl mingw needs --enable-runtime-pseudo-reloc since it can`t
dnl auto import all symbols.
dnl
RELOC_LDFLAGS="-Wl,--enable-runtime-pseudo-reloc"
AC_MSG_CHECKING([Linker flag ${RELOC_LDFLAGS}])
cf_save_LDFLAGS="${LDFLAGS}"
LDFLAGS="${LDFLAGS} ${RELOC_LDFLAGS}"
reloc_result="no"
AC_LINK_IFELSE([
int main() {
    return 0;
}
],[reloc_result="yes"],[RELOC_LDFLAGS=""])
LDFLAGS="${cf_save_LDFLAGS}"
AC_MSG_RESULT(${reloc_result})
AC_SUBST(RELOC_LDFLAGS)

dnl Add linker flag to enable the stack trace
LDFLAGS="${LDFLAGS} -rdynamic"

AC_CHECK_TYPES([uint])

AM_MINISIP_ENABLE_DEBUG
AM_MINISIP_ENABLE_TEST_SUITE

AM_MINISIP_CHECK_LIBMUTIL([0.8.0])
AM_MINISIP_CHECK_LIBMNETUTIL([0.8.0])
AM_MINISIP_CHECK_LIBMSTUN([0.8.0])
AM_MINISIP_CHECK_COMPLETE

AC_HEADER_STDC
AC_CHECK_HEADERS([malloc.h stdlib.h string.h unistd.h])

AC_C_CONST

AC_CONFIG_FILES([
		Makefile
		ministun.pc
		include/Makefile
		m4/Makefile
		source/Makefile
		win32/Makefile
		win32/ministun-res.rc
		win32/ministund-res.rc
	])
AC_OUTPUT
