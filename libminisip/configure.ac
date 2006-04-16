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
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_LIBTOOL_WIN32_DLL
AC_PROG_LIBTOOL
AC_PROG_CPP
AC_PROG_CXX
PKG_PROG_PKG_CONFIG

AC_C_BIGENDIAN

AC_LANG(C++)

if test x$os_win = xyes; then
    AC_CHECK_TOOL(WINDRES, [windres])
    if test x$WINDRES = x; then
        AC_MSG_ERROR([Could not find windres in your PATH.])
    fi
fi

dnl
dnl Linker flags
dnl mingw needs --enable-runtime-pseudo-reloc since it can't
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
			[SAMPLERATE_LIBS="-lsamplerate"
			 AC_DEFINE(FLOAT_RESAMPLER, [], [Use the advanced floating point based resampler])
			 FLOAT_RESAMPLER="yes"
                        ]
                )
	]
)
AM_CONDITIONAL(FLOAT_RESAMPLER, test "${FLOAT_RESAMPLER}" = "yes")
AC_SUBST(SAMPLERATE_LIBS)

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
		AC_CHECK_LIB([speex], [speex_encoder_init],
			[SPEEX_LIBS="-lspeex"
			 AC_DEFINE(HAS_SPEEX, [], [Use the Speex CODEC])])
			 HAS_SPEEX="yes"
	]
)
AM_CONDITIONAL(HAS_SPEEX, test "${HAS_SPEEX}" = "yes")
AC_SUBST(SPEEX_LIBS)

dnl Check for libgsm
AC_CHECK_HEADER([gsm.h], 
	[
		AC_CHECK_LIB([gsm], [gsm_encode],
			[GSM_LIBS="-lgsm"
			 AC_DEFINE(HAS_GSM, [], [Use the GSM CODEC])])
			 HAS_GSM="yes"
	]
)
AM_CONDITIONAL(HAS_GSM, test "${HAS_GSM}" = "yes")
AC_SUBST(GSM_LIBS)

dnl
dnl PortAudio V19 support
dnl
AC_ARG_ENABLE(portaudio,
	[AS_HELP_STRING([--enable-portaudio],
		[enable PortAudio V19 support (default auto).])],,
	[enable_portaudio=auto])

if test ! x$enable_portaudio = xno; then
   have_portaudio=yes
   AC_CHECK_HEADERS([portaudio.h],,have_portaudio=no)
   AC_CHECK_LIB([portaudio],[Pa_GetDefaultOutputDevice],[PORTAUDIO_LIBS=-lportaudio],have_portaudio=no)

   if test x$have_portaudio = xyes; then
      AC_DEFINE(PORTAUDIO_SUPPORT, [1], [PortAudio V19 SoundDevice support.])
   elif test x$enable_portaudio = xyes; then
      AC_MSG_ERROR([Could not find PortAudio requirement(s).])
   fi

   AC_SUBST(PORTAUDIO_LIBS)
fi
AM_CONDITIONAL([PORTAUDIO_SUPPORT], [test x$have_portaudio = xyes])


dnl #############
dnl IPSEC SUPPORT
dnl #############
AC_ARG_ENABLE(ipsec-enable,
    [AS_HELP_STRING([--enable-ipsec-enable],
    	[enable ipsec features (default disabled).])],
    [	if "${enable_ipsec}" = "yes"
        then
	    AC_CHECK_LIB([ipsec], [main], [], [AC_MSG_ERROR(
	      [Could not locate libipsec, required for ipsec features.])])
	    AC_DEFINE(IPSEC_SUPPORT, [], [Adding ipsec support])
        fi ])
AM_CONDITIONAL(IPSEC_SUPPORT, test "${enable_ipsec}" = "yes")

dnl P2T support
AC_ARG_ENABLE(p2t,
    [AS_HELP_STRING([--enable-p2t],
    	[enable push-2-talk features (default disabled)])],
    [  if test "${enable_p2t}" = "yes"
       then
	   AC_DEFINE(P2T_SUPPORT, [], [P2T support])
       fi ])
AM_CONDITIONAL(P2T_SUPPORT, test "${enabled_p2t}" = "yes")

dnl AEC support
AC_ARG_ENABLE(aec,
    [AS_HELP_STRING([--enable-aec],
    	[enables Auto Echo Cancellation features (default disabled)])],
    [  if test "${enable_aec}" = "yes"
       then
	   AC_DEFINE(AEC_SUPPORT, [], [AEC support])
       fi ])
AM_CONDITIONAL(AEC_SUPPORT, test "${enabled_aec}" = "yes")

AC_ARG_ENABLE(gconf,
    [AS_HELP_STRING([--enable-gconf], 
    	[enables support for GConf (default disabled).])]
   )

if test "x${enable_gconf}" != "xno"; then
   gconf_support="yes"
   PKG_CHECK_MODULES(GOBJECT, [gobject-2.0],, [gconf_support="no"])

   PKG_CHECK_MODULES(GCONF, [gconf-2.0],, [gconf_support="no"])

   if test "x${gconf_support}" = "xyes"; then
      AC_DEFINE(GCONF_SUPPORT, [], [Support gconf.])
   elif test "x${enable_gconf}" = "xyes"; then
      AC_MSG_ERROR([GConf development libraries not found.])
   else
      AC_MSG_NOTICE([GConf development libraries not found, GConf support is disabled.])
   fi
fi
AM_CONDITIONAL(GCONF_SUPPORT, [test "x${gconf_support}" = "xyes"])


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
        LIBS_SAVE="${LIBS}"

        AC_PATH_PROG(FFMPEG_CONFIG, ffmpeg-config, no)
        if test "${FFMPEG_CONFIG}" != "no"
        then
          FFMPEG_CFLAGS="`ffmpeg-config --cflags`"
          FFMPEG_LIBS="`ffmpeg-config --libs avcodec`"
	else
	  FFMPEG_CFLAGS=
	  FFMPEG_LIBS="-lavcodec -lz"
	fi
        
	LIBS="${FFMPEG_LIBS} ${LIBS}"
	CPPFLAGS="${CPPFLAGS} ${FFMPEG_CFLAGS} ${AVCODEC_CPPFLAGS}"
	LDFLAGS="${LDFLAGS} ${AVCODEC_LDFLAGS}"

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

	CPPFLAGS="${CPPFLAGS_SAVE}"
	LDFLAGS="${LDFLAGS_SAVE}"
	LIBS="${LIBS_SAVE}"
        
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
	       DC1394_LIBS="-ldc1394_control -lraw1394"
	       ], [], [-lraw1394])
	       ])
               AC_SUBST(DC1394_LIBS)
	     ])
dnl look for xvideo
	CPPFLAGS_SAVE="${CPPFLAGS}"
	LDFLAGS_SAVE="${LDFLAGS}"
	LIBS_SAVE="${LIBS}"

	AC_PATH_XTRA
	X_LIBS="$X_LIBS -lX11"
	LDFLAGS="${LDFLAGS} ${X_EXTRA_LIBS} ${X_LIBS}"
	have_xv="yes"
	AC_CHECK_LIB([Xv], [main],,[have_xv="no"])
	AC_CHECK_HEADERS([X11/Xlib.h],,[have_xv="no"])
        AC_CHECK_HEADERS([X11/extensions/Xv.h],[
          AC_CHECK_HEADERS([X11/extensions/Xvlib.h],
	    [],[have_xv="no"],
[
#if HAVE_X11_XLIB_H
# include<X11/Xlib.h>
#endif
])
	  ],[have_xv="no"])

	if test "${have_xv}" = "yes"; then
	  X_LIBS="-lXv $X_LIBS"
	  VIDEO_DISPLAY="${VIDEO_DISPLAY} xv"
	  AC_DEFINE(XV_SUPPORT, [], [Compile Xvideo support])
	  XV_VIDEO_DISPLAY="yes" 
	fi

dnl check for SDL
        AC_CHECK_HEADERS([SDL/SDL.h],
	  [
	   AC_CHECK_LIB([SDL],[main], [
               if test "x${enable_sdl}" != "xno"
               then
	         VIDEO_DISPLAY="${VIDEO_DISPLAY} sdl"
	         SDL_VIDEO_DISPLAY="yes" 
	         AC_DEFINE(SDL_SUPPORT, [], [Compile SDL support])
                 X_EXTRA_LIBS="${X_EXTRA_LIBS} -lXext"
		 SDL_LIBS="-lSDL -pthread"
	         CPPFLAGS="${CPPFLAGS} ${X_CFLAGS}"

		 AC_SUBST(SDL_LIBS)
               fi
	     ], [], [${X_EXTRA_LIBS} -lXext ${X_LIBS} -lpthread] )
	  ])
	
	CPPFLAGS="${CPPFLAGS_SAVE}"
	LDFLAGS="${LDFLAGS_SAVE}"
	LIBS="${LIBS_SAVE}"
	
	if test "x${VIDEO_DISPLAY}" != "x"
	then
	  AC_DEFINE(VIDEO_SUPPORT, [], [Compile video features])
	  
	  AC_SUBST(FFMPEG_LIBS)
	  AC_SUBST(FFMPEG_CFLAGS)
	  AC_SUBST(AVCODEC_LDFLAGS)
	  AC_SUBST(AVCODEC_CPPFLAGS)
	fi
      fi ])

AC_SUBST(X_LIBS)
AC_SUBST(X_CFLAGS)
AC_SUBST(X_EXTRA_LIBS)

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
        AC_CHECK_LIB([asound], [main],
	   [ALSA_LIBS="-lasound"
	    AC_DEFINE(HAVE_LIBASOUND, [1], [Define to 1 if you have the `asound' library (-lasound).])],
           [AC_MSG_ERROR([Could not find the alsa library])]
        )
      fi ])

AM_CONDITIONAL(HAVE_LIBASOUND, [test "${enable_alsa}" = "yes"])
AC_SUBST(ALSA_LIBS)

dnl DirectSound sound interface
AC_ARG_ENABLE(dsound,
    [AS_HELP_STRING([--enable-dsound], 
    	[enables DirectSound sound support (default disabled)])],
    [ if test "${enable_dsound}" = "yes"
      then
        AC_DEFINE(DSOUND, [], [Use DirectSound sound driver]) 
      fi ])

if test ! x$enable_dsound = xno; then
   have_dsound=yes
   save_LIBS="${LIBS}"
   dnl Can't check stdcall function DirectSoundFullDuplexCreate.
   AC_CHECK_LIB([dsound], [main],,[have_dsound=no])
   AC_CHECK_LIB([uuid], [GUID_NULL],,[have_dsound=no])
   AC_CHECK_LIB([dxguid], [IID_IDirectSoundNotify],,[have_dsound=no])
   AC_CHECK_HEADERS([dsound.h],,[have_dsound=no])
   if test x$have_dsound = xyes; then
      DSOUND_LIBS="-ldxguid -luuid -ldsound"
   elif test x$enable_dsound = xyes; then
      AC_MSG_ERROR([Could not find libraries required by DirectSound support])
   fi
   LIBS="${save_LIBS}"
   AC_SUBST(DSOUND_LIBS)
fi
AM_CONDITIONAL(DIRECT_SOUND, [test "${have_dsound}" = "yes"])


PKG_CHECK_MODULES(OSSO, [libosso],
	[
	 	AC_DEFINE(OSSO_SUPPORT, [], [Support the libosso library])
		osso_support="yes"
	],[echo 'No libosso support.'])

AM_CONDITIONAL(OSSO_SUPPORT, [test "${osso_support}" = "yes"])



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
AC_CHECK_LIB([wsock32], [main])

dnl FIXME: Replace `main' with a function in `-lm':
AC_CHECK_LIB([m], [main])
dnl FIXME: Replace `main' with a function in `-lnsl':
AC_CHECK_LIB([nsl], [main])


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
AC_SUBST(AM_CXXFLAGS)

AC_SUBST(datadir)

dnl it shoudl be like this ... but for now, reduce it ...AC_CONFIG_FILES([Makefile include/Makefile debian/Makefile win32/Makefile libmsip.pc win32/libmsip-res.rc])
AC_CONFIG_FILES([libminisip.pc
		Makefile
		include/Makefile
		source/win32/libminisip-res.rc])

AC_OUTPUT
