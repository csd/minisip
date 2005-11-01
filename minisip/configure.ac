dnl                                               -*- Autoconf -*-
dnl Process this file with autoconf to produce a configure script.

dnl AC_PREREQ(2.57)
AC_INIT(minisip, 0.7, BUG-REPORT-ADDRESS)
dnl AC_CONFIG_HEADER([config.h])
dnl AM_CONFIG_HEADER([config.h])

AM_INIT_AUTOMAKE(minisip, 0.7.1)

AM_CONFIG_HEADER(include/compilation_config.h)

dnl Checks for programs.
AC_PROG_CXX
AC_PROG_CPP
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_LIBTOOL_DLOPEN
AC_PROG_LIBTOOL

AC_C_BIGENDIAN

AC_LANG(C++)

AM_CPPFLAGS="-I\$(top_srcdir)/include"

dnl HACK: QT is compiled, unless GTK or textui is
dnl Update: QT disabled for now
QTGUI="no"


dnl STL
dnl AC_ARG_ENABLE(stl,
dnl    [ --enable-stl enables the use of C++ STL (default disabled)],
dnl    [ if test "${enable_stl}" = "yes"
dnl      then
        AC_DEFINE(USE_STL, [], [STL enabled])
dnl      fi ])

dnl debug flag
AC_ARG_ENABLE(autocall,
    [ --enable-autocall enables automatic calling for debug purposes (default disabled)],
    [ if test "${enable_autocall}" = "yes"
      then
        AC_DEFINE(MINISIP_AUTOCALL, [], [Debug autocalling])
      fi ])

dnl compile for iPAQ
AC_ARG_ENABLE(ipaq,
    [ --enable-ipaq enables various fixes for the iPAQ (default disabled)],
    [ if test "${enable_ipaq}" = "yes"
      then
        AC_DEFINE(IPAQ, [], [Compile for iPAQ])
      fi ])

dnl debug flag
AC_ARG_ENABLE(memdebug,
    [ --enable-memdebug enables memory debugging code. WARNING: This changes
     the MemObject API that is used by both libraries and applications. Use
     with care (default disabled)],
    [ if test "${enable_memdebug}" = "yes"
      then
        AC_DEFINE(MINISIP_MEMDEBUG, [], [Debug memory])
      fi ])

dnl debug flag
AC_ARG_ENABLE(debug,
    [ --enable-debug enables debug output (default disabled)],
    [ if test "${enable_debug}" = "yes"
      then
        AC_DEFINE(DEBUG_OUTPUT, [], [Debug output])
      else
      	AC_DEFINE(NDEBUG, [], [No debug output])
      fi ])

dnl debug flag
AC_ARG_ENABLE(tcpfriendly,
    [ --enable-tcpfriendly enables tcp friendly experiment code by David
     Tlahuetl and Erik Eliasson (default disabled)],
    [ if test "${enable_debug}" = "yes"
      then
        AC_DEFINE(TCP_FRIENDLY, [], [TCP Friendly])
	TCP_FRIENDLY="yes"
      else
      	AC_DEFINE(NTCP_FRIENDLY, [], [No TCP Friendly support])
	TCP_FRIENDLY="no"
      fi ])

AM_CONDITIONAL(TCP_FRIENDLY, test "${TCP_FRIENDLY}" = "yes")



dnl debug flag
AC_ARG_ENABLE(color-terminal,
    [ --enable-color-terminal enables color coding of some debug messages (default disabled)],
    [ if test "${enable_color_terminal}" = "yes"
      then
        AC_DEFINE(COLOR_TERMINAL, [], [Color terminal support])
      fi ])


dnl ipaq buzzer
AC_ARG_ENABLE(buzzer,
    [ --enable-buzzer enables IPAQ buzzer (default disabled)],
    [ if test "${enable_buzzer}" = "yes" -o "${enable_ipaq}" = "yes"
      then
        AC_DEFINE(IPAQ_BUZZER, [], [iPAQ buzzer support])
      else
      	AC_DEFINE(NBUZZER, [], [No iPAQ buzzer support])
      fi ])


dnl --with-avcodec argument
AC_ARG_WITH(avcodec,
    [ --with-avcodec=DIR location of the libavcodec tree],
    [ if test "${with_avcodec}" != "no"
      then
        AVCODEC_CPPFLAGS="-I${with_avcodec}"
        AVCODEC_LDFLAGS="-L${with_avcodec}"
      fi ])


dnl Check for libsamplerate
AC_CHECK_HEADER([samplerate.h], 
	[
		AC_CHECK_LIB([samplerate], [main],
			[LIBS="-lsamplerate ${LIBS}"
			 AC_DEFINE(FLOAT_RESAMPLER, [], [Use the advanced floating point based resampler])
			 FLOAT_RESAMPLER="yes"
                        ]
                )
	]
)
AM_CONDITIONAL(FLOAT_RESAMPLER, test "${FLOAT_RESAMPLER}" = "yes")


dnl Check for libspeex
AC_CHECK_HEADER([speex/speex.h], 
	[
		AC_CHECK_LIB([speex], [main],
			[LIBS="-lspeex ${LIBS}"
			 AC_DEFINE(HAS_SPEEX, [], [Use the Speex CODEC])])
			 HAS_SPEEX="yes"
	]
)
AM_CONDITIONAL(HAS_SPEEX, test "${HAS_SPEEX}" = "yes")

dnl Check for libgsm
AC_CHECK_HEADER([gsm.h], 
	[
		AC_CHECK_LIB([gsm], [main],
			[LIBS="-lgsm ${LIBS}"
			 AC_DEFINE(HAS_GSM, [], [Use the GSM CODEC])])
			 HAS_GSM="yes"
	]
)
AM_CONDITIONAL(HAS_GSM, test "${HAS_GSM}" = "yes")





dnl #############
dnl IPSEC SUPPORT
dnl #############
AC_ARG_ENABLE(ipsec,
    [ --enable-ipsec enables ipsec features (default disabled)],
    [	if test "${enable_ipsec}" = "yes"
        then
	  AC_CHECK_LIB([ipsec], [main], [], [AC_MSG_ERROR(
	    [Could not locate libipsec, required for ipsec features.])])
	  AC_DEFINE(IPSEC_SUPPORT, [], [Adding ipsec support])
        fi
    ]
)

AM_CONDITIONAL(IPSEC_SUPPORT, test "${enable_ipsec}" = "yes")

dnl P2T support
AC_ARG_ENABLE(p2t,
    [ --enable-p2t enables push-2-talk features (default disabled)],
    [  
      if test "${enable_p2t}" = "yes"
      then
	AC_DEFINE(P2T_SUPPORT, [], [Push-to-talk support])
      fi
    ]
)

AM_CONDITIONAL(P2T_SUPPORT, test "${enabled_p2t}" = "yes")

dnl AEC support
AC_ARG_ENABLE(aec,
    [ --enable-aec enables echo cancelling features (default enabled)],
    [ if test "${enable_aec}" != "no"
      then
	   AC_DEFINE(AEC_SUPPORT, [], [AEC support]) 
      fi])

AM_CONDITIONAL(AEC_SUPPORT, test "${enable_aec}" != "no")

dnl #############
dnl VIDEO SUPPORT
dnl #############

AC_ARG_ENABLE(sdl,
    [ --enable-sdl enables SDL video output if the required library is found])

AC_ARG_ENABLE(video,
    [ --enable-video enables video features (default disabled)],
    [ if test "${enable_video}" = "yes"
      then
dnl first check for libavcodec
        CPPFLAGS_SAVE="${CPPFLAGS}"
	LDFLAGS_SAVE="${LDFLAGS}"
	CPPFLAGS="${CPPFLAGS} ${AVCODEC_CPPFLAGS}"
	LDFLAGS="${LDFLAGS} ${AVCODEC_LDFLAGS}"
	
        AC_PATH_PROG(FFMPEG_CONFIG, ffmpeg-config, no)
        if test "${FFMPEG_CONFIG}" != "no"
        then
          CPPFLAGS="${CPPFLAGS} `ffmpeg-config --cflags`"
          LIBS="`ffmpeg-config --libs avcodec` ${LIBS}"
        else
	  AC_CHECK_HEADER([avcodec.h], [],
            [
              AC_MSG_ERROR(
	      [Could not locate avcodec header files. Please install 
               the libavcodec-devel package.]
              )
            ]
          )
            
	  AC_CHECK_LIB([avcodec], [main], [], [AC_MSG_ERROR(
	    [Could not locate libavcodec, required for video features.])])

	  AC_CHECK_LIB([z], [main], [], [AC_MSG_ERROR(
	    [Could not find zlib, required by libavcodec.])])
        fi
dnl	CPPFLAGS="${CPPFLAGS_SAVE}"
dnl	LDFLAGS="${LDFLAGS_SAVE}"
        
dnl look for V4L headers
        AC_CHECK_HEADERS([linux/videodev.h],
          [VIDEO_GRABBER="${VIDEO_GRABBER} v4l"
	   V4L_VIDEO_GRABBER="yes" ])
dnl look for libdc1394
        AC_CHECK_HEADERS([libdc1394/dc1394_control.h],
	  [
	   AC_CHECK_LIB([raw1394],[main], [
	     AC_CHECK_LIB([dc1394_control],[main], [
	       VIDEO_GRABBER="${VIDEO_GRABBER} dc1394}"
	       DC1394_VIDEO_GRABBER="yes" 
	       AC_DEFINE(DC1394_SUPPORT, [], [Compile DC1394 support])
	       LIBS="${LIBS} -ldc1394_control -lraw1394"
	       ], [], [-lraw1394])
	       ])
	     ])
dnl look for xvideo
	AC_PATH_XTRA
	X_LIBS="$X_LIBS -lX11"
	LDFLAGS="${LDFLAGS} ${X_EXTRA_LIBS} ${X_LIBS}"
	AC_CHECK_LIB([Xv], [main])
        AC_CHECK_HEADERS([X11/extensions/Xv.h],[
          AC_CHECK_HEADERS([X11/extensions/Xvlib.h],
	    [VIDEO_DISPLAY="${VIDEO_DISPLAY} xv"
	     AC_DEFINE(XV_SUPPORT, [], [Compile Xvideo support])
	     XV_VIDEO_DISPLAY="yes" ],[])
	  ])
dnl check for SDL
        AC_CHECK_HEADERS([SDL/SDL.h],
	  [
	   AC_CHECK_LIB([SDL],[main], [
               if test "x${enable_sdl}" != "xno"
               then
	         VIDEO_DISPLAY="${VIDEO_DISPLAY} sdl"
	         SDL_VIDEO_DISPLAY="yes" 
	         AC_DEFINE(SDL_SUPPORT, [], [Compile SDL support])
	         LIBS="${LIBS} -lSDL ${X_EXTRA_LIBS} -lXext ${X_LIBS} -lpthread"
	         CPPFLAGS="${CPPFLAGS} ${X_CFLAGS}"
               fi
	     ], [], [${X_EXTRA_LIBS} -lXext ${X_LIBS} -lpthread] )
	  ])
	
	
	if test "x${VIDEO_DISPLAY}" != "x"
	then
	  AC_DEFINE(VIDEO_SUPPORT, [], [Compile video features])
	  
	  AC_SUBST(AVCODEC_LDFLAGS)
	  AC_SUBST(AVCODEC_CPPFLAGS)
	fi
      fi ])

AM_CONDITIONAL(VIDEO_SUPPORT, test "${enable_video}" = "yes")
AM_CONDITIONAL(V4L_VIDEO_GRABBER, test "${V4L_VIDEO_GRABBER}" = "yes")
AM_CONDITIONAL(DC1394_VIDEO_GRABBER, test "${DC1394_VIDEO_GRABBER}" = "yes")
AM_CONDITIONAL(XV_VIDEO_DISPLAY, test "${XV_VIDEO_DISPLAY}" = "yes")
AM_CONDITIONAL(SDL_VIDEO_DISPLAY, test "${SDL_VIDEO_DISPLAY}" = "yes")



      

dnl ALSA sound interface
AC_ARG_ENABLE(alsa,
    [ --enable-alsa enables ALSA sound support (default disabled)],
    [ if test "${enable_alsa}" = "yes"
      then
        AC_CHECK_LIB([asound], [main],,
           [AC_MSG_ERROR([Could not find the alsa library])]
        )
      fi ])

AM_CONDITIONAL(HAVE_LIBASOUND, [test "${enable_alsa}" = "yes"])

dnl DirectSound sound interface
AC_ARG_ENABLE(dsound,
    [ --enable-dsound enables DirectSound sound support (default disabled)],
    [ AC_DEFINE(DSOUND, [], [Use DirectSound sound driver]) ])

AM_CONDITIONAL(DIRECT_SOUND, [test "${enable_dsound}" = "yes"])

dnl TEXT UI
AC_ARG_ENABLE(textui,
    [ --enable-textui enables the text based user interface (default disabled)],
    [ if test "${enable_textui}" = "yes"
      then
	  AC_DEFINE(TEXT_UI, [], [Use the text based user interface])
          QTGUI="no"
	  enable_gtk="no"
	  
      fi ])
AM_CONDITIONAL(TEXT_UI, [test "${enable_textui}" = "yes"])

dnl Qt GUI
AC_ARG_ENABLE(qtui,
    [ --enable-qtui enables the Qt based user interface (default disabled)],
    [ if test "${enable_qtui}" = "yes"
      then
	  AC_DEFINE(QT_GUI, [], [Use the Qt based user interface])
          QTGUI="yes"
	  enable_gtk="no"
	  
      fi ])
AM_CONDITIONAL(QT_GUI, [test "${enable_qtui}" = "yes"])


dnl GTK GUI
AC_ARG_ENABLE(gtk,
    [ --disable-gtk disables the GTK interface (default enabled).])

if test "x${enable_gtk}" != "xno"
  then
    PKG_CHECK_MODULES(GLADEMM, [libglademm-2.4], 
      [AC_DEFINE(GTK_GUI, [], [Use the GTK GUI])],
      [PKG_CHECK_MODULES(GLADEMM, [libglademm-2.0], 
        [AC_DEFINE(GTK_GUI, [], [Use the GTK GUI])
	 AC_DEFINE(OLDLIBGLADEMM, [], [Use libglademm-2.0])],
        [AC_MSG_ERROR([Could not find the glademm-2.0 library])]
      )]
    )
    QTGUI="no"
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

AC_ARG_ENABLE(gconf,
    [ --enable-gconf enables support for GConf (default enabled).])

PKG_CHECK_MODULES(GCONF, [gconf-2.0],
	[
		if test "x${enable_gconf}" != "xno"
		then
			AC_DEFINE(GCONF_SUPPORT, [], [Support gconf.])
			gconf_support="yes"
		fi
	],[echo 'GConf development libraries not found, GConf support is disabled.'])
AM_CONDITIONAL(GCONF_SUPPORT, [test "x${gconf_support}" = "xyes"])








QT="qt"

dnl QT embedded
AC_ARG_ENABLE(qte,
    [ --enable-qte enables Qt Embedded support (default disabled)],
    [ if test "${enable_qte}" = "yes"
      then
      	QT="qte"
	CXXFLAGS="${CXXFLAGS} -fno-rtti"
	AC_DEFINE(QWS, [], [Use Qt Embedded windowing system])
	dnl For some reason the openssl libs I cross-compiled require this
	LDFLAGS="${LDFLAGS} -ldl"
	
      fi ])

dnl Damn RedHat
AC_DEFINE(OPENSSL_NO_KRB5, [], [No Kerberos in OpenSSL])


if test "${QTGUI}" = "yes"
then

dnl Set the QTDIR directory
AC_ARG_WITH(qtdir,
    [ --with-qtdir=DIR QT installation directory (default /usr)],
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

dnl Check for libssl 0.9.7
AC_CHECK_HEADERS([openssl/aes.h],[],[])

dnl
dnl libmsip
dnl
AC_CHECK_HEADER(libmsip/SipStack.h,,[AC_MSG_ERROR(["You need the libmsip
        headers/library. Try installing the libmsip-devel package for your
        distribution."])])
AC_CHECK_LIB([msip], [main],
        [
                LIBS="-lmsip ${LIBS}"
        ],[
                AC_MSG_ERROR([Could not find libmsip. Please install the corresponding package.])
        ])

dnl
dnl libmikey
dnl
AC_CHECK_HEADER(libmikey/MikeyMessage.h,,[AC_MSG_ERROR(["You need the libmikey
        headers/library. Try installing the libmikey-devel package for your
        distribution."])])
AC_CHECK_LIB([mikey], [main],
        [
                LIBS="-lmikey ${LIBS}"
        ],[
                AC_MSG_ERROR([Could not find libmikey. Please install the corresponding package.])
        ])

dnl Checks for libraries.
AC_CHECK_LIB([gdi32], [main])
AC_CHECK_LIB([wsock32], [main])

dnl FIXME: Replace `main' with a function in `-lX11':
AC_CHECK_LIB([X11], [main])
dnl FIXME: Replace `main' with a function in `-lXext':
AC_CHECK_LIB([Xext], [main])
dnl FIXME: Replace `main' with a function in `-lm':
AC_CHECK_LIB([m], [main])
dnl FIXME: Replace `main' with a function in `-lnsl':
AC_CHECK_LIB([nsl], [main])

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
    [ --enable-opie enables Open Palmtop Integrated Environment support (default disabled)],
    [ if test "${enable_opie}" = "yes"
      then
	LDFLAGS="${LDFLAGS} -L${QT_LIB_LOCATION} -L${OPIEDIR}/lib"
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
AC_SUBST(AM_CPPFLAGS)

AC_SUBST(datadir)

AC_OUTPUT([codecs/Makefile
                 codecs/ilbc/Makefile
		 codecs/g711/Makefile
                 minisip/Makefile
                 minisip/contactdb/Makefile
                 minisip/confbackend/Makefile
                 minisip/ipprovider/Makefile
                 minisip/gui/Makefile
                 minisip/gui/qtgui/Makefile
                 minisip/gui/gtkgui/Makefile
                 minisip/gui/textui/Makefile
                 rtp/Makefile
                 rtp/ratecontrol/Makefile
                 sip/Makefile
                 soundcard/Makefile
                 spaudio/Makefile
                 stun/Makefile
		 ipsec/Makefile
                 sdp/Makefile
                 p2t/Makefile
		 mediahandler/Makefile
                 video/Makefile
		 conf/Makefile
		 aec/Makefile
		 debian/Makefile
		 Makefile])

