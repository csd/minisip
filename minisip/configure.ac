dnl                                               -*- Autoconf -*-
dnl Process this file with autoconf to produce a configure script.

dnl AC_PREREQ(2.57)

m4_MINISIP_PACKAGE_VERSION(minisip, minisip_ui, 0, 8, 0)
m4_MINISIP_PACKAGE_CONTACT([Erik Eliasson], [eliasson@imit.kth.se])
dnl m4_MINISIP_PACKAGE_RELEASED

AC_INIT(MINISIP_PACKAGE_NAME, MINISIP_PACKAGE_VERSION, MINISIP_PACKAGE_CONTACT)
AC_CONFIG_AUX_DIR(config)
dnl AC_CONFIG_MACRO_DIR(m4)
AM_MINISIP_PACKAGE_INIT
AM_CONFIG_HEADER(include/compilation_config.h)

AM_MINISIP_LIBTOOL_EXTRAS
AC_PROG_LIBTOOL

dnl HACK: QT is compiled, unless GTK or textui is
dnl Update: QT disabled for now
QTGUI="no"

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

dnl Check types
AC_CHECK_TYPES([uint])

dnl debug flag
AC_ARG_ENABLE(autocall,
    AS_HELP_STRING([--enable-autocall],
        [enables automatic calling for debug purposes (default disabled)]),
    [ if test "${enable_autocall}" = "yes"
      then
        AC_DEFINE(MINISIP_AUTOCALL, [], [Debug autocalling])
      fi ])

dnl compile for iPAQ
AC_ARG_ENABLE(ipaq,
    AS_HELP_STRING([--enable-ipaq],
        [enables various fixes for the iPAQ (default disabled)]),
    [ if test "${enable_ipaq}" = "yes"
      then
        AC_DEFINE(IPAQ, [], [Compile for iPAQ])
      fi ])

AM_MINISIP_ENABLE_DEBUG
AM_MINISIP_ENABLE_TEST_SUITE

dnl P2T support
AC_ARG_ENABLE(p2t,
    AS_HELP_STRING([--enable-p2t],
        [enables push-2-talk features (default disabled)]),
    [  
      if test "${enable_p2t}" = "yes"
      then
	AC_DEFINE(P2T_SUPPORT, [], [Push-to-talk support])
      fi
    ]
)
AM_CONDITIONAL(P2T_SUPPORT, test "${enabled_p2t}" = "yes")

dnl #############
dnl VIDEO SUPPORT
dnl #############

AC_ARG_ENABLE(video,
    AS_HELP_STRING([--enable-video],
        [enables video features (default disabled)]),
    [ if test "${enable_video}" = "yes"
      then
	  AC_DEFINE(VIDEO_SUPPORT, [], [Compile video features])
      fi ])
AM_CONDITIONAL(VIDEO_SUPPORT, test "${enable_video}" = "yes")

dnl TEXT UI
AC_ARG_ENABLE(textui,
    AS_HELP_STRING([--enable-textui],
        [enables the text based user interface (default disabled)]),
    [ if test "${enable_textui}" = "yes"
      then
	  AC_DEFINE(TEXT_UI, [], [Use the text based user interface])
dnl          QTGUI="no"
dnl	  enable_gtk="no"
      fi ])
AM_CONDITIONAL(TEXT_UI, [test "${enable_textui}" = "yes"])


dnl OpenGL GUI
AC_ARG_ENABLE(opengl,
    AS_HELP_STRING([--enable-opengl],
        [enables the OpenGL based full-screen user interface (default disabled)]),
    [ if test "${enable_opengl}" = "yes"
      then
	  AC_DEFINE(OPENGL_GUI, [], [Use the OpenGL full screen user interface])
	  LDFLAGS="${LDFLAGS} -lminisip_video"
      fi ])
AM_CONDITIONAL(OPENGL_GUI, [test "${enable_opengl}" = "yes"])



dnl Qt GUI
AC_ARG_ENABLE(qtui,
    AS_HELP_STRING([--enable-qtui],
        [enables the Qt based user interface (default disabled)]),
    [ if test "${enable_qtui}" = "yes"
      then
	  AC_DEFINE(QT_GUI, [], [Use the Qt based user interface])
dnl          QTGUI="yes"
dnl	  enable_gtk="no"
	  
      fi ])
AM_CONDITIONAL(QT_GUI, [test "${enable_qtui}" = "yes"])




dnl CONF UI
AC_ARG_ENABLE(confgui,
    AS_HELP_STRING([--enable-confgui],
        [enables the conference user interface (default disabled)]),
    [ if test "${enable_confgui}" = "yes"
      then
          AC_DEFINE(CONF_GUI, [], [Compile the conference bridge application])
      fi ])
AM_CONDITIONAL(CONF_GUI, [test "${enable_confgui}" = "yes"])




dnl GTK GUI
AC_ARG_ENABLE(gtk,
    AS_HELP_STRING([--disable-gtk],
        [disables the GTK interface (default enabled).]))

if test "x${enable_gtk}" != "xno"
	then
		PKG_CHECK_MODULES(GLADEMM, [libglademm-2.4 >= 2.6],
			[AC_DEFINE(HAVE_LIBGLADEMM_2_6, [1], [Define to 1 if you have libglademm version 2.6 or newer])
			have_glademm=yes],
			[PKG_CHECK_MODULES(GLADEMM, [libglademm-2.4], 
				[have_glademm=yes],
				[PKG_CHECK_MODULES(GLADEMM, [libglademm-2.0],
					[AC_DEFINE(OLDLIBGLADEMM, [], [Use libglademm-2.0])
					have_glademm=yes],
					[AC_MSG_ERROR([Could not find the glademm-2.0 library])]
				)]
			)]
		)

		if test x${have_glademm} = xyes; then
			AC_DEFINE(GTK_GUI, [], [Use the GTK GUI])
		fi

		cf_save_CPPFLAGS="${CPPFLAGS}"
		CPPFLAGS="${CPPFLAGS} ${GLADEMM_CFLAGS}"
		AC_CHECK_HEADERS([glibconfig.h])
		AC_CHECK_DECL(G_THREADS_ENABLED, 
				[PKG_CHECK_MODULES(GTHREAD, [gthread-2.0],,[
				AC_MSG_ERROR([Could not find the gthread-2.0 library])
				])
				],, [
				#ifdef HAVE_GLIBCONFIG_H
				#include<glibconfig.h>
				#endif
		])
		CPPFLAGS="${cf_save_CPPFLAGS}"

		dnl    QTGUI="no"
fi

AM_CONDITIONAL(GTK_GUI, [test "x${enable_gtk}" != "xno"])

PKG_CHECK_MODULES(HILDON, [hildon-libs],
	[
	 	AC_DEFINE(HILDON_SUPPORT, [], [Support the Hildon extensions])
		hildon_support="yes"
	],[echo 'No Hildon support.'])

AM_CONDITIONAL(HILDON_SUPPORT, [test "${hildon_support}" = "yes"])

PKG_CHECK_MODULES(OSSO, [libosso],
	[
	 	AC_DEFINE(OSSO_SUPPORT, [], [Support the libosso library])
		osso_support="yes"
	],[echo 'No libosso support.'])

AM_CONDITIONAL(OSSO_SUPPORT, [test "${osso_support}" = "yes"])


QT="qt"

dnl QT embedded
AC_ARG_ENABLE(qte,
    AS_HELP_STRING([--enable-qte],
        [enables Qt Embedded support (default disabled)]),
    [ if test "${enable_qte}" = "yes"
      then
      	QT="qte"
	CXXFLAGS="${CXXFLAGS} -fno-rtti"
	AC_DEFINE(QWS, [], [Use Qt Embedded windowing system])
      fi ])

if test "${QTGUI}" = "yes"
then

dnl Set the QTDIR directory
AC_ARG_WITH(qtdir,
    AS_HELP_STRING([--with-qtdir=DIR],
        [QT installation directory (default /usr)]),
    [ if test "${with_qtdir}" != "no"
      then
        QTDIR="${with_qtdir}"
      fi ])

if test -z "${QTDIR}"; then
  QTDIR="/usr"
fi

CPPFLAGS="${CPPFLAGS} -I`pwd`/include -I${QTDIR}/include"


dnl Look for the QT moc program

MOC_PATH="${QTDIR}/bin:${PATH}"
AC_PATH_PROG(MOC, moc, no, ${MOC_PATH} )
if test "${MOC}" = "no"
then
  AC_MSG_ERROR([Could not find the moc program, required for the QT interface. You may supply the QT installation directory with --with-qtdir=])
fi
AC_SUBST(MOC)
fi

AM_MINISIP_CHECK_LIBMUTIL([0.8.0])
AM_MINISIP_CHECK_LIBMNETUTIL([0.8.0])
AM_MINISIP_CHECK_LIBMCRYPTO([0.8.0])
AM_MINISIP_CHECK_LIBMSIP([0.8.0])
AM_MINISIP_CHECK_LIBMIKEY([0.8.0])
AM_MINISIP_CHECK_LIBMINISIP([0.8.0])
AM_MINISIP_CHECK_COMPLETE

dnl Checks for libraries.

dnl Check for QT libraries
if test "${QTGUI}" = "yes"
then
AC_CHECK_LIB([${QT}-mt], [main], ,[
  AC_CHECK_LIB([${QT}], [main], ,[
    AC_MSG_ERROR([Could not find the QT libraries])
  ],[-L${QTDIR}/lib -lstdc++])
],[-L${QTDIR}/lib -lstdc++])

QT_LIB_LOCATION="${QTDIR}/lib"
AC_SUBST(QT_LIB_LOCATION)
fi

dnl Check for the QT headers
if test "${QTGUI}" = "yes"
then
dnl Self installed case
AC_CHECK_HEADER(qapplication.h,[QT_INCLUDE="${QTDIR}/include"],
  [
dnl "Usual" case I guess
  AC_CHECK_HEADER(qt/qapplication.h,[QT_INCLUDE="${QTDIR}/include/qt"],
   [ 
dnl Debian case
    AC_CHECK_HEADER(qt3/qapplication.h,[QT_INCLUDE="${QTDIR}/include/qt3"],AC_MSG_ERROR(["Could not find the QT headers. You may specify your QT installation directory with --with-qtdir="]))
    ])
  ])
AC_SUBST(QT_INCLUDE)
fi

dnl OPIE
AC_ARG_ENABLE(opie,
    AS_HELP_STRING([--enable-opie],
      [enables Open Palmtop Integrated Environment support (default disabled)]),
    [ if test "${enable_opie}" = "yes"
      then
	LDFLAGS="${LDFLAGS} -L${QT_LIB_LOCATION} -L${OPIEDIR}/lib"
	dnl FIXME: Replace `main' with a function in `-lX11':
	AC_CHECK_LIB([X11], [main])
	dnl FIXME: Replace `main' with a function in `-lXext':
	AC_CHECK_LIB([Xext], [main])
	dnl FIXME: Replace `main' with a function in `-lm':
	AC_CHECK_LIB([m], [main])
	dnl FIXME: Replace `main' with a function in `-lnsl':
	AC_CHECK_LIB([nsl], [main])
        AC_CHECK_LIB([qpe], [main],,
           [AC_MSG_ERROR([Could not find the QPE library])]
	)
        AC_CHECK_LIB([opie], [main],,
           [AC_MSG_ERROR([Could not find the OPIE library])]
	)
        AC_DEFINE(OPIE, [], [Use OPIE library])
	CPPFLAGS="${CPPFLAGS} -I${OPIEDIR}/include"
      fi ])

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([arpa/inet.h fcntl.h limits.h malloc.h netdb.h netinet/in.h stdlib.h string.h sys/ioctl.h sys/socket.h sys/time.h syslog.h unistd.h])

AC_C_CONST
AC_HEADER_TIME

AC_SUBST(datadir)

AC_CONFIG_FILES([minisip/Makefile
			include/Makefile
			minisip/gui/Makefile
			minisip/gui/qtgui/Makefile
			minisip/gui/gtkgui/Makefile
			minisip/gui/textui/Makefile
			minisip/gui/openglgui/Makefile
			minisip/gui/confgui/Makefile
			minisip/win32/Makefile
			minisip/win32/minisip-res.rc
			share/Makefile
			debian/Makefile
			Makefile])
AC_OUTPUT
