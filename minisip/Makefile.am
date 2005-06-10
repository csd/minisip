CPPFLAGS = @CPPFLAGS@ -DDATADIR="$(datadir)"
LDFLAGS = @LDFLAGS@

noinst_HEADERS = include/*.h 

if VIDEO_SUPPORT
  video_dir = video
endif
if IPSEC_SUPPORT
  ipsec_dir = ipsec
endif
if P2T_SUPPORT
  p2t_dir = p2t
endif

dist_pkgdata_DATA = share/tray_icon.png share/secure.png share/insecure.png share/minisip.glade
EXTRA_DIST = test_cert/*.pem Documentation/* minisip-libavcodec.patch share/minisip.png share/minisip.desktop minisip.spec
SUBDIRS = $(ipsec_dir) codecs soundcard spaudio rtp sip stun $(p2t_dir) mediahandler $(video_dir) sdp conf aec minisip
DIST_SUBDIRS = $(SUBDIRS) ipsec video p2t debian

