if TEXT_UI
SUBDIRS = textui
else
if GTK_GUI
SUBDIRS = gtkgui
else
SUBDIRS = qtgui
endif
endif


noinst_LIBRARIES = libminisip_gui.a

libminisip_gui_a_SOURCES = Gui.cxx \
                           GuiCallback.cxx

noinst_HEADERS  = Gui.h \
                  GuiCallback.h
