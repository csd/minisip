dnl                                               -*- Autoconf -*-
dnl Process this file with autoconf to produce a configure script.

dnl AC_PREREQ(2.57)

m4_MINISIP_PACKAGE_VERSION(libmutil, mutil, 0, 8, 0)
m4_MINISIP_PACKAGE_CONTACT([Erik Eliasson], [eliasson@imit.kth.se])
dnl m4_MINISIP_PACKAGE_RELEASED
m4_MINISIP_LIBRARY_VERSION(0, 0, 0)

AC_INIT(MINISIP_PACKAGE_NAME, MINISIP_PACKAGE_VERSION, MINISIP_PACKAGE_CONTACT)
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_MACRO_DIR(m4)
AM_MINISIP_PACKAGE_INIT
AM_CONFIG_HEADER(include/compilation_config.h)

AC_LIBLTDL_CONVENIENCE
AM_MINISIP_LIBTOOL_EXTRAS
AC_PROG_LIBTOOL
AC_SUBST(LIBLTDL)
AC_SUBST(LTDLINCL)

AM_MINISIP_ENABLE_DEBUG

dnl debug flag
AC_ARG_ENABLE(memdebug,
    AS_HELP_STRING([--enable-memdebug],
	[enables memory debugging code. (default disabled)]),
    [ if test "${enable_memdebug}" = "yes"
      then
        AC_DEFINE(MDEBUG, [], [Debug memory])
      fi ])

AM_MINISIP_ENABLE_TEST_SUITE

AC_ARG_ENABLE(color-terminal,
    AS_HELP_STRING([--enable-color-terminal], 
	[enables color coding of some debug messages (default disabled)]),
    [ if test "${enable_color_terminal}" = "yes"
      then
        AC_DEFINE(COLOR_TERMINAL, [], [Color terminal support])
      fi ])

AC_ARG_WITH(pthread,
        [AS_HELP_STRING([--with-pthread],
                [location of the pthread library])],
        [pthread_path=$withval], [pthread_path="yes"])

dnl
dnl pthread
dnl
have_pthread="no"
if test ! x$pthread_path = xno; then

   cf_save_CPPFLAGS="${CPPFLAGS}"
   cf_save_LDFLAGS="${LDFLAGS}"
   cf_save_LIBS="${LIBS}"

   if test ! x$pthread_path = xyes; then
      CPPFLAGS="${CPPFLAGS} -I${pthread_path}"
      LDFLAGS="${LDFLAGS} -L${pthread_path}"
   fi

   dnl Pthreads-win32 depends on Windows libraries, check first
   if test x$os_win = xyes; then
      dnl Check for Pthreads-win32 begin

      have_pthread=yes
      AC_CHECK_LIB([winmm], [main])
      AC_CHECK_LIB([wsock32], [main])

      AC_SEARCH_LIBS([pthread_mutex_init], [pthreadGCE2 pthreadGCE1 pthreadGCE],, [have_pthread=no])
      AC_CHECK_HEADERS([pthread.h semaphore.h],,[have_pthread=no])

   dnl Check for Pthreads-win32 end
   else
      dnl Check for native pthread begin
      have_pthread=yes

      AC_CHECK_LIB([pthread], [pthread_attr_init],,[have_pthread=no])
      AC_CHECK_HEADERS([pthread.h semaphore.h],,[have_pthread=no])

      PTHREAD_CFLAGS="-D_REENTRANT"
      dnl Check for native pthread end
   fi

   if test x$have_pthread = xno; then
      LIBS="${cf_save_LIBS}"
   fi
   CPPFLAGS="${cf_save_CPPFLAGS}"
   LDFLAGS="${cf_save_LDFLAGS}"
fi

if test x$have_pthread = xyes; then
   if test ! x$pthread_path = xyes; then
      PTHREAD_CFLAGS="${PTHREAD_CFLAGS} -I${pthread_path}"
      PTHREAD_LIBS="-L${pthread_path}"
      PTHREAD_PATH="${pthread_path}"
   fi
else
   if test ! x$os_win = xyes; then
      AC_MSG_ERROR([POSIX thread library and headers are required.])
   fi
fi

AC_SUBST(PTHREAD_CFLAGS)
AC_SUBST(PTHREAD_LIBS)
AC_SUBST(PTHREAD_PATH)

AM_CONDITIONAL(HAVE_PTHREAD, test "${have_pthread}" = "yes")

dnl FIXME: Replace `main' with a function in `-ldl':
AC_CHECK_LIB([dl], [dlopen])

dnl Check for stack trace support
AC_CHECK_HEADER([execinfo.h],
	[
		AC_DEFINE(HAVE_EXECINFO_H, [], [Stack trace functionality found])
		HAVE_EXECINFO_H="yes"
	]
)
AM_CONDITIONAL(HAVE_EXECINFO_H, test "${HAVE_EXECINFO}" = "yes")


dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([malloc.h stdlib.h string.h unistd.h pthread.h termios.h semaphore.h])

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_HEADER_TIME
AM_MINISIP_CHECK_TIME
AM_MINISIP_CHECK_NOTHING
AM_MINISIP_CHECK_COMPLETE

AC_CONFIG_FILES([ 
		Makefile
		examples/Makefile
		include/Makefile
		m4/Makefile
		win32/Makefile
		debian/Makefile
		win32/libmutil-res.rc
		libmutil.pc
	])

dnl Configure libltdl
AC_CONFIG_SUBDIRS(libltdl)

AC_OUTPUT

