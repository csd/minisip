datadir = @datadir@/minisip
CPPFLAGS = @CPPFLAGS@ -DDATADIR="$(datadir)"
LDFLAGS = @LDFLAGS@

noinst_HEADERS = include/*.h 

if VIDEO_SUPPORT
  video_dir = video
endif

dist_data_DATA = share/tray_icon.png share/minisip.glade
EXTRA_DIST = test_cert/*.pem
SUBDIRS = codecs soundcard rtp sip stun p2t mediahandler $(video_dir) minisip
DIST_SUBDIRS = $(SUBDIRS) video debian

