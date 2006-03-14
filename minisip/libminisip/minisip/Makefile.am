SUBDIRS = gui contactdb ipprovider confbackend win32

lib_LTLIBRARIES = libminisip.la

libminisip_la_SOURCES = Bell.cxx \
                       LocationDetector.cxx \
		       MessageRouter.cxx \
		       ConsoleDebugger.cxx \
		       LogEntry.cxx


AM_LDFLAGS = @AVCODEC_LDFLAGS@ @OSSO_LIBS@ $(RELOC_LDFLAGS)
AM_CXXFLAGS += @OSSO_CFLAGS@

libminisip_la_LDFLAGS = $(AM_LDFLAGS) -no-undefined -version-info $(LT_VERSION_INFO)

libminisip_la_LIBADD =gui/libminisip_gui.la \
                ../sip/libsip.la

if IPSEC_SUPPORT
libminisip_la_LIBADD +=../ipsec/libmsipipsecapi.la
endif


libminisip_la_LIBADD +=../sdp/libsdp.la \
		contactdb/libcontactdb.la

if P2T_SUPPORT
libminisip_la_LIBADD +=../p2t/libp2t.la
endif

libminisip_la_LIBADD +=../mediahandler/libmediahandler.la \
		        ../conf/libconf.la 




if VIDEO_SUPPORT
libminisip_la_LIBADD += ../video/libvideo.la
AM_LDFLAGS += @X_LIBS@ @X_EXTRA_LIBS@
endif


libminisip_la_LIBADD +=../rtp/librtp.la \
                ../codecs/libcodecs.la \
                ../codecs/ilbc/libcodec_ilbc.la \
                ../codecs/g711/libcodec_g711.la \
                ../soundcard/libsoundcard.la \
		../spaudio/libspaudio.la \
		../aec/libaec.la \
		ipprovider/libipprovider.la \
		confbackend/libconfbackend.la \
                ../stun/libstun.la \
		$(MSIP_LIBS) \
		$(MNETUTIL_LIBS) \
		$(MIKEY_LIBS) \
		$(MUTIL_LIBS)


libminisip_la_SOURCES += Minisip.cxx
#main.cxx

if OS_WIN
res_obj = win32/libminisip-res.o
libminisip_la_LDFLAGS += -Wl,$(res_obj)
libminisip_la_DEPENDENCIES = $(res_obj)
endif OS_WIN

noinst_HEADERS  = Bell.h \
                  LocationDetector.h \
                  MessageRouter.h \
                  ConsoleDebugger.h \
		  LogEntry.h \
		  Minisip.h


