SUBDIRS = gui contactdb ipprovider

noinst_LIBRARIES = libminisip.a

libminisip_a_SOURCES = Bell.cxx \
                       LocationDetector.cxx \
		       MessageRouter.cxx \
		       ConsoleDebugger.cxx \
		       LogEntry.cxx


bin_PROGRAMS = minisip

AM_LDFLAGS = @LDFLAGS@ @AVCODEC_LDFLAGS@
AM_CXXFLAGS = @CXXFLAGS@
AM_CPPFLAGS = @CPPFLAGS@



if GTK_GUI
AM_LDFLAGS +=  @GLADEMM_LIBS@
AM_CXXFLAGS += @GLADEMM_CFLAGS@
minisip_LDADD = gui/gtkgui/libminisip_gtkgui.a
else !GTK_GUI
if TEXT_UI
minisip_LDADD = gui/textui/libminisip_textui.a
else
AM_CPPFLAGS += -I@QT_INCLUDE@
AM_LDFLAGS += -L@QT_LIB_LOCATION@
minisip_LDADD = gui/qtgui/libminisip_qtgui.a
endif !TEXT_UI
endif !GTK_GUI



minisip_LDADD +=gui/libminisip_gui.a \
                libminisip.a \
                ../sip/libsip.a

if IPSEC_SUPPORT
minisip_LDADD +=../ipsec/libmsipipsecapi.a
endif


minisip_LDADD +=../sdp/libsdp.a \
		contactdb/libcontactdb.a

if P2T_SUPPORT
minisip_LDADD +=../p2t/libp2t.a
endif

minisip_LDADD +=../mediahandler/libmediahandler.a \
		        ../conf/libconf.a 




if VIDEO_SUPPORT
minisip_LDADD += ../video/libvideo.a
AM_LDFLAGS += @X_LIBS@ @X_EXTRA_LIBS@
endif


                
minisip_LDADD +=../rtp/librtp.a \
                ../minisip/gui/libminisip_gui.a \
                ../codecs/libcodecs.a \
                ../codecs/ilbc/libcodec_ilbc.a \
                ../codecs/g711/libcodec_g711.a \
                ../soundcard/libsoundcard.a \
		../spaudio/libspaudio.a \
		ipprovider/libipprovider.a \
                ../stun/libstun.a



minisip_SOURCES = Minisip.cxx
#main.cxx

noinst_HEADERS  = Bell.h \
                  LocationDetector.h \
                  MessageRouter.h \
                  ConsoleDebugger.h \
		  LogEntry.h \
		  Minisip.h


