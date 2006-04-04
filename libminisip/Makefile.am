abs_top_srcdir = @abs_top_srcdir@
pkgconfigdir = $(libdir)/pkgconfig

SUBDIRS = include 
DIST_SUBDIRS = $(SUBDIRS)
EXTRA_DIST = libminisip.spec

if VIDEO_SUPPORT
  video_dir = video
endif
if IPSEC_SUPPORT
  ipsec_dir = ipsec
endif
if P2T_SUPPORT
  p2t_dir = p2t
endif

pkgconfig_DATA = libminisip.pc

lib_LTLIBRARIES = libminisip.la
libminisip_la_CPPFLAGS = -I$(abs_top_srcdir)/include
libminisip_la_CXXFLAGS = $(MSIP_CFLAGS) $(MIKEY_CFLAGS) $(MNETUTIL_CFLAGS) $(MUTIL_CFLAGS) 
libminisip_la_LDFLAGS = -no-undefined -version-info $(LT_VERSION_INFO)
libminisip_la_LIBADD = $(MSIP_LIBS) $(MIKEY_LIBS) $(MNETUTIL_LIBS) $(MUTIL_LIBS)

libminisip_la_SOURCES = $(libminisip_src)

if OS_WIN
res_obj = win32/libminisip-res.o
libminisip_la_LDFLAGS += -Wl,$(res_obj)
libminisip_la_DEPENDENCIES = $(res_obj)
endif OS_WIN
if HAVE_LIBASOUND
sound_src = source/soundcard/AlsaSoundDevice.cxx source/soundcard/OssSoundDevice.cxx
else !HAVE_LIBASOUND
if DIRECT_SOUND
sound_src = source/soundcard/DirectSoundDevice.cxx
else !DIRECT_SOUND
sound_src = source/soundcard/OssSoundDevice.cxx
endif !DIRECT_SOUND
endif !HAVE_LIBASOUND



minisip_src =  source/stun/STUN.cxx \
		       source/stun/STUNMessage.cxx \
		       source/stun/STUNTest.cxx \
		       source/stun/STUNAttributes.cxx \
		       source/rtp/CryptoContext.cxx \
		       source/rtp/RtcpPacket.cxx \
		       source/rtp/RtcpReport.cxx \
		       source/rtp/RtcpReportReceptionBlock.cxx \
		       source/rtp/RtcpReportRR.cxx \
		       source/rtp/RtcpReportSDES.cxx \
		       source/rtp/RtcpReportSenderInfo.cxx \
		       source/rtp/RtcpReportSR.cxx \
		       source/rtp/RtcpReportXR.cxx \
		       source/rtp/RtpHeader.cxx \
		       source/rtp/RtpPacket.cxx \
		       source/rtp/SDESChunk.cxx \
		       source/rtp/SDES_CNAME.cxx \
		       source/rtp/SDES_EMAIL.cxx \
		       source/rtp/SDESItem.cxx \
		       source/rtp/SDES_LOC.cxx \
		       source/rtp/SDES_NAME.cxx \
		       source/rtp/SDES_NOTE.cxx \
		       source/rtp/SDES_PHONE.cxx \
		       source/rtp/SDES_TOOL.cxx \
		       source/rtp/SRtpPacket.cxx \
		       source/rtp/XRReportBlock.cxx \
		       source/rtp/XRVoIPReportBlock.cxx \
		       source/sdp/SdpHeaderA.cxx \
		       source/sdp/SdpHeaderC.cxx \
		       source/sdp/SdpHeader.cxx \
		       source/sdp/SdpHeaderM.cxx \
		       source/sdp/SdpHeaderO.cxx \
		       source/sdp/SdpHeaderS.cxx \
		       source/sdp/SdpHeaderT.cxx \
		       source/sdp/SdpHeaderV.cxx \
		       source/sdp/SdpPacket.cxx \
		       source/mediahandler/AudioMedia.cxx \
		       source/mediahandler/DtmfSender.cxx \
		       source/mediahandler/KeyAgreement.cxx \
		       source/mediahandler/Media.cxx \
		       source/mediahandler/MediaCommandString.cxx \
		       source/mediahandler/MediaHandler.cxx \
		       source/mediahandler/MediaStream.cxx \
		       source/mediahandler/RtpReceiver.cxx \
		       source/mediahandler/Session.cxx \
		       source/mediahandler/SessionRegistry.cxx \
		       source/soundcard/AlsaCard.cxx \
		       source/soundcard/FileSoundDevice.cxx \
		       source/soundcard/FileSoundSource.cxx \
		       source/soundcard/SilenceSensor.cxx \
		       source/soundcard/SoundDevice.cxx \
		       source/soundcard/SoundIO.cxx \
		       source/soundcard/SoundSource.cxx \
		       source/spaudio/SpAudio.cxx \
		       source/soundcard/resampler/FloatResampler.cxx \
		       source/soundcard/resampler/Resampler.cxx \
		       source/soundcard/resampler/SimpleResampler.cxx \
		       source/codecs/Codec.cxx \
		       source/codecs/G711CODEC.cxx \
		       source/codecs/g711/codec_g711.cxx \
		       source/codecs/SPEEXCODEC.cxx \
		       source/sip/DefaultDialogHandler.cxx \
		       source/sip/PresenceMessageContent.cxx \
		       source/sip/Sip.cxx \
		       source/sip/SipDialogConfVoip.cxx \
		       source/sip/SipDialogPresenceClient.cxx \
		       source/sip/SipDialogPresenceServer.cxx \
		       source/sip/SipDialogSecurityConfig.cxx \
		       source/sip/SipDialogVoip.cxx \
		       source/sip/SipSoftPhoneConfiguration.cxx \
		       source/ipprovider/IpProvider.cxx \
		       source/ipprovider/SimpleIpProvider.cxx \
		       source/ipprovider/StunIpProvider.cxx \
		       source/conference/ConferenceControl.cxx \
		       source/conference/ConfCallback.cxx \
		       source/Bell.cxx \
		       source/ConsoleDebugger.cxx \
		       source/LogEntry.cxx \
		       source/MessageRouter.cxx \
		       source/Minisip.cxx \
		       source/contactdb/ContactDb.cxx \
		       source/contactdb/MXmlPhoneBookIo.cxx \
		       source/contactdb/PhoneBook.cxx \
		       source/Gui.cxx \
		       source/GuiCallback.cxx
		       
		       
		       




