SUBDIRS = gui contactdb ipprovider confbackend win32

noinst_LIBRARIES = libminisip.a

libminisip_a_SOURCES = Bell.cxx \
                       LocationDetector.cxx \
		       MessageRouter.cxx \
		       ConsoleDebugger.cxx \
		       LogEntry.cxx


bin_PROGRAMS = minisip

AM_LDFLAGS = @AVCODEC_LDFLAGS@ @OSSO_LIBS@ $(RELOC_LDFLAGS)
AM_CXXFLAGS += @OSSO_CFLAGS@
minisip_LDADD = 

if GCONF_SUPPORT
AM_LDFLAGS += @GCONF_LIBS@
endif



if GTK_GUI
AM_LDFLAGS +=  @HILDON_LIBS@
AM_CXXFLAGS += @HILDON_CFLAGS@ @GLADEMM_CFLAGS@
minisip_LDADD += gui/gtkgui/libminisip_gtkgui.a $(GLADEMM_LIBS) $(GTHREAD_LIBS)
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



minisip_LDADD +=gui/libminisip_gui.la \
                libminisip.a \
                ../sip/libsip.la

if IPSEC_SUPPORT
minisip_LDADD +=../ipsec/libmsipipsecapi.la
endif


minisip_LDADD +=../sdp/libsdp.la \
		contactdb/libcontactdb.la

if P2T_SUPPORT
minisip_LDADD +=../p2t/libp2t.la
endif

minisip_LDADD +=../mediahandler/libmediahandler.la \
		        ../conf/libconf.la 




if VIDEO_SUPPORT
minisip_LDADD += ../video/libvideo.la
AM_LDFLAGS += @X_LIBS@ @X_EXTRA_LIBS@
endif


minisip_LDADD +=../rtp/librtp.la \
                ../minisip/gui/libminisip_gui.la \
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



minisip_SOURCES = Minisip.cxx
#main.cxx

if OS_WIN
res_obj = win32/minisip-res.o
minisip_LDADD += $(res_obj)
minisip_DEPENDENCIES = $(res_obj)
endif OS_WIN

noinst_HEADERS  = Bell.h \
                  LocationDetector.h \
                  MessageRouter.h \
                  ConsoleDebugger.h \
		  LogEntry.h \
		  Minisip.h


