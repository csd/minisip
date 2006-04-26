abs_top_srcdir = @abs_top_srcdir@
pkgconfigdir = $(libdir)/pkgconfig
pluginsdir = $(pkglibdir)/plugins

SUBDIRS = include m4
DIST_SUBDIRS = $(SUBDIRS)
EXTRA_DIST = libminisip.spec

AM_CPPFLAGS = -I$(top_srcdir)/include $(MINISIP_CFLAGS) 

pkgconfig_DATA = libminisip.pc


lib_LTLIBRARIES = libminisip.la
libminisip_la_CPPFLAGS = $(AM_CPPFLAGS) -DMINISIP_PLUGINDIR=\"$(pluginsdir)\" \
		$(AVCODEC_CPPFLAGS) $(FFMPEG_CFLAGS) $(X_CFLAGS)

libminisip_la_LDFLAGS = $(MINISIP_LIBRARY_LDFLAGS) \
		$(AVCODEC_LDFLAGS) $(RELOC_LDFLAGS) 

libminisip_la_LIBADD = $(MINISIP_LIBS) $(FFMPEG_LIBS) $(DC1394_LIBS) \
		$(SDL_LIBS) $(X_EXTRA_LIBS) $(X_LIBS)

plugins_LTLIBRARIES =


if AEC_SUPPORT
        libaec_src = source/aec/aec.cxx
endif

libcodecs_libadd = 
libcodecs_src = source/codecs/Codec.cxx


#
# G.711 codec plugin
#
libcodec_g711_src = source/codecs/G711CODEC.cxx \
			source/codecs/g711/codec_g711.cxx \
			source/codecs/G711CODEC.h \
			source/codecs/g711/codec_g711.h

plugins_LTLIBRARIES += mg711.la
mg711_la_LDFLAGS = -no-undefined $(RELOC_LDFLAGS) -avoid-version -export-dynamic -module
mg711_la_SOURCES = $(libcodec_g711_src)
mg711_la_LIBADD = libminisip.la $(MUTIL_LIBS)


#
# iLBC codec plugin
#
libcodec_ilbc_src = source/codecs/ILBCCODEC.cxx \
			source/codecs/ILBCCODEC.h \
			source/codecs/ilbc/anaFilter.cxx \
			source/codecs/ilbc/constants.cxx \
			source/codecs/ilbc/createCB.cxx \
			source/codecs/ilbc/doCPLC.cxx \
			source/codecs/ilbc/enhancer.cxx \
			source/codecs/ilbc/filter.cxx \
			source/codecs/ilbc/FrameFlassify.cxx \
			source/codecs/ilbc/gainquant.cxx \
			source/codecs/ilbc/getCBvec.cxx \
			source/codecs/ilbc/helpfun.cxx \
			source/codecs/ilbc/hpInput.cxx \
			source/codecs/ilbc/hpOutput.cxx \
			source/codecs/ilbc/iCBConstruct.cxx \
			source/codecs/ilbc/iCBSearch.cxx \
			source/codecs/ilbc/iLBC_decode.cxx \
			source/codecs/ilbc/iLBC_encode.cxx \
			source/codecs/ilbc/LPCdecode.cxx \
			source/codecs/ilbc/LPCencode.cxx \
			source/codecs/ilbc/lsf.cxx \
			source/codecs/ilbc/packing.cxx \
			source/codecs/ilbc/StateConstructW.cxx \
			source/codecs/ilbc/StateSearchW.cxx \
			source/codecs/ilbc/syntFilter.cxx \
			source/codecs/ilbc/StateConstructW.h \
			source/codecs/ilbc/StateSearchW.h \
			source/codecs/ilbc/helpfun.h \
			source/codecs/ilbc/iCBConstruct.h \
			source/codecs/ilbc/syntFilter.h \
			source/codecs/ilbc/anaFilter.h \
			source/codecs/ilbc/iLBC_decode.h \
			source/codecs/ilbc/constants.h \
			source/codecs/ilbc/getCBvec.h \
			source/codecs/ilbc/enhancer.h \
			source/codecs/ilbc/iCBSearch.h \
			source/codecs/ilbc/hpInput.h \
			source/codecs/ilbc/gainquant.h \
			source/codecs/ilbc/LPCencode.h \
			source/codecs/ilbc/createCB.h \
			source/codecs/ilbc/doCPLC.h \
			source/codecs/ilbc/iLBC_define.h \
			source/codecs/ilbc/packing.h \
			source/codecs/ilbc/LPCdecode.h \
			source/codecs/ilbc/hpOutput.h \
			source/codecs/ilbc/lsf.h \
			source/codecs/ilbc/filter.h \
			source/codecs/ilbc/iLBC_encode.h \
			source/codecs/ilbc/FrameClassify.h

plugins_LTLIBRARIES += milbc.la
milbc_la_LDFLAGS = -no-undefined $(RELOC_LDFLAGS) -avoid-version -export-dynamic -module
milbc_la_SOURCES = $(libcodec_ilbc_src)
milbc_la_LIBADD = libminisip.la $(MUTIL_LIBS)


#
# Speex codec plugin
#
if HAS_SPEEX
plugins_LTLIBRARIES += mspeex.la
mspeex_la_LDFLAGS = -no-undefined $(RELOC_LDFLAGS) -avoid-version -export-dynamic -module
mspeex_la_SOURCES = source/codecs/SPEEXCODEC.cxx source/codecs/SPEEXCODEC.h
mspeex_la_LIBADD = libminisip.la $(MUTIL_LIBS) $(SPEEX_LIBS)
endif


#
# Gsm codec plugin
#
if HAS_GSM
plugins_LTLIBRARIES += mgsm.la
mgsm_la_LDFLAGS = -no-undefined $(RELOC_LDFLAGS) -avoid-version -export-dynamic -module
mgsm_la_SOURCES = source/codecs/GsmCodec.cxx source/codecs/GsmCodec.h
mgsm_la_LIBADD = libminisip.la $(GSM_LIBS)
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
		source/soundcard/resampler/SimpleResampler.cxx
if FLOAT_RESAMPLER
        RESAMPLER_SRC += source/soundcard/resampler/FloatResampler.cxx
        libsoundcard_libadd += $(SAMPLERATE_LIBS)
endif

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

DSOUND_SRC = 	source/soundcard/DirectSoundDevice.cxx 

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
        libsoundcard_src += $(DSOUND_SRC)
        libsoundcard_libadd += $(DSOUND_LIBS)
endif DIRECT_SOUND


#
# PortAudio plugin
#
if PORTAUDIO_SUPPORT
plugins_LTLIBRARIES += mportaudio.la
mportaudio_la_LDFLAGS = -no-undefined $(RELOC_LDFLAGS) -avoid-version -export-dynamic -module
mportaudio_la_SOURCES = $(PORTAUDIO_SRC)
mportaudio_la_LIBADD = libminisip.la $(MUTIL_LIBS) $(PORTAUDIO_LIBS)
endif PORTAUDIO_SUPPORT


libspaudio_src = source/spaudio/SpAudio.cxx 

libstun_src = source/stun/STUNAttributes.cxx \
		source/stun/STUN.cxx \
		source/stun/STUNMessage.cxx \
		source/stun/STUNTest.cxx

if VIDEO_SUPPORT
        libvideo_libadd =
        display_src = source/video/display/VideoDisplay.cxx \
			source/video/display/X11Display.cxx
if XV_VIDEO_DISPLAY
        display_src += source/video/display/XvDisplay.cxx
        libvideo_libadd += $(X_LIBS) $(X_EXTRA_LIBS)
endif

if SDL_VIDEO_DISPLAY
        display_src += source/video/display/SdlDisplay.cxx
endif

        grabber_src = source/video/grabber/Grabber.cxx
if V4L_VIDEO_GRABBER
        grabber_src += source/video/grabber/V4LGrabber.cxx
endif
if DC1394_VIDEO_GRABBER
        grabber_src += source/video/grabber/Dc1394Grabber.cxx
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

plugins_LTLIBRARIES += mxmlconf.la
mxmlconf_la_LDFLAGS = -no-undefined $(RELOC_LDFLAGS) -avoid-version -export-dynamic -module
mxmlconf_la_SOURCES = source/configbackend/MXmlConfBackend.cxx \
			source/configbackend/MXmlConfBackend.h
mxmlconf_la_LIBADD = libminisip.la $(MUTIL_LIBS)


if OS_WIN
        libconfigbackend_src += source/configbackend/UserConfigWin32.cxx
else
        libconfigbackend_src += source/configbackend/UserConfigPosix.cxx
endif

if GCONF_SUPPORT
plugins_LTLIBRARIES += mgconf.la
mgconf_la_CPPFLAGS = $(AM_CPPFLAGS) $(GCONF_CFLAGS)
mgconf_la_LDFLAGS = -no-undefined $(RELOC_LDFLAGS) -avoid-version -export-dynamic -module
mgconf_la_SOURCES = source/configbackend/GConfBackend.cxx \
			source/configbackend/GConfBackend.h
mgconf_la_LIBADD = libminisip.la $(MUTIL_LIBS) $(GCONF_LIBS)
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
			$(libsdp_src) \
			$(libsoundcard_src) \
			$(libspaudio_src) \
			$(libstun_src) \
			$(libvideo_src)

libminisip_la_LIBADD += $(libcodecs_libadd) \
			$(libconfigbackend_libadd) \
			$(libsoundcard_libadd) \
			$(libvideo_libadd)

# maintainer rules
ACLOCAL_AMFLAGS = -I m4 $(ACLOCAL_FLAGS)
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

