dnl                                               -*- Autoconf -*-
dnl Process this file with autoconf to produce a configure script.

dnl AC_PREREQ(2.57)

m4_define([minisiplib_major_version], [0])
m4_define([minisiplib_minor_version], [3])
m4_define([minisiplib_micro_version], [1])
m4_define([minisiplib_version],
          [minisiplib_major_version.minisiplib_minor_version.minisiplib_micro_version])

dnl
dnl Libtool style version info
dnl update before each new release 
dnl See node "Updating version info::" in info libtool
dnl
m4_define([lt_current], [0])
m4_define([lt_revision], [0])
m4_define([lt_age], [0])

AC_INIT(libminisip, [minisiplib_major_version.minisiplib_minor_version],
        eliasson@imit.kth.se)
dnl AC_CONFIG_HEADER([config.h])
dnl AM_CONFIG_HEADER([config.h])

AM_INIT_AUTOMAKE(libminisip, [minisiplib_version])

AM_CONFIG_HEADER(include/compilation_config.h)

MINISIPLIB_MAJOR_VERSION=minisiplib_major_version
MINISIPLIB_MINOR_VERSION=minisiplib_minor_version
MINISIPLIB_MICRO_VERSION=minisiplib_micro_version
LT_VERSION_INFO="lt_current:lt_revision:lt_age"
LT_CURRENT_MINUS_AGE=m4_eval(lt_current - lt_age)
AC_SUBST(MINISIPLIB_MAJOR_VERSION)
AC_SUBST(MINISIPLIB_MINOR_VERSION)
AC_SUBST(MINISIPLIB_MICRO_VERSION)
AC_SUBST(LT_VERSION_INFO)
AC_SUBST(LT_CURRENT_MINUS_AGE)

AC_CANONICAL_HOST
case $host_os in
     *mingw* )
	     os_win=yes
	     ;;
esac

AM_CONDITIONAL(OS_WIN, test x$os_win = xyes)

dnl Checks for programs.
dnl AC_PROG_LEX
dnl AM_PROG_LEX
AC_PROG_LN_S
AC_PROG_MAKE_SET
dnl AC_PROG_YACC
dnl AC_PROG_RANLIB
AC_PROG_LIBTOOL
dnl AC_PROG_CC
AC_PROG_CPP
AC_PROG_CXX

AC_C_BIGENDIAN

AC_LANG(C++)

if test x$os_win = xyes; then
    AC_CHECK_TOOL(WINDRES, [windres])
    if test x$WINDRES = x; then
        AC_MSG_ERROR([Could not find windres in your PATH.])
    fi
fi

dnl STL
dnl For now STL is made mandatory

dnl AC_ARG_ENABLE(stl,
dnl     [ --enable-stl enables the use of C++ STL (default enabled)],
dnl     [ if test "${enable_stl}" = "yes"
dnl       then
        AC_DEFINE(USE_STL, [], [STL enabled])
dnl       fi ])


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

dnl Check for stack trace support
AC_CHECK_HEADER([execinfo.h], 
	[
		AC_DEFINE(HAS_EXECINFO, [], [Stack trace functionality found])
			HAS_EXECINFO="yes"
	]
)
AM_CONDITIONAL(HAS_EXECINFO, test "${HAS_EXECINFO}" = "yes")

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

dnl #############
dnl IPSEC SUPPORT
dnl #############
ipsec_enable=no
AC_ARG_ENABLE(ipsec-enable,
    [ --enable-ipsec-enable enables ipsec features (default disabled)],
    [	ipsec_enable=yes
	AC_CHECK_LIB([ipsec], [main], [], [AC_MSG_ERROR(
	  [Could not locate libipsec, required for ipsec features.])])
	AC_DEFINE(IPSEC_SUPPORT, [], [Adding ipsec support])])
AM_CONDITIONAL(IPSEC_SUPPORT, test "${ipsec_enable}" = "yes")

dnl P2T support
p2t_enabled=no
AC_ARG_ENABLE(p2t,
    [ --enable-ipsec-enable enables push-2-talk features (default enabled)],
    [  p2t_enable=yes
	   AC_DEFINE(P2T_SUPPORT, [], [P2T support])])
AM_CONDITIONAL(P2T_SUPPORT, test "${p2t_enabled}" = "yes")

dnl AEC support
aec_enabled=no
AC_ARG_ENABLE(aec,
    [ --enable-aec enables push-2-talk features (default enabled)],
    [  aec_enable=yes
	   AC_DEFINE(AEC_SUPPORT, [], [AEC support])])
AM_CONDITIONAL(AEC_SUPPORT, test "${aec_enabled}" = "yes")

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


dnl Damn RedHat
AC_DEFINE(OPENSSL_NO_KRB5, [], [No Kerberos in OpenSSL])

dnl Check for libssl 0.9.7
AC_CHECK_HEADERS([openssl/aes.h],[],[])

dnl
dnl libmutil
dnl
PKG_CHECK_MODULES(MUTIL, [libmutil >= 0.3.1])
AM_CXXFLAGS="${AM_CXXFLAGS} ${MUTIL_CFLAGS}"

save_CPPFLAGS="${CPPFLAGS}"
save_LIBS="${LIBS}"
CPPFLAGS="${CPPFLAGS} ${MUTIL_CFLAGS}"
LIBS="${MUTIL_LIBS} ${LIBS}"

AC_CHECK_HEADER(libmutil/itoa.h,,[AC_MSG_ERROR(["You need the libmutil
	headers/library. Try installing the libmutil-devel package for your
	distribution."])])
AC_CHECK_LIB([mutil], [main],
	[
		LIBS="-lmutil ${LIBS}"
	],[
		AC_MSG_ERROR([Could not find libmutil. Please install the corresponding package.])
	])


dnl
dnl libmnetutil
dnl
PKG_CHECK_MODULES(MNETUTIL, [libmnetutil >= 0.3.1])
AM_CXXFLAGS="${AM_CXXFLAGS} ${MNETUTIL_CFLAGS}"

CPPFLAGS="${CPPFLAGS} ${MNETUTIL_CFLAGS}"
LIBS="${MNETUTIL_LIBS} ${LIBS}"

AC_CHECK_HEADER(libmnetutil/NetUtil.h,,[AC_MSG_ERROR(["You need the libmnetutil
        headers/library. Try installing the libmnetutil-devel package for your
        distribution."])])
AC_CHECK_LIB([mnetutil], [main],
        [
                LIBS="-lmnetutil ${LIBS}"
        ],[
                AC_MSG_ERROR([Could not find libmnetutil. Please install the corresponding package.])
        ])


dnl
dnl libmsip
dnl
PKG_CHECK_MODULES(MSIP, [libmsip >= 0.3.1])
AM_CXXFLAGS="${AM_CXXFLAGS} ${MSIP_CFLAGS}"

CPPFLAGS="${CPPFLAGS} ${MSIP_CFLAGS}"
LIBS="${MSIP_LIBS} ${LIBS}"

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
PKG_CHECK_MODULES(MIKEY, [libmikey >= 0.4.1])
AM_CXXFLAGS="${AM_CXXFLAGS} ${MIKEY_CFLAGS}"

CPPFLAGS="${CPPFLAGS} ${MIKEY_CFLAGS}"
LIBS="${MIKEY_LIBS} ${LIBS}"

AC_CHECK_HEADER(libmikey/MikeyMessage.h,,[AC_MSG_ERROR(["You need the libmikey
        headers/library. Try installing the libmikey-devel package for your
        distribution."])])
AC_CHECK_LIB([mikey], [main],
        [
                LIBS="-lmikey ${LIBS}"
        ],[
                AC_MSG_ERROR([Could not find libmikey. Please install the corresponding package.])
        ])

CPPFLAGS="${save_CPPFLAGS}"
LIBS="${save_LIBS}"

dnl Checks for libraries.
AC_CHECK_LIB([gdi32], [main])
AC_CHECK_LIB([wsock32], [main])
AC_CHECK_LIB([dsound], [main])
AC_CHECK_LIB([uuid], [main])
AC_CHECK_LIB([dxguid], [main])

dnl FIXME: Replace `main' with a function in `-lX11':
AC_CHECK_LIB([X11], [main])
dnl FIXME: Replace `main' with a function in `-lXext':
AC_CHECK_LIB([Xext], [main])
dnl FIXME: Replace `main' with a function in `-lm':
AC_CHECK_LIB([m], [main])
dnl FIXME: Replace `main' with a function in `-lnsl':
AC_CHECK_LIB([nsl], [main])

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([malloc.h stdlib.h string.h unistd.h])

dnl Checks for typedefs, structures, and compiler characteristics.
dnl AC_HEADER_STDBOOL
AC_C_CONST
AC_HEADER_TIME
dnl AC_C_VOLATILE

dnl Checks for library functions.
dnl AC_FUNC_ERROR_AT_LINE
dnl AC_PROG_GCC_TRADITIONAL
dnl AC_FUNC_MALLOC
dnl AC_TYPE_SIGNAL
dnl AC_CHECK_FUNCS([bzero gettimeofday inet_ntoa memmove memset pow socket sqrt strtol])

dnl AC_SUBST(CPPFLAGS)
dnl AC_SUBST(LDFLAGS)

dnl it shoudl be like this ... but for now, reduce it ...AC_CONFIG_FILES([Makefile include/Makefile debian/Makefile win32/Makefile libmsip.pc win32/libmsip-res.rc])
AC_CONFIG_FILES([Makefile include/Makefile libminisip.pc])
AC_OUTPUT