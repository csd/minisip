CPPFLAGS = @CPPFLAGS@ -DDATADIR="$(datadir)"
LDFLAGS = @LDFLAGS@

noinst_HEADERS = include/*.h 

if VIDEO_SUPPORT
  video_dir = video
endif

dist_pkgdata_DATA = share/tray_icon.png share/minisip.glade
EXTRA_DIST = test_cert/*.pem Documentation/* minisip-libavcodec.patch
SUBDIRS = codecs soundcard rtp sip stun p2t mediahandler $(video_dir) sdp minisip
DIST_SUBDIRS = $(SUBDIRS) video debian

