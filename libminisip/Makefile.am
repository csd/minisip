pkgconfigdir = $(libdir)/pkgconfig
pluginsdir = $(pkglibdir)/plugins

SUBDIRS = include m4
DIST_SUBDIRS = $(SUBDIRS)
EXTRA_DIST = libminisip.spec

AM_CPPFLAGS = -I$(top_srcdir)/include $(MINISIP_CFLAGS) 

pkgconfig_DATA = libminisip.pc


lib_LTLIBRARIES = libminisip.la
libminisip_la_CPPFLAGS = $(AM_CPPFLAGS) -DMINISIP_PLUGINDIR=\"$(pluginsdir)\"

libminisip_la_LDFLAGS = $(MINISIP_LIBRARY_LDFLAGS) \
		$(RELOC_LDFLAGS) 

libminisip_la_LIBADD = $(MINISIP_LIBS)

plugins_LTLIBRARIES =

plugins_LDFLAGS = -Wl,--no-undefined -no-undefined $(RELOC_LDFLAGS) -avoid-version -export-dynamic -module

if AEC_SUPPORT
        libaec_src = source/aec/aec.cxx
endif

libcodecs_src = source/codecs/Codec.cxx


#
# G.711 codec plugin
#
libcodec_g711_src = source/codecs/G711CODEC.cxx \
			source/codecs/g711/codec_g711.cxx \
			source/codecs/G711CODEC.h \
			source/codecs/g711/codec_g711.h

if TRUE
libcodecs_src += $(libcodec_g711_src)
else
plugins_LTLIBRARIES += mg711.la
mg711_la_LDFLAGS = $(plugins_LDFLAGS)
mg711_la_SOURCES = $(libcodec_g711_src)
mg711_la_LIBADD = libminisip.la $(MUTIL_LIBS)
endif


#
# Speex codec plugin
#
if HAS_SPEEX
plugins_LTLIBRARIES += mspeex.la
mspeex_la_LDFLAGS = $(plugins_LDFLAGS)
mspeex_la_SOURCES = source/codecs/SPEEXCODEC.cxx source/codecs/SPEEXCODEC.h
mspeex_la_LIBADD = libminisip.la $(MUTIL_LIBS) $(SPEEX_LIBS)
endif


#
# Gsm codec plugin
#
if HAS_GSM
plugins_LTLIBRARIES += mgsm.la
mgsm_la_LDFLAGS = $(plugins_LDFLAGS)
mgsm_la_SOURCES = source/codecs/GsmCodec.cxx source/codecs/GsmCodec.h
mgsm_la_LIBADD = libminisip.la $(MUTIL_LIBS) $(GSM_LIBS)
endif


if IPSEC_SUPPORT
        libipsecapi_src = source/ipsec/MsipIpsecAPI.cxx 
endif

libmediahandler_src = source/mediahandler/MediaHandler.cxx \
			source/mediahandler/Session.cxx \
			source/mediahandler/MediaStream.cxx \
			source/mediahandler/Media.cxx \
			source/mediahandler/RtpReceiver.cxx \
			source/mediahandler/KeyAgreement.cxx \
			source/mediahandler/MediaCommandString.cxx \
			source/mediahandler/AudioMedia.cxx \
			source/mediahandler/AudioPlugin.cxx \
			source/mediahandler/AudioPlugin.h \
			source/mediahandler/SessionRegistry.cxx \
			source/mediahandler/DtmfSender.cxx

if P2T_SUPPORT
        libp2t_src = source/p2t/RtcpAPP.cxx \
			source/p2t/RtcpAPPHeader.cxx \
			source/p2t/RtcpTransactionGetFloor.cxx \
			source/p2t/RtcpTransactionGrantFloor.cxx \
			source/p2t/RtcpTransactionIdleFloor.cxx \
			source/p2t/RtcpTransactionReleaseFloor.cxx \
			source/p2t/RtcpTransactionTakenFloor.cxx \
			source/p2t/SipDialogP2T.cxx \
			source/p2t/SipDialogP2Tuser.cxx \
			source/p2t/GroupList.cxx \
			source/p2t/GroupListUserElement.cxx \
			source/p2t/GroupListClient.cxx \
			source/p2t/GroupListServer.cxx \
			source/p2t/RtcpReceiver.cxx \
			source/p2t/RtcpSender.cxx \
			source/p2t/P2T.cxx
endif

librtp_src = source/rtp/RtpHeader.cxx \
		source/rtp/RtpPacket.cxx \
		source/rtp/CryptoContext.cxx \
		source/rtp/SRtpPacket.cxx \
		source/rtp/SDESChunk.cxx \
		source/rtp/SDES_CNAME.cxx \
		source/rtp/SDES_EMAIL.cxx \
		source/rtp/SDESItem.cxx \
		source/rtp/SDES_LOC.cxx \
		source/rtp/SDES_NAME.cxx \
		source/rtp/SDES_NOTE.cxx \
		source/rtp/SDES_PHONE.cxx \
		source/rtp/SDES_TOOL.cxx \
		source/rtp/RtcpPacket.cxx \
		source/rtp/RtcpReport.cxx \
		source/rtp/RtcpDebugMonitor.cxx \
		source/rtp/RtcpReportReceptionBlock.cxx \
		source/rtp/RtcpReportRR.cxx \
		source/rtp/RtcpReportSDES.cxx \
		source/rtp/RtcpReportSenderInfo.cxx \
		source/rtp/RtcpReportSR.cxx \
		source/rtp/RtcpReportXR.cxx \
		source/rtp/XRReportBlock.cxx \
		source/rtp/XRVoIPReportBlock.cxx

if ZRTP_SUPPORT
libzrtp_src = source/zrtp/ZIDFile.cxx \
		source/zrtp/ZIDRecord.cxx \
		source/zrtp/ZRtp.cxx \
		source/zrtp/ZrtpHostBridgeMinisip.cxx \
		source/zrtp/ZrtpPacketCommit.cxx \
		source/zrtp/ZrtpPacketConf2Ack.cxx \
		source/zrtp/ZrtpPacketConfirm.cxx \
		source/zrtp/ZrtpPacketDHPart.cxx \
		source/zrtp/ZrtpPacketError.cxx \
		source/zrtp/ZrtpPacketHelloAck.cxx \
		source/zrtp/ZrtpPacketHello.cxx \
		source/zrtp/ZrtpStateClass.cxx \
		source/zrtp/ZrtpTextData.cxx

endif

libsdp_src = source/sdp/SdpPacket.cxx \
		source/sdp/SdpHeader.cxx \
		source/sdp/SdpHeaderA.cxx \
		source/sdp/SdpHeaderC.cxx \
		source/sdp/SdpHeaderM.cxx \
		source/sdp/SdpHeaderO.cxx \
		source/sdp/SdpHeaderS.cxx \
		source/sdp/SdpHeaderT.cxx \
		source/sdp/SdpHeaderV.cxx

libsip_src = source/sip/SipDialogVoip.cxx \
		source/sip/SipDialogVoipClient.cxx \
		source/sip/SipDialogVoipServer.cxx \
		source/sip/SipDialogVoipServer100rel.cxx \
		source/sip/SipDialogConfVoip.cxx \
		source/sip/SipDialogPresenceClient.cxx \
		source/sip/SipDialogPresenceServer.cxx \
		source/sip/SipSoftPhoneConfiguration.cxx \
		source/sip/DefaultDialogHandler.cxx \
		source/sip/PresenceMessageContent.cxx \
		source/sip/SipDialogSecurityConfig.cxx \
		source/sip/Sip.cxx

libsoundcard_libadd = 
RESAMPLER_SRC = source/soundcard/resampler/Resampler.cxx \
		source/soundcard/resampler/SimpleResampler.cxx \
		source/soundcard/resampler/SimpleResampler.h

if FLOAT_RESAMPLER
plugins_LTLIBRARIES += mfloat_resampler.la
mfloat_resampler_la_LDFLAGS = $(plugins_LDFLAGS)
mfloat_resampler_la_SOURCES = source/soundcard/resampler/FloatResampler.cxx \
		source/soundcard/resampler/FloatResampler.h
mfloat_resampler_la_LIBADD = libminisip.la $(MUTIL_LIBS) $(SAMPLERATE_LIBS)
endif FLOAT_RESAMPLER

AUDIOMIXER_SRC = source/soundcard/AudioMixer.cxx \
			source/soundcard/AudioMixerSimple.cxx \
			source/soundcard/AudioMixerSpatial.cxx

BASIC_SOUNDCARD_SRC = source/soundcard/SilenceSensor.cxx \
			source/soundcard/SoundIO.cxx \
			source/soundcard/SoundSource.cxx \
			${RESAMPLER_SRC} \
			${AUDIOMIXER_SRC} \
			source/soundcard/FileSoundDevice.cxx \
			source/soundcard/FileSoundSource.cxx \
			source/soundcard/SoundDevice.cxx \
			source/soundcard/SoundDriver.cxx \
			source/soundcard/SoundDriverRegistry.cxx 

ALSA_SRC =	source/soundcard/AlsaCard.cxx \
		source/soundcard/AlsaSoundDevice.cxx 

DSOUND_SRC = 	source/soundcard/DirectSoundDevice.cxx  \
		source/soundcard/DirectSoundDevice.h \
		source/soundcard/DirectSoundDriver.cxx \
		source/soundcard/DirectSoundDriver.h

PORTAUDIO_SRC = source/soundcard/PortAudioDevice.cxx \
			source/soundcard/PortAudioDriver.cxx \
			source/soundcard/PortAudioDevice.h \
			source/soundcard/PortAudioDriver.h

libsoundcard_src = $(BASIC_SOUNDCARD_SRC)

if !OS_WIN
        libsoundcard_src += source/soundcard/OssSoundDevice.cxx 
endif !OS_WIN

if HAVE_LIBASOUND
        libsoundcard_src += $(ALSA_SRC)
        libsoundcard_libadd += $(ALSA_LIBS)
endif HAVE_LIBASOUND

if DIRECT_SOUND
plugins_LTLIBRARIES += mdsound.la
mdsound_la_LDFLAGS = $(plugins_LDFLAGS)
mdsound_la_SOURCES = $(DSOUND_SRC)
mdsound_la_LIBADD = libminisip.la $(MUTIL_LIBS) $(DSOUND_LIBS)
endif DIRECT_SOUND


#
# PortAudio plugin
#
if PORTAUDIO_SUPPORT
plugins_LTLIBRARIES += mportaudio.la
mportaudio_la_LDFLAGS = $(plugins_LDFLAGS)
mportaudio_la_SOURCES = $(PORTAUDIO_SRC)
mportaudio_la_LIBADD = libminisip.la $(MUTIL_LIBS) $(PORTAUDIO_LIBS)
endif PORTAUDIO_SUPPORT


libspaudio_src = source/spaudio/SpAudio.cxx 

#
# Video plugin
#
if VIDEO_SUPPORT
lib_LTLIBRARIES += libminisip_video.la
libminisip_video_la_CPPFLAGS = $(AM_CPPFLAGS) $(AVCODEC_CPPFLAGS)	\
		$(FFMPEG_CFLAGS)
libminisip_video_la_LDFLAGS = $(AVCODEC_LDFLAGS) 
libminisip_video_la_SOURCES = $(libvideo_src)
libminisip_video_la_LIBADD = $(libvideo_libadd) $(FFMPEG_LIBS)

plugins_LTLIBRARIES += mvideo.la
mvideo_la_CPPFLAGS = $(AM_CPPFLAGS) $(AVCODEC_CPPFLAGS) $(FFMPEG_CFLAGS)
mvideo_la_LDFLAGS = $(plugins_LDFLAGS)
mvideo_la_SOURCES = source/video/VideoPlugin.cxx
mvideo_la_LIBADD = libminisip_video.la libminisip.la $(MUTIL_LIBS)

        libvideo_libadd =
        display_src = source/video/display/VideoDisplay.cxx

#
# X11 display plugin
#
plugins_LTLIBRARIES += mx11.la
mx11_la_CXXFLAGS = $(AM_CXXFLAGS) $(X_CFLAGS)
mx11_la_LDFLAGS = $(plugins_LDFLAGS)
mx11_la_SOURCES = source/video/display/X11Display.cxx \
			source/video/display/X11Display.h
mx11_la_LIBADD = libminisip_video.la libminisip.la $(MUTIL_LIBS) $(X_LIBS) $(X_EXTRA_LIBS)

#
# Xv display plugin
#
if XV_VIDEO_DISPLAY
plugins_LTLIBRARIES += mxv.la
mxv_la_CXXFLAGS = $(AM_CXXFLAGS) $(X_CFLAGS)
mxv_la_LDFLAGS = $(plugins_LDFLAGS)
mxv_la_SOURCES = source/video/display/XvDisplay.cxx \
			source/video/display/XvDisplay.h \
			source/video/display/X11Display.cxx \
			source/video/display/X11Display.h
mxv_la_LIBADD = libminisip_video.la libminisip.la $(MUTIL_LIBS) $(XV_LIBS) $(X_LIBS) $(X_EXTRA_LIBS)
endif

#
# SDL display plugin
#
if SDL_VIDEO_DISPLAY
plugins_LTLIBRARIES += msdl.la
msdl_la_LDFLAGS = $(plugins_LDFLAGS)
msdl_la_SOURCES = source/video/display/SdlDisplay.cxx \
			source/video/display/SdlDisplay.h
msdl_la_LIBADD = libminisip_video.la libminisip.la $(MUTIL_LIBS) $(XV_LIBS) $(SDL_LIBS)
endif

        grabber_src = source/video/grabber/Grabber.cxx
if V4L_VIDEO_GRABBER
        grabber_src += source/video/grabber/V4LGrabber.cxx \
			source/video/grabber/V4LGrabber.h
endif

#
# IEEE1394 Digital camera plugin
#
if DC1394_VIDEO_GRABBER
plugins_LTLIBRARIES += mdc1394.la
mdc1394_la_CPPFLAGS = $(MINISIP_CFLAGS) $(AVCODEC_CPPFLAGS) $(FFMPEG_CFLAGS)
mdc1394_la_LDFLAGS = $(plugins_LDFLAGS)
mdc1394_la_SOURCES = source/video/grabber/Dc1394Grabber.cxx \
			source/video/grabber/Dc1394Grabber.h
mdc1394_la_LIBADD = libminisip_video.la libminisip.la $(MINISIP_LIBS) $(DC1394_LIBS)
endif

        mixer_src = source/video/mixer/ImageMixer.cxx

        codec_src = source/video/codec/AVCoder.cxx \
			source/video/codec/AVDecoder.cxx \
			source/video/codec/VideoCodec.cxx

        libvideo_src = $(display_src) \
			$(grabber_src) \
			$(codec_src) \
			$(mixer_src) \
			source/video/VideoMedia.cxx
endif VIDEO_SUPPORT

libcontactdb_src = source/contactdb/ContactDb.cxx \
			source/contactdb/PhoneBook.cxx \
			source/contactdb/MXmlPhoneBookIo.cxx

libgui_src = source/gui/Gui.cxx \
			source/gui/LogEntry.cxx \
			source/gui/Bell.cxx \
			source/gui/ConsoleDebugger.cxx

libipprovider_src = source/ipprovider/IpProvider.cxx \
			source/ipprovider/SimpleIpProvider.cxx \
			source/ipprovider/SimpleIp6Provider.cxx \
			source/ipprovider/SimpleIp6Provider.h \
			source/ipprovider/StunIpProvider.cxx

if OS_WIN
        res_obj = source/win32/libminisip-res.o
        libminisip_la_LDFLAGS += -Wl,$(res_obj)
        libminisip_la_DEPENDENCIES = $(res_obj)
        noinst_DATA = $(res_obj)
        CLEANFILES = $(res_obj)
endif OS_WIN

.rc.o:
	$(WINDRES) -I$(srcdir) -I$(top_srcdir)/share $< $@

SUFFIXES = .rc .o

libconference_src = source/conference/ConferenceControl.cxx \
			source/conference/ConfCallback.cxx \
			source/conference/ConfMessageRouter.cxx

libconfigbackend_src = source/configbackend/ConfBackend.cxx

if TRUE
libconfigbackend_src += source/configbackend/MXmlConfBackend.cxx \
			source/configbackend/MXmlConfBackend.h
else
plugins_LTLIBRARIES += mxmlconf.la
mxmlconf_la_LDFLAGS = $(plugins_LDFLAGS)
mxmlconf_la_SOURCES = source/configbackend/MXmlConfBackend.cxx \
			source/configbackend/MXmlConfBackend.h
mxmlconf_la_LIBADD = libminisip.la $(MUTIL_LIBS)
endif


if OS_WIN
        libconfigbackend_src += source/configbackend/UserConfigWin32.cxx
else
        libconfigbackend_src += source/configbackend/UserConfigPosix.cxx
endif

if GCONF_SUPPORT
plugins_LTLIBRARIES += mgconf.la
mgconf_la_CPPFLAGS = $(AM_CPPFLAGS) $(GCONF_CFLAGS)
mgconf_la_LDFLAGS = $(plugins_LDFLAGS)
mgconf_la_SOURCES = source/configbackend/GConfBackend.cxx \
			source/configbackend/GConfBackend.h
mgconf_la_LIBADD = libminisip.la $(MUTIL_LIBS) $(GCONF_LIBS) $(GOBJECT_LIBS)
endif

libminisip_la_SOURCES = source/Minisip.cxx \
			$(libaec_src) \
			$(libsip_src) \
			$(libcodecs_src) \
			$(libconference_src) \
			$(libconfigbackend_src) \
			$(libcontactdb_src) \
			$(libgui_src) \
			$(libipsecapi_src) \
			$(libipprovider_src) \
			$(libmediahandler_src) \
			$(libp2t_src) \
			$(librtp_src) \
			$(libzrtp_src) \
			$(libsdp_src) \
			$(libsoundcard_src) \
			$(libspaudio_src)

libminisip_la_LIBADD += $(libsoundcard_libadd)

# maintainer rules
ACLOCAL_AMFLAGS = -I m4 ${ACLOCAL_FLAGS}
MAINTAINERCLEANFILES  = $(srcdir)/configure \
			$(srcdir)/Makefile.in \
			$(srcdir)/config/depcomp \
			$(srcdir)/config/config.sub \
			$(srcdir)/config/config.guess \
			$(srcdir)/config/install-sh \
			$(srcdir)/config/ltmain.sh \
			$(srcdir)/config/missing \
			$(srcdir)/depcomp \
			$(srcdir)/config.guess \
			$(srcdir)/config.sub \
			$(srcdir)/ltmain.sh \
			$(srcdir)/missing \
			$(srcdir)/aclocal.m4 \
			$(srcdir)/install-sh

