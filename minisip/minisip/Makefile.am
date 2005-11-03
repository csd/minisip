SUBDIRS = gui contactdb ipprovider confbackend

noinst_LIBRARIES = libminisip.a

libminisip_a_SOURCES = Bell.cxx \
                       LocationDetector.cxx \
		       MessageRouter.cxx \
		       ConsoleDebugger.cxx \
		       LogEntry.cxx


bin_PROGRAMS = minisip

AM_LDFLAGS = @LDFLAGS@ @AVCODEC_LDFLAGS@ @OSSO_LIBS@
AM_CXXFLAGS = @CXXFLAGS@ @OSSO_CFLAGS@
minisip_LDADD = 

if GCONF_SUPPORT
AM_LDFLAGS += @GCONF_LIBS@
endif



if GTK_GUI
AM_LDFLAGS +=  @HILDON_LIBS@ @GLADEMM_LIBS@
AM_CXXFLAGS += @HILDON_CFLAGS@ @GLADEMM_CFLAGS@
minisip_LDADD += gui/gtkgui/libminisip_gtkgui.a
else !GTK_GUI
if TEXT_UI
minisip_LDADD += gui/textui/libminisip_textui.a
else !TEXT_UI
if QT_GUI
AM_CPPFLAGS += -I@QT_INCLUDE@
AM_LDFLAGS += -L@QT_LIB_LOCATION@
minisip_LDADD += gui/qtgui/libminisip_qtgui.a
endif QT_GUI
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
		../aec/libaec.a \
		ipprovider/libipprovider.a \
		confbackend/libconfbackend.a \
                ../stun/libstun.a



minisip_SOURCES = Minisip.cxx
#main.cxx

noinst_HEADERS  = Bell.h \
                  LocationDetector.h \
                  MessageRouter.h \
                  ConsoleDebugger.h \
		  LogEntry.h \
		  Minisip.h


