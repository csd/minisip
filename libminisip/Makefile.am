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

libaec_src = aec/aec.cxx

libcodecs_libadd = 
libcodec_ilbc_src = codecs/ilbc/anaFilter.cxx \
			codecs/ilbc/constants.cxx \
			codecs/ilbc/createCB.cxx \
			codecs/ilbc/doCPLC.cxx \
			codecs/ilbc/enhancer.cxx \
			codecs/ilbc/filter.cxx \
			codecs/ilbc/FrameFlassify.cxx \
			codecs/ilbc/gainquant.cxx \
			codecs/ilbc/getCBvec.cxx \
			codecs/ilbc/helpfun.cxx \
			codecs/ilbc/hpInput.cxx \
			codecs/ilbc/hpOutput.cxx \
			codecs/ilbc/iCBConstruct.cxx \
			codecs/ilbc/iCBSearch.cxx \
			codecs/ilbc/iLBC_decode.cxx \
			codecs/ilbc/iLBC_encode.cxx \
			codecs/ilbc/LPCdecode.cxx \
			codecs/ilbc/LPCencode.cxx \
			codecs/ilbc/lsf.cxx \
			codecs/ilbc/packing.cxx \
			codecs/ilbc/StateConstructW.cxx \
			codecs/ilbc/StateSearchW.cxx \
			codecs/ilbc/syntFilter.cxx
libcodecs_la_SOURCES = codecs/ILBCCODEC.cxx \
			codecs/G711CODEC.cxx \
			codecs/Codec.cxx

if HAS_SPEEX
	libcodecs_src += codecs/SPEEXCODEC.cxx
	libcodecs_libadd += $(SPEEX_LIBS)
endif

if HAS_GSM
	libcodecs_src += codecs/GsmCodec.cxx
	libcodecs_libadd += $(GSM_LIBS)
endif

if IPSEC_SUPPORT
	libmsipipsecapi_src = ipsec/MsipIpsecAPI.cxx 
endif

libmediahandler_src = mediahandler/MediaHandler.cxx \
			mediahandler/Session.cxx \
			mediahandler/MediaStream.cxx \
			mediahandler/Media.cxx \
			mediahandler/RtpReceiver.cxx \
			mediahandler/KeyAgreement.cxx \
			mediahandler/MediaCommandString.cxx \
			mediahandler/AudioMedia.cxx \
			mediahandler/SessionRegistry.cxx \
			mediahandler/DtmfSender.cxx

if P2T_SUPPORT
	libp2t_src = p2t/RtcpAPP.cxx \
			p2t/RtcpAPPHeader.cxx \
			p2t/RtcpTransactionGetFloor.cxx \
			p2t/RtcpTransactionGrantFloor.cxx \
			p2t/RtcpTransactionIdleFloor.cxx \
			p2t/RtcpTransactionReleaseFloor.cxx \
			p2t/RtcpTransactionTakenFloor.cxx \
			p2t/SipDialogP2T.cxx \
			p2t/SipDialogP2Tuser.cxx \
			p2t/GroupList.cxx \
			p2t/GroupListUserElement.cxx \
			p2t/GroupListClient.cxx \
			p2t/GroupListServer.cxx \
			p2t/RtcpReceiver.cxx \
			p2t/RtcpSender.cxx \
			p2t/P2T.cxx
endif

librtp_src = rtp/RtpHeader.cxx \
		rtp/RtpPacket.cxx \
		rtp/CryptoContext.cxx \
		rtp/SRtpPacket.cxx \
		rtp/SDESChunk.cxx \
		rtp/SDES_CNAME.cxx \
		rtp/SDES_EMAIL.cxx \
		rtp/SDESItem.cxx \
		rtp/SDES_LOC.cxx \
		rtp/SDES_NAME.cxx \
		rtp/SDES_NOTE.cxx \
		rtp/SDES_PHONE.cxx \
		rtp/SDES_TOOL.cxx \
		rtp/RtcpPacket.cxx \
		rtp/RtcpReport.cxx \
		rtp/RtcpDebugMonitor.cxx \
		rtp/RtcpReportReceptionBlock.cxx \
		rtp/RtcpReportRR.cxx \
		rtp/RtcpReportSDES.cxx \
		rtp/RtcpReportSenderInfo.cxx \
		rtp/RtcpReportSR.cxx \
		rtp/RtcpReportXR.cxx \
		rtp/XRReportBlock.cxx \
		rtp/XRVoIPReportBlock.cxx

libsdp_src = sdp/SdpPacket.cxx \
		sdp/SdpHeader.cxx \
		sdp/SdpHeaderA.cxx \
		sdp/SdpHeaderC.cxx \
		sdp/SdpHeaderM.cxx \
		sdp/SdpHeaderO.cxx \
		sdp/SdpHeaderS.cxx \
		sdp/SdpHeaderT.cxx \
		sdp/SdpHeaderV.cxx

libsip_src = sip/SipDialogVoip.cxx \
		sip/SipDialogVoipClient.cxx \
		sip/SipDialogVoipServer.cxx \
		sip/SipDialogConfVoip.cxx \
		sip/SipDialogPresenceClient.cxx \
		sip/SipDialogPresenceServer.cxx \
		sip/SipSoftPhoneConfiguration.cxx \
		sip/DefaultDialogHandler.cxx \
		sip/PresenceMessageContent.cxx \
		sip/SipDialogSecurityConfig.cxx \
		sip/Sip.cxx

libsoundcard_libadd = 
RESAMPLER_SRC = soundcard/resampler/Resampler.cxx \
		soundcard/resampler/SimpleResampler.cxx
if FLOAT_RESAMPLER
	RESAMPLER_SRC += soundcard/resampler/FloatResampler.cxx
	libsoundcard_libadd += $(SAMPLERATE_LIBS)
endif

AUDIOMIXER_SRC = soundcard/AudioMixer.cxx \
		soundcard/AudioMixerSimple.cxx \
		soundcard/AudioMixerSpatial.cxx

BASIC_SOUNDCARD_SRC = soundcard/SilenceSensor.cxx \
		soundcard/SoundIO.cxx \
		soundcard/SoundSource.cxx \
		${RESAMPLER_SRC} \
		${AUDIOMIXER_SRC} \
		soundcard/FileSoundDevice.cxx \
		soundcard/FileSoundSource.cxx \
		soundcard/SoundDevice.cxx 

ALSA_SRC =	soundcard/AlsaCard.cxx \
		soundcard/AlsaSoundDevice.cxx 

DSOUND_SRC = 	soundcard/DirectSoundDevice.cxx 

PORTAUDIO_SRC = soundcard/PortAudioDevice.cxx

libsoundcard_src = $(BASIC_SOUNDCARD_SRC)

if !OS_WIN
	libsoundcard_src += soundcard/OssSoundDevice.cxx 
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

libspaudio_src = spaudio/SpAudio.cxx 

libstun_src = stun/STUNAttributes.cxx \
		stun/STUN.cxx \
		stun/STUNMessage.cxx \
		stun/STUNTest.cxx

libvideo_libadd =
display_src = video/display/VideoDisplay.cxx display/X11Display.cxx
if XV_VIDEO_DISPLAY
	display_src += video/display/XvDisplay.cxx
	libvideo_libadd += $(X_LIBS) $(X_EXTRA_LIBS)
endif

if SDL_VIDEO_DISPLAY
	display_src += video/display/SdlDisplay.cxx
endif

grabber_src = video/grabber/Grabber.cxx
if V4L_VIDEO_GRABBER
	grabber_src += video/grabber/V4LGrabber.cxx
endif
if DC1394_VIDEO_GRABBER
	grabber_src += video/grabber/Dc1394Grabber.cxx
endif

mixer_src = video/mixer/ImageMixer.cxx

codec_src = video/codec/AVCoder.cxx \
		video/codec/AVDecoder.cxx \
		video/codec/VideoCodec.cxx

libvideo_src = $(display_src) \
		$(grabber_src) \
		$(codec_src) \
		$(mixer_src) \
		video/VideoMedia.cxx

libcontactdb_src = contactdb/ContactDb.cxx \
			contactdb/PhoneBook.cxx \
			contactdb/MXmlPhoneBookIo.cxx

libminisip_gui_src = gui/Gui.cxx \
			gui/LogEntry.cxx \
			gui/Bell.cxx \
			gui/ConsoleDebugger.cxx

libipprovider_src = ipprovider/IpProvider.cxx \
			ipprovider/SimpleIpProvider.cxx \
			ipprovider/StunIpProvider.cxx

if OS_WIN
	res_obj = win32/libminisip-res.o
	noinst_DATA = $(res_obj)
	CLEANFILES = $(res_obj)
endif OS_WIN

.rc.o:
	$(WINDRES) -I$(srcdir) -I$(top_srcdir)/share $< $@

SUFFIXES = .rc .o

libconference_src = conference/ConferenceControl.cxx \
			conference/ConfCallback.cxx \
			conference/ConfMessageRouter.cxx

libconfigbackend_src = configbackend/ConfBackend.cxx \
			 configbackend/MXmlConfBackend.cxx
if OS_WIN
	libconfigbackend_src += configbackend/UserConfigWin32.cxx
else
	libconfigbackend_src += configbackend/UserConfigPosix.cxx
endif

if GCONF_SUPPORT
	AM_CPPFLAGS += @GCONF_CFLAGS@
	libconfigbackend_src += configbackend/GConfBackend.cxx
	libconfigbackend_libadd = $(GCONF_LIBS)
endif

libminisip_la_SOURCES = Minisip.cxx \
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
