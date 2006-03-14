if VIDEO_SUPPORT
  video_dir = video
endif
if IPSEC_SUPPORT
  ipsec_dir = ipsec
endif
if P2T_SUPPORT
  p2t_dir = p2t
endif

SUBDIRS = $(ipsec_dir) codecs soundcard spaudio rtp sip stun $(p2t_dir) mediahandler $(video_dir) sdp conf aec minisip
DIST_SUBDIRS = $(SUBDIRS) ipsec video p2t

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libminisip.pc
