pkgconfigdir = $(libdir)/pkgconfig
pluginsdir = $(pkglibdir)/plugins

SUBDIRS = include m4 . tests
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
        libaec_src = source/subsystem_media/aec/aec.cxx
endif

libcodecs_src = source/subsystem_media/codecs/Codec.cxx


#
# G.711 codec plugin
#
libcodec_g711_src = source/subsystem_media/codecs/G711CODEC.cxx \
			source/subsystem_media/codecs/g711/codec_g711.cxx \
			source/subsystem_media/codecs/G711CODEC.h \
			source/subsystem_media/codecs/g711/codec_g711.h

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
mspeex_la_SOURCES = source/subsystem_media/codecs/SPEEXCODEC.cxx source/subsystem_media/codecs/SPEEXCODEC.h
mspeex_la_LIBADD = libminisip.la $(MUTIL_LIBS) $(SPEEX_LIBS)
endif


#
# Gsm codec plugin
#
if HAS_GSM
plugins_LTLIBRARIES += mgsm.la
mgsm_la_LDFLAGS = $(plugins_LDFLAGS)
mgsm_la_SOURCES = source/subsystem_media/codecs/GsmCodec.cxx source/subsystem_media/codecs/GsmCodec.h
mgsm_la_LIBADD = libminisip.la $(MUTIL_LIBS) $(GSM_LIBS)
endif


libmediahandler_src = source/subsystem_media/MediaHandler.cxx \
			source/subsystem_media/MediaHandler.h \
			source/subsystem_media/SubsystemMedia.cxx \
			source/subsystem_media/Session.cxx \
			source/subsystem_media/Session.h \
			source/subsystem_media/MediaStream.cxx \
			source/subsystem_media/Media.cxx \
			source/subsystem_media/RealtimeMedia.cxx \
			source/subsystem_media/RtpReceiver.cxx \
			source/subsystem_media/MediaCommandString.cxx \
			source/subsystem_media/AudioMedia.cxx \
			source/subsystem_media/AudioPlugin.cxx \
			source/subsystem_media/AudioPlugin.h \
			source/subsystem_media/SessionRegistry.cxx \
			source/subsystem_media/SessionRegistry.h \
			source/subsystem_media/CallRecorder.cxx \
			source/subsystem_media/DtmfSender.cxx


if P2T_SUPPORT
        libp2t_src = source/subsystem_signaling/p2t/RtcpAPP.cxx \
			source/subsystem_signaling/p2t/RtcpAPPHeader.cxx \
			source/subsystem_signaling/p2t/RtcpTransactionGetFloor.cxx \
			source/subsystem_signaling/p2t/RtcpTransactionGrantFloor.cxx \
			source/subsystem_signaling/p2t/RtcpTransactionIdleFloor.cxx \
			source/subsystem_signaling/p2t/RtcpTransactionReleaseFloor.cxx \
			source/subsystem_signaling/p2t/RtcpTransactionTakenFloor.cxx \
			source/subsystem_signaling/p2t/SipDialogP2T.cxx \
			source/subsystem_signaling/p2t/SipDialogP2Tuser.cxx \
			source/subsystem_signaling/p2t/GroupList.cxx \
			source/subsystem_signaling/p2t/GroupListUserElement.cxx \
			source/subsystem_signaling/p2t/GroupListClient.cxx \
			source/subsystem_signaling/p2t/GroupListServer.cxx \
			source/subsystem_signaling/p2t/RtcpReceiver.cxx \
			source/subsystem_signaling/p2t/RtcpSender.cxx \
			source/subsystem_signaling/p2t/P2T.cxx
endif

if MSRP_SUPPORT
libmsrp_src = source/subsystem_media/msrp/MSRPMessage.cxx \
		source/subsystem_media/msrp/MSRPMessage.h \
		source/subsystem_signaling/sip/SipDialogFileTransferClient.cxx \
		source/subsystem_signaling/sip/SipDialogFileTransferClient.h \
		source/subsystem_signaling/sip/SipDialogFileTransferServer.cxx \
		source/subsystem_signaling/sip/SipDialogFileTransferServer.h

endif


if ZRTP_SUPPORT
libzrtp_src = source/subsystem_media/zrtp/ZrtpHostBridgeMinisip.cxx
endif

librtp_src = source/subsystem_media/rtp/RtpHeader.cxx \
		source/subsystem_media/rtp/RtpPacket.cxx \
		source/subsystem_media/rtp/CryptoContext.cxx \
		source/subsystem_media/rtp/SRtpPacket.cxx \
		source/subsystem_media/rtp/SDESChunk.cxx \
		source/subsystem_media/rtp/SDES_CNAME.cxx \
		source/subsystem_media/rtp/SDES_EMAIL.cxx \
		source/subsystem_media/rtp/SDESItem.cxx \
		source/subsystem_media/rtp/SDES_LOC.cxx \
		source/subsystem_media/rtp/SDES_NAME.cxx \
		source/subsystem_media/rtp/SDES_NOTE.cxx \
		source/subsystem_media/rtp/SDES_PHONE.cxx \
		source/subsystem_media/rtp/SDES_TOOL.cxx \
		source/subsystem_media/rtp/RtcpPacket.cxx \
		source/subsystem_media/rtp/RtcpReport.cxx \
		source/subsystem_media/rtp/RtcpDebugMonitor.cxx \
		source/subsystem_media/rtp/RtcpReportReceptionBlock.cxx \
		source/subsystem_media/rtp/RtcpReportRR.cxx \
		source/subsystem_media/rtp/RtcpReportSDES.cxx \
		source/subsystem_media/rtp/RtcpReportSenderInfo.cxx \
		source/subsystem_media/rtp/RtcpReportSR.cxx \
		source/subsystem_media/rtp/RtcpReportXR.cxx \
		source/subsystem_media/rtp/XRReportBlock.cxx \
		source/subsystem_media/rtp/XRVoIPReportBlock.cxx

libsdp_src = source/subsystem_signaling/sdp/SdpPacket.cxx \
		source/subsystem_signaling/sdp/SdpHeader.cxx \
		source/subsystem_signaling/sdp/SdpHeaderA.cxx \
		source/subsystem_signaling/sdp/SdpHeaderC.cxx \
		source/subsystem_signaling/sdp/SdpHeaderI.cxx \
		source/subsystem_signaling/sdp/SdpHeaderM.cxx \
		source/subsystem_signaling/sdp/SdpHeaderO.cxx \
		source/subsystem_signaling/sdp/SdpHeaderS.cxx \
		source/subsystem_signaling/sdp/SdpHeaderT.cxx \
		source/subsystem_signaling/sdp/SdpHeaderV.cxx

libsip_src = source/subsystem_signaling/sip/SipDialogVoip.cxx \
		source/subsystem_signaling/sip/SipDialogVoipClient.cxx \
		source/subsystem_signaling/sip/SipDialogVoipServer.cxx \
		source/subsystem_signaling/sip/SipDialogConfVoip.cxx \
		source/subsystem_signaling/sip/SipDialogPresenceClient.cxx \
		source/subsystem_signaling/sip/SipDialogPresenceServer.cxx \
		source/subsystem_signaling/sip/SipSoftPhoneConfiguration.cxx \
		source/subsystem_signaling/sip/DefaultDialogHandler.cxx \
		source/subsystem_signaling/sip/PresenceMessageContent.cxx \
		source/subsystem_signaling/sip/Sip.cxx

libsoundcard_libadd =
RESAMPLER_SRC = source/subsystem_media/soundcard/resampler/Resampler.cxx \
		source/subsystem_media/soundcard/resampler/SimpleResampler.cxx \
		source/subsystem_media/soundcard/resampler/SimpleResampler.h

if FLOAT_RESAMPLER
plugins_LTLIBRARIES += mfloat_resampler.la
mfloat_resampler_la_LDFLAGS = $(plugins_LDFLAGS)
mfloat_resampler_la_SOURCES = source/subsystem_media/soundcard/resampler/FloatResampler.cxx \
		source/subsystem_media/soundcard/resampler/FloatResampler.h
mfloat_resampler_la_LIBADD = libminisip.la $(MUTIL_LIBS) $(SAMPLERATE_LIBS)
endif FLOAT_RESAMPLER

AUDIOMIXER_SRC = source/subsystem_media/soundcard/AudioMixer.cxx \
			source/subsystem_media/soundcard/AudioMixerSimple.cxx \
			source/subsystem_media/soundcard/AudioMixerSpatial.cxx

BASIC_SOUNDCARD_SRC = source/subsystem_media/soundcard/SilenceSensor.cxx \
			source/subsystem_media/soundcard/SoundIO.cxx \
			source/subsystem_media/soundcard/SoundSource.cxx \
			${RESAMPLER_SRC} \
			${AUDIOMIXER_SRC} \
			source/subsystem_media/soundcard/FileSoundDevice.cxx \
			source/subsystem_media/soundcard/FileSoundDriver.cxx \
			source/subsystem_media/soundcard/FileSoundDriver.h \
			source/subsystem_media/soundcard/FileSoundSource.cxx \
			source/subsystem_media/soundcard/SoundDevice.cxx \
			source/subsystem_media/soundcard/SoundDriver.cxx \
			source/subsystem_media/soundcard/SoundDriverRegistry.cxx

ALSA_SRC =	source/subsystem_media/soundcard/AlsaCard.cxx \
		source/subsystem_media/soundcard/AlsaCard.h \
		source/subsystem_media/soundcard/AlsaSoundDriver.h \
		source/subsystem_media/soundcard/AlsaSoundDriver.cxx \
		source/subsystem_media/soundcard/AlsaSoundDevice.h \
		source/subsystem_media/soundcard/AlsaSoundDevice.cxx

DSOUND_SRC = 	source/subsystem_media/soundcard/DirectSoundDevice.cxx  \
		source/subsystem_media/soundcard/DirectSoundDevice.h \
		source/subsystem_media/soundcard/DirectSoundDriver.cxx \
		source/subsystem_media/soundcard/DirectSoundDriver.h

PORTAUDIO_SRC = source/subsystem_media/soundcard/PortAudioDevice.cxx \
			source/subsystem_media/soundcard/PortAudioDriver.cxx \
			source/subsystem_media/soundcard/PortAudioDevice.h \
			source/subsystem_media/soundcard/PortAudioDriver.h

libsoundcard_src = $(BASIC_SOUNDCARD_SRC)

if !OS_WIN
if ENABLE_OSS
libsoundcard_src += source/subsystem_media/soundcard/OssSoundDevice.cxx \
		source/subsystem_media/soundcard/OssSoundDevice.h \
		source/subsystem_media/soundcard/OssSoundDriver.cxx \
		source/subsystem_media/soundcard/OssSoundDriver.h
endif ENABLE_OSS
endif !OS_WIN

if HAVE_LIBASOUND
plugins_LTLIBRARIES += malsa.la
malsa_la_LDFLAGS = $(plugins_LDFLAGS)
malsa_la_SOURCES = $(ALSA_SRC)
malsa_la_LIBADD = libminisip.la $(MUTIL_LIBS) $(ALSA_LIBS)
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


libspaudio_src = source/subsystem_media/spaudio/SpAudio.cxx

#
# Video plugin
#
if VIDEO_SUPPORT
lib_LTLIBRARIES += libminisip_video.la
libminisip_video_la_CPPFLAGS = $(AM_CPPFLAGS) $(AVCODEC_CPPFLAGS)	\
		$(FFMPEG_CFLAGS)
libminisip_video_la_LDFLAGS = $(MINISIP_LIBRARY_LDFLAGS)	\
		$(RELOC_LDFLAGS) $(AVCODEC_LDFLAGS)
libminisip_video_la_SOURCES = $(libvideo_src)
libminisip_video_la_LIBADD = $(libvideo_libadd) $(FFMPEG_LIBS)	\
		libminisip.la $(MUTIL_LIBS)

plugins_LTLIBRARIES += mvideo.la
mvideo_la_CPPFLAGS = $(AM_CPPFLAGS) $(AVCODEC_CPPFLAGS) $(FFMPEG_CFLAGS)
mvideo_la_LDFLAGS = $(plugins_LDFLAGS)
mvideo_la_SOURCES = source/subsystem_media/video/VideoPlugin.cxx
mvideo_la_LIBADD = libminisip_video.la libminisip.la $(MUTIL_LIBS)

        libvideo_libadd =
        display_src = source/subsystem_media/video/display/VideoDisplay.cxx

#
# X11 display plugin
#
plugins_LTLIBRARIES += mx11.la
mx11_la_CXXFLAGS = $(AM_CXXFLAGS) $(X_CFLAGS)
mx11_la_LDFLAGS = $(plugins_LDFLAGS)
mx11_la_SOURCES = source/subsystem_media/video/display/X11Display.cxx \
			source/subsystem_media/video/display/X11Display.h
mx11_la_LIBADD = libminisip_video.la libminisip.la $(MUTIL_LIBS) $(X_LIBS) $(X_EXTRA_LIBS)

#
# Xv display plugin
#
if XV_VIDEO_DISPLAY
plugins_LTLIBRARIES += mxv.la
mxv_la_CXXFLAGS = $(AM_CXXFLAGS) $(X_CFLAGS)
mxv_la_LDFLAGS = $(plugins_LDFLAGS)
mxv_la_SOURCES = source/subsystem_media/video/display/XvDisplay.cxx \
			source/subsystem_media/video/display/XvDisplay.h \
			source/subsystem_media/video/display/X11Display.cxx \
			source/subsystem_media/video/display/X11Display.h
mxv_la_LIBADD = libminisip_video.la libminisip.la $(MUTIL_LIBS) $(XV_LIBS) $(X_LIBS) $(X_EXTRA_LIBS)
endif

#
# SDL display plugin
#
if SDL_VIDEO_DISPLAY
plugins_LTLIBRARIES += msdl.la
msdl_la_LDFLAGS = $(plugins_LDFLAGS)
msdl_la_SOURCES = source/subsystem_media/video/display/SdlDisplay.cxx \
			source/subsystem_media/video/display/SdlDisplay.h
msdl_la_LIBADD = libminisip_video.la libminisip.la $(MUTIL_LIBS) $(XV_LIBS) $(SDL_LIBS) $(X_LIBS) $(X_EXTRA_LIBS)
endif

        grabber_src = source/subsystem_media/video/grabber/Grabber.cxx
if V4L_VIDEO_GRABBER
        grabber_src += source/subsystem_media/video/grabber/V4LGrabber.cxx \
			source/subsystem_media/video/grabber/V4LGrabber.h
endif

#
# IEEE1394 Digital camera plugin
#
if DC1394_VIDEO_GRABBER
plugins_LTLIBRARIES += mdc1394.la
mdc1394_la_CPPFLAGS = $(MINISIP_CFLAGS) $(AVCODEC_CPPFLAGS) $(FFMPEG_CFLAGS)
mdc1394_la_LDFLAGS = $(plugins_LDFLAGS)
mdc1394_la_SOURCES = source/subsystem_media/video/grabber/Dc1394Grabber.cxx \
			source/subsystem_media/video/grabber/Dc1394Grabber.h
mdc1394_la_LIBADD = libminisip_video.la libminisip.la $(MINISIP_LIBS) $(DC1394_LIBS)
endif

        mixer_src = source/subsystem_media/video/mixer/ImageMixer.cxx

        codec_src = source/subsystem_media/video/codec/AVCoder.cxx \
			source/subsystem_media/video/codec/AVDecoder.cxx \
			source/subsystem_media/video/codec/VideoCodec.cxx

        libvideo_src = $(display_src) \
			$(grabber_src) \
			$(codec_src) \
			$(mixer_src) \
			source/subsystem_media/video/VideoMedia.cxx
endif VIDEO_SUPPORT

libcontactdb_src = source/subsystem_contacts/ContactDb.cxx \
			source/subsystem_contacts/PhoneBook.cxx \
			source/subsystem_contacts/MXmlPhoneBookIo.cxx \
			source/subsystem_contacts/MXmlPhoneBookIo.h

libgui_src = source/subsystem_gui/Gui.cxx \
			source/subsystem_gui/LogEntry.cxx \
			source/subsystem_gui/Bell.cxx \
			source/subsystem_gui/ConsoleDebugger.cxx

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

libconfigbackend_src = source/subsystem_config/ConfBackend.cxx

if TRUE
libconfigbackend_src += source/subsystem_config/MXmlConfBackend.cxx \
			source/subsystem_config/MXmlConfBackend.h
else
plugins_LTLIBRARIES += mxmlconf.la
mxmlconf_la_LDFLAGS = $(plugins_LDFLAGS)
mxmlconf_la_SOURCES = source/subsystem_config/MXmlConfBackend.cxx \
			source/subsystem_config/MXmlConfBackend.h
mxmlconf_la_LIBADD = libminisip.la $(MUTIL_LIBS)
endif


if OS_WIN
        libconfigbackend_src += source/subsystem_config/UserConfigWin32.cxx
else
        libconfigbackend_src += source/subsystem_config/UserConfigPosix.cxx
endif

if GCONF_SUPPORT
plugins_LTLIBRARIES += mgconf.la
mgconf_la_CPPFLAGS = $(AM_CPPFLAGS) $(GCONF_CFLAGS)
mgconf_la_LDFLAGS = $(plugins_LDFLAGS)
mgconf_la_SOURCES = source/subsystem_config/GConfBackend.cxx \
			source/subsystem_config/GConfBackend.h
mgconf_la_LIBADD = libminisip.la $(MUTIL_LIBS) $(GCONF_LIBS) $(GOBJECT_LIBS)
endif

if ONLINECONF_SUPPORT
plugins_LTLIBRARIES += onlineconf.la
onlineconf_la_CPPFLAGS = $(AM_CPPFLAGS) $(ONLINECONF_CFLAGS)
onlineconf_la_LDFLAGS = $(plugins_LDFLAGS) -lmcrypto -lmnetutil
onlineconf_la_SOURCES = source/subsystem_config/OnlineMXmlConfBackend.cxx \
			source/subsystem_config/OnlineMXmlConfBackend.h
onlineconf_la_LIBADD = libminisip.la $(MUTIL_LIBS) $(ONLINECONF_LIBS)
libonlineconf_src = source/subsystem_config/OnlineConfBackend.cxx \
	  	    source/subsystem_contacts/OnlineMXmlPhoneBookIo.cxx \
	  	    source/subsystem_contacts/OnlineMXmlPhoneBookIo.h
endif

if LDAPCONTACTS_SUPPORT
plugins_LTLIBRARIES += ldappb.la
ldappb_la_CPPFLAGS = $(AM_CPPFLAGS)
ldappb_la_LDFLAGS = $(plugins_LDFLAGS)
ldappb_la_SOURCES = source/subsystem_contacts/LdapPhoneBookIo.cxx
ldappb_la_LIBADD = libminisip.la $(MUTIL_LIBS) $(MNETUTIL_LIBS)
endif LDAPCONTACTS_SUPPORT

libminisip_la_SOURCES = source/Minisip.cxx \
			source/MinisipExceptions.cxx \
			$(libonlineconf_src) \
			$(libaec_src) \
			$(libsip_src) \
			$(libcodecs_src) \
			$(libconference_src) \
			$(libconfigbackend_src) \
			$(libcontactdb_src) \
			$(libgui_src) \
			$(libipprovider_src) \
			$(libmediahandler_src) \
			$(libp2t_src) \
			$(librtp_src) \
			$(libmsrp_src) \
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


dist-hook:
	cp -a version $(distdir)/.version
