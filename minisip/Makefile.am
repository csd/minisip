CPPFLAGS = @CPPFLAGS@ -DDATADIR="$(datadir)"
LDFLAGS = @LDFLAGS@

noinst_HEADERS = include/*.h 

if VIDEO_SUPPORT
  video_dir = video
endif
if IPSEC_SUPPORT
  ipsec_dir = ipsec
endif

dist_pkgdata_DATA = share/tray_icon.png share/minisip.glade
EXTRA_DIST = test_cert/*.pem Documentation/* minisip-libavcodec.patch
SUBDIRS = $(ipsec_dir) codecs soundcard rtp sip stun p2t mediahandler $(video_dir) sdp minisip
DIST_SUBDIRS = $(SUBDIRS) ipsec video debian

