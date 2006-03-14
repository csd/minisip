if TEXT_UI
SUBDIRS = textui
else
if GTK_GUI
SUBDIRS = gtkgui
else
SUBDIRS = qtgui
endif
endif


noinst_LTLIBRARIES = libminisip_gui.la

libminisip_gui_la_SOURCES = Gui.cxx \
                           GuiCallback.cxx

noinst_HEADERS  = Gui.h \
                  GuiCallback.h
