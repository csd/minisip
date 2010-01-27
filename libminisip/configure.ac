dnl                                               -*- Autoconf -*-
dnl Process this file with autoconf to produce a configure script.

dnl AC_PREREQ(2.57)

m4_MINISIP_PACKAGE_VERSION(libminisip, minisip, 0, 8, 0)
m4_MINISIP_PACKAGE_CONTACT([Erik Eliasson], [eliasson@imit.kth.se])
dnl m4_MINISIP_PACKAGE_RELEASED
m4_MINISIP_LIBRARY_VERSION(0, 0, 0)

AC_INIT(MINISIP_PACKAGE_NAME, MINISIP_PACKAGE_VERSION, MINISIP_PACKAGE_CONTACT)
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_MACRO_DIR(m4)
AM_MINISIP_PACKAGE_INIT
AM_CONFIG_HEADER(include/compilation_config.h)

AM_MINISIP_LIBTOOL_EXTRAS

AC_PROG_LIBTOOL




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

dnl ipaq buzzer
AC_ARG_ENABLE(buzzer,
    AS_HELP_STRING([--enable-buzzer],
    	[enables IPAQ buzzer (default disabled)]),
    [ if test "${enable_buzzer}" = "yes" -o "${enable_ipaq}" = "yes"
      then
        AC_DEFINE(IPAQ_BUZZER, [], [iPAQ buzzer support])
      else
      	AC_DEFINE(NBUZZER, [], [No iPAQ buzzer support])
      fi ])

dnl OSS support
AC_ARG_ENABLE(oss,
    AS_HELP_STRING([--disable-oss],
    	[disables OSS sound IO support (default enabled)]),
)
if test "${enable_oss}" = "no" -o "${os_win}" = "yes"
then
      have_oss=no
else
      have_oss=yes
fi

if test "${have_oss}" = "no"
then
      AC_DEFINE(NO_OSS, [], [No OSS support])
else
      AC_DEFINE(ENABLE_OSS, [], [OSS support])
fi

AM_CONDITIONAL(ENABLE_OSS, [test "${have_oss}" != "no"])



dnl --with-avcodec argument
AC_ARG_WITH(avcodec,
    AS_HELP_STRING([--with-avcodec=DIR],[location of the libavcodec tree]),
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

dnl MSRP support
AC_ARG_ENABLE(msrp,
    AS_HELP_STRING([--enable-msrp],
		   [enable MSRP file transfer protocol (default disabled)]),
    [  if test "${enable_msrp}" = "yes"
       then
	   AC_DEFINE(MSRP_SUPPORT, [], [MSRP support])
       fi ])
AM_CONDITIONAL(MSRP_SUPPORT, test "${enable_msrp}" = "yes")


dnl Snake web services support
AC_ARG_ENABLE(snake,
    AS_HELP_STRING([--enable-snake],
		   [enable Snake web services support (default disabled)]),
    [  if test "${enable_snake}" = "yes"
       then
	   AC_DEFINE(SNAKE_SUPPORT, [], [SNAKE support])
	   CPPFLAGS="${CPPFLAGS} -I${AXIS2C_HOME}/include/axis2-1.6.0/"
	   LDFLAGS="${LDFLAGS} -L${AXIS2C_HOME}/lib/ -laxutil -laxis2_engine -lneethi -laxis2_http_common -laxis2_axiom -laxis2_parser -lguththila -laxis2_minizip"
       fi ])
AM_CONDITIONAL(SNAKE_SUPPORT, test "${enable_snake}" = "yes")

dnl
dnl PortAudio V19 support
dnl
AC_ARG_ENABLE(portaudio,
	AS_HELP_STRING([--enable-portaudio],
		[enable PortAudio V19 support (default auto).]),,
	[enable_portaudio=auto])

if test ! x$enable_portaudio = xno; then
   have_portaudio=yes
   PKG_CHECK_MODULES(PORTAUDIO,[portaudio-2.0],[
      CPPFLAGS_SAVE="${CPPFLAGS}"
      CPPFLAGS="${CPPFLAGS} ${PORTAUDIO_CFLAGS}"
      AC_CHECK_HEADERS([portaudio.h],[
         LIBS_SAVE="${LIBS}"
         LIBS="${PORTAUDIO_LIBS} ${LIBS}"
         AC_SEARCH_LIBS([Pa_GetDefaultOutputDevice],[],[],[have_portaudio=no])
         LIBS="${LIBS_SAVE}"
      ],[have_portaudio=no])
      CPPFLAGS="${CPPFLAGS_SAVE}"
   ],[have_portaudio=no])

   if test x$have_portaudio = xyes; then
      AC_DEFINE(PORTAUDIO_SUPPORT, [1], [PortAudio V19 SoundDevice support.])
   elif test x$enable_portaudio = xyes; then
      AC_MSG_ERROR([Could not find PortAudio requirement(s).])
   fi
fi
AM_CONDITIONAL([PORTAUDIO_SUPPORT], [test x$have_portaudio = xyes])

dnl P2T support
AC_ARG_ENABLE(p2t,
    AS_HELP_STRING([--enable-p2t],
    	[enable push-2-talk features (default disabled)]),
    [  if test "${enable_p2t}" = "yes"
       then
	   AC_DEFINE(P2T_SUPPORT, [], [P2T support])
       fi ])
AM_CONDITIONAL(P2T_SUPPORT, test "${enable_p2t}" = "yes")

dnl ZRTP support
AC_ARG_ENABLE(zrtp,
    AS_HELP_STRING([--enable-zrtp],
    	[enable ZRTP support (default disabled)]))
if test "${enable_zrtp}" = "yes"
then
   AC_DEFINE(ZRTP_SUPPORT, [], [ZRTP support])
fi
AM_CONDITIONAL(ZRTP_SUPPORT, test "${enable_zrtp}" = "yes")

dnl G-D HW authentication token support
AC_ARG_ENABLE(scsim,
    AS_HELP_STRING([--enable-scsim],
    	[enable G-D HW authentication token (SmartCard) support (default disabled)]))
if test "${enable_scsim}" = "yes"
then
   AC_DEFINE(SCSIM_SUPPORT, [], [SCSIM support])
fi
AM_CONDITIONAL(SCSIM_SUPPORT, test "${enable_scsim}" = "yes")




dnl AEC support
AC_ARG_ENABLE(aec,
    AS_HELP_STRING([--enable-aec],
    	[enables Auto Echo Cancellation features (default disabled)]),
    [  if test "${enable_aec}" = "yes"
       then
	   AC_DEFINE(AEC_SUPPORT, [], [AEC support])
       fi ])
AM_CONDITIONAL(AEC_SUPPORT, test "${enable_aec}" = "yes")

AC_ARG_ENABLE(gconf,
    AS_HELP_STRING([--enable-gconf],
    	[enables support for GConf (default disabled).])
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


AC_ARG_ENABLE(onlineconf,
    AS_HELP_STRING([--enable-onlineconf],
    	[enables support for storing minisip configuration data online (default disabled - under development).])
   )


if test "x${enable_onlineconf}" = "xyes"; then
   onlineconf_support="yes"
   AC_DEFINE(ONLINECONF_SUPPORT, [], [Support configuration stored online])
fi
AM_CONDITIONAL(ONLINECONF_SUPPORT, [test "x${onlineconf_support}" = "xyes"])


# VNC SUPPORT

AC_ARG_ENABLE(vnc,
    AS_HELP_STRING([--enable-vnc],
    	[enables support for presentation sharing using vnc-rfb (under development).])
   )


if test "x${enable_vnc}" = "xyes"; then
   vnc_support="yes"
   AC_DEFINE(VNC_SUPPORT, [], [Support configuration stored online])
fi
AM_CONDITIONAL(VNC_SUPPORT, [test "x${vnc_support}" = "xyes"])




dnl #############
dnl VIDEO SUPPORT
dnl #############

dnl Blackmagic DeckLink capture support
AC_ARG_ENABLE(decklink,
    AS_HELP_STRING([--enable-decklink],
    	[enables Blackmagic support for HD grabbing (default disabled)]),
    [ if test "${enable_decklink}" = "yes"
      then
             VIDEO_GRABBER="${VIDEO_GRABBER} decklink}"
             DECKLINK_VIDEO_GRABBER="yes"
             DECKLINK_LIBS="-ldl"
             AC_DEFINE(DECKLINK_SUPPORT, [], [Compile DeckLink grabber support.])
             AC_SUBST(DECKLINK_LIBS)
      fi ])



dnl compile for Matrox Imaging library
AC_ARG_ENABLE(mil,
    AS_HELP_STRING([--enable-mil],
    	[enables Matrox Imaging Library support for HD grabbing (default disabled)]),
    [ if test "${enable_mil}" = "yes"
      then
	LDFLAGS_SAVE="${LDFLAGS}"
	CPPFLAGS_SAVE="${CPPFLAGS}"
	MATROX_LDFLAGS="-L${MILDIR}/lib"
	MATROX_CPPFLAGS="-I${MILDIR}/include"
	LDFLAGS="${LDFLAGS} ${MATROX_LDFLAGS}"
	CPPFLAGS="${CPPFLAGS} ${MATROX_CPPFLAGS}"
	AC_CHECK_HEADERS([mil.h],
         [
             AC_CHECK_LIB([mil],[main], [
               VIDEO_GRABBER="${VIDEO_GRABBER} mil}"
               MATROX_VIDEO_GRABBER="yes"
               AC_DEFINE(MATROX_SUPPORT, [], [Compile Matrox imaging library grabber support. Wrong MILDIR environmen variablet?])
               MATROX_LIBS="-lmil"
               ], [AC_MSG_ERROR([Could not find MIL library (-lmil) Wrong MILDIR environment variable?])], [-lmil])
               AC_SUBST(MATROX_LIBS)
               AC_SUBST(MATROX_LDFLAGS)
               AC_SUBST(MATROX_CPPFLAGS)
             ], [AC_MSG_ERROR([Could not find mil.h])] )

	LDFLAGS="${LDFLAGS_SAVE}"
	CPPFLAGS="${CPPFLAGS_SAVE}"
      fi ])


AC_ARG_ENABLE(opengl,
    AS_HELP_STRING([--enable-opengl],
    	[enables OpenGL video output (if present)]))

AC_ARG_ENABLE(sdl,
    AS_HELP_STRING([--enable-sdl],
    	[enables SDL video output (if present)]))

AC_ARG_ENABLE(avcodec-fixes,
    AS_HELP_STRING([--enable-avcodec-fixes],
	[enables support for new avcodec abi (default: auto)]),,
	[enable_avcodec_fixes="auto"])


AC_ARG_ENABLE(video,
    AS_HELP_STRING([--enable-video],
    	[enables video features (default disabled)]),
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
	  FFMPEG_CFLAGS="`pkg-config libavcodec --cflags`"
	  FFMPEG_LIBS="`pkg-config libavcodec --libs`"
	fi

	dnl If pkg-config returned space, fallback to the following
	if test "${FFMPEG_CFLAGS}" = " "
        then
	  FFMPEG_CFLAGS="-I/usr/include/ffmpeg -I/usr/include/libavcodec -I/usr/include/libswscale"
        fi
	if test "${FFMPEG_LDFLAGS}" = " "
        then
	  FFMPEG_LDFLAGS="-avcodec -lavutil"
        fi

	FFMPEG_LIBS="-lswscale ${FFMPEG_LIBS}"
	AVCODEC_LDFLAGS="-lswscale ${AVCODEC_LDFLAGS}"

	LIBS="${FFMPEG_LIBS} ${LIBS}"
	CPPFLAGS="${CPPFLAGS} ${FFMPEG_CFLAGS} ${AVCODEC_CPPFLAGS}"
	LDFLAGS="${LDFLAGS} ${AVCODEC_LDFLAGS}"

	  AC_CHECK_LIB([swscale], [main], [], [AC_MSG_ERROR(
	    [Could not locate libswscale, required for video features.])])

	  AC_CHECK_HEADER([swscale.h], [],
            [
              AC_MSG_ERROR(
	      [Could not locate swscale.h header files. Please install
               the libswscale-dev package.]
              )
            ]
          )



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

	  AC_CHECK_FUNCS([avcodec_register_all], [], [AC_MSG_ERROR(
	    [Could not find avcodec_register_all in libavcodec, required for video features.])])

	  AC_CHECK_LIB([z], [main], [], [AC_MSG_ERROR(
	    [Could not find zlib, required by libavcodec.])])

	  if test "x${enable_avcodec_fixes}" = "xauto"
	  then
		AC_MSG_CHECKING([new avcodec context abi])
		AC_COMPILE_IFELSE([
#include<avcodec.h>

int main()
{
	struct AVCodecContext * context;
	AVRational timeBase = { 1, 15 };

	context->time_base = timeBase;
	context->pix_fmt = PIX_FMT_YUV420P;

	return 0;
}], [enable_avcodec_fixes="yes"], [enable_avcodec_fixes="no"])
		AC_MSG_RESULT(${enable_avcodec_fixes})
	    fi

	if test "x${enable_avcodec_fixes}" = "xyes"
	then
	    AC_DEFINE(AVCODEC_FIXES, [], [Use new avcodec context abi.])
	fi

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
	  XV_LIBS="-lXv"
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
	     ], [], [${X_EXTRA_LIBS} -lXext ${XV_LIBS} ${X_LIBS} -lpthread] )
	  ])

dnl check for SDL-based OpenGL
        AC_CHECK_HEADERS([SDL/SDL_ttf.h],
	  [
	   AC_CHECK_LIB([GLU],[main], [
               if test "x${enable_opengl}" != "xno"
               then
	         VIDEO_DISPLAY="${VIDEO_DISPLAY} opengl"
	         OPENGL_VIDEO_DISPLAY="yes"
	         AC_DEFINE(OPENGL_SUPPORT, [], [Compile OpenGL support])
                 X_EXTRA_LIBS="${X_EXTRA_LIBS} -lXext"
		 OPENGL_LIBS="-lSDL -lGL -lGLU -lSDL_ttf"
	         CPPFLAGS="${CPPFLAGS} ${X_CFLAGS}"

		 AC_SUBST(OPENGL_LIBS)
               fi
	     ], [], [${X_EXTRA_LIBS} -lXext ${XV_LIBS} ${X_LIBS} -lpthread] )
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
AC_SUBST(XV_LIBS)
AC_SUBST(X_CFLAGS)
AC_SUBST(X_EXTRA_LIBS)

AM_CONDITIONAL(VIDEO_SUPPORT, test "${enable_video}" = "yes")
AM_CONDITIONAL(V4L_VIDEO_GRABBER, test "${V4L_VIDEO_GRABBER}" = "yes")
AM_CONDITIONAL(DC1394_VIDEO_GRABBER, test "${DC1394_VIDEO_GRABBER}" = "yes")
AM_CONDITIONAL(MATROX_VIDEO_GRABBER, test "${MATROX_VIDEO_GRABBER}" = "yes")
AM_CONDITIONAL(DECKLINK_VIDEO_GRABBER, test "${DECKLINK_VIDEO_GRABBER}" = "yes")
AM_CONDITIONAL(XV_VIDEO_DISPLAY, test "${XV_VIDEO_DISPLAY}" = "yes")
AM_CONDITIONAL(SDL_VIDEO_DISPLAY, test "${SDL_VIDEO_DISPLAY}" = "yes")
AM_CONDITIONAL(OPENGL_VIDEO_DISPLAY, test "${OPENGL_VIDEO_DISPLAY}" = "yes")





dnl ALSA sound interface
AC_ARG_ENABLE(alsa,
    AS_HELP_STRING([--enable-alsa],
    	[enables ALSA sound support (default disabled)]),
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
    AS_HELP_STRING([--enable-dsound],
    	[enables DirectSound sound support (default disabled)]),
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
   AC_CHECK_LIB([rpcrt4], [main],,[have_dsound=no])
   AC_CHECK_HEADERS([dsound.h],,[have_dsound=no])
   if test x$have_dsound = xyes; then
      DSOUND_LIBS="-ldxguid -luuid -ldsound -lrpcrt4"
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

AM_MINISIP_CHECK_LIBMUTIL([0.8.0])
AM_MINISIP_CHECK_LIBMNETUTIL([0.8.0])
AM_MINISIP_CHECK_LIBMCRYPTO([0.8.0])

AM_MINISIP_CHECK_LIBMSTUN([0.8.0])
AM_MINISIP_CHECK_LIBMSIP([0.8.0])
AM_MINISIP_CHECK_LIBMIKEY([0.8.0])

if test "${enable_zrtp}" = "yes"
then
AM_MINISIP_CHECK_LIBZRTPCPP([0.9.0])
fi

AM_MINISIP_CHECK_COMPLETE

dnl Checks for libraries.
AC_CHECK_LIB([wsock32], [main])

dnl FIXME: Replace `main' with a function in `-lm':
AC_CHECK_LIB([m], [main])
dnl FIXME: Replace `main' with a function in `-lnsl':
AC_CHECK_LIB([nsl], [main])


dnl OPIE
AC_ARG_ENABLE(opie,
    AS_HELP_STRING([--enable-opie],
    	[enables Open Palmtop Integrated Environment support (default disabled)]),
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


dnl Contacts
AC_ARG_ENABLE(ldapcontacts,
    AS_HELP_STRING([--enable-ldapcontacts],
    	[enables support for loading minisip contacts from LDAP (default auto).]),,enable_ldapcontacts="auto")

use_ldapcontacts="no"
if test "${enable_ldapcontacts}" != "no"; then
   CPPFLAGS_SAVE="${CPPFLAGS}"
   CPPFLAGS="${CPPFLAGS} ${MINISIP_CFLAGS}"
   LIBS_SAVE="${LIBS}"
   LIBS="${MINISIP_LIBS} ${LIBS}"

   AC_MSG_CHECKING([LDAP in libmnetutil])
   AC_LINK_IFELSE([
	AC_INCLUDES_DEFAULT([])
#include<libmnetutil/LdapConnection.h>

int main(){
    MRef<LdapConnection*> conn = new LdapConnection("", 0);
    return 0;
}
],
	[use_ldapcontacts="yes"],
	[if test "${enable_ldapcontacts}" = "yes"; then
	   AC_MSG_ERROR([LDAP unsupported by libmnetutil])
	fi])
fi
   AC_MSG_RESULT(${use_ldapcontacts})
   CPPFLAGS="${CPPFLAGS_SAVE}"
   LIBS="${LIBS_SAVE}"

if test "${use_ldapcontacts}" = "yes"; then
   AC_DEFINE(LDAPCONTACTS_SUPPORT, [], [Enable support for contacts details stored on LDAP])
fi
AM_CONDITIONAL(LDAPCONTACTS_SUPPORT, [test "${use_ldapcontacts}" = "yes"])


dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([arpa/inet.h fcntl.h limits.h malloc.h netdb.h netinet/in.h stdlib.h string.h sys/ioctl.h sys/socket.h sys/time.h syslog.h unistd.h])

AC_C_CONST
AC_HEADER_TIME

AC_SUBST(datadir)

dnl it shoudl be like this ... but for now, reduce it ...AC_CONFIG_FILES([Makefile include/Makefile debian/Makefile win32/Makefile libmsip.pc win32/libmsip-res.rc])
AC_CONFIG_FILES([
		Makefile
		libminisip.pc
		include/Makefile
		m4/Makefile
		debian/Makefile
		tests/Makefile
		source/win32/libminisip-res.rc
	])
AC_OUTPUT
