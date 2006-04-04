abs_top_srcdir = @abs_top_srcdir@
pkgconfigdir = $(libdir)/pkgconfig

SUBDIRS = include
DIST_SUBDIRS = $(SUBDIRS)
EXTRA_DIST = libminisip.spec

pkgconfig_DATA = libminisip.pc

lib_LTLIBRARIES = libminisip.la
libminisip_la_CPPFLAGS = -I$(abs_top_srcdir)/include
libminisip_la_CXXFLAGS = $(MSIP_CFLAGS) $(MIKEY_CFLAGS) $(MNETUTIL_CFLAGS) $(MUTIL_CFLAGS) 
libminisip_la_LDFLAGS = -no-undefined -version-info $(LT_VERSION_INFO)
libminisip_la_LIBADD = $(MSIP_LIBS) $(MIKEY_LIBS) $(MNETUTIL_LIBS) $(MUTIL_LIBS)

if AEC_SUPPORT
	libaec_src = source/aec/aec.cxx
endif

libcodecs_libadd = 
libcodecs_src = source/codecs/G711CODEC.cxx \
			source/codecs/g711/codec_g711.cxx \
			source/codecs/Codec.cxx
libcodec_ilbc_src = source/codecs/ILBCCODEC.cxx \
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
			source/codecs/ilbc/syntFilter.cxx

libcodecs_src += $(libcodec_ilbc_src)

if HAS_SPEEX
	libcodecs_src += source/codecs/SPEEXCODEC.cxx
	libcodecs_libadd += $(SPEEX_LIBS)
endif

if HAS_GSM
	libcodecs_src += source/codecs/GsmCodec.cxx
	libcodecs_libadd += $(GSM_LIBS)
endif

if IPSEC_SUPPORT
	libmsipipsecapi_src = source/ipsec/MsipIpsecAPI.cxx 
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
			source/soundcard/SoundDevice.cxx 

ALSA_SRC =	source/soundcard/AlsaCard.cxx \
		source/soundcard/AlsaSoundDevice.cxx 

DSOUND_SRC = 	source/soundcard/DirectSoundDevice.cxx 

PORTAUDIO_SRC = source/soundcard/PortAudioDevice.cxx

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

if PORTAUDIO_SUPPORT
	libsoundcard_src += $(PORTAUDIO_SRC)
	libsoundcard_libadd += $(PORTAUDIO_LIBS)
endif PORTAUDIO_SUPPORT

libspaudio_src = source/spaudio/SpAudio.cxx 

libstun_src = source/stun/STUNAttributes.cxx \
		source/stun/STUN.cxx \
		source/stun/STUNMessage.cxx \
		source/stun/STUNTest.cxx

if VIDEO_SUPPORT
	libvideo_libadd =
	display_src = source/video/display/VideoDisplay.cxx \
			source/display/X11Display.cxx
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
endif

libcontactdb_src = source/contactdb/ContactDb.cxx \
			source/contactdb/PhoneBook.cxx \
			source/contactdb/MXmlPhoneBookIo.cxx

libminisip_gui_src = source/gui/Gui.cxx \
			source/gui/LogEntry.cxx \
			source/gui/Bell.cxx \
			source/gui/ConsoleDebugger.cxx

libipprovider_src = source/ipprovider/IpProvider.cxx \
			source/ipprovider/SimpleIpProvider.cxx \
			source/ipprovider/StunIpProvider.cxx

if OS_WIN
	res_obj = source/win32/libminisip-res.o
	noinst_DATA = $(res_obj)
	CLEANFILES = $(res_obj)
endif OS_WIN

.rc.o:
	$(WINDRES) -I$(srcdir) -I$(top_srcdir)/share $< $@

SUFFIXES = .rc .o

libconference_src = source/conference/ConferenceControl.cxx \
			source/conference/ConfCallback.cxx \
			source/conference/ConfMessageRouter.cxx

libconfigbackend_src = source/configbackend/ConfBackend.cxx \
			source/ configbackend/MXmlConfBackend.cxx
if OS_WIN
	libconfigbackend_src += source/configbackend/UserConfigWin32.cxx
else
	libconfigbackend_src += source/configbackend/UserConfigPosix.cxx
endif

if GCONF_SUPPORT
	AM_CPPFLAGS += @GCONF_CFLAGS@
	libconfigbackend_src += source/configbackend/GConfBackend.cxx
	libconfigbackend_libadd = $(GCONF_LIBS)
endif

libminisip_la_SOURCES = source/Minisip.cxx \
			$(libaec_src) \
			$(libcodec_ilbc_src) \
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
			$(libsip_src) \
			$(libsoundcard_src) \
			$(libspaudio_src) \
			$(libstun_src) \
			$(libvideo_src)

libminisip_la_LIBADD += $(libcodecs_libadd) \
			$(libsoundcard_libadd) \
			$(libvideo_libadd)
