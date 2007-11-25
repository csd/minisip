bin_PROGRAMS = minisip_gtkgui
AM_CXXFLAGS = $(HILDON_CFLAGS) \
		$(GLADEMM_CFLAGS) \
		$(GTHREAD_CFLAGS)

AM_CPPFLAGS = -DMINISIP_DATADIR="\"$(pkgdatadir)\""	\
		-I$(top_srcdir)/include \
		$(MINISIP_CFLAGS)

AM_LDFLAGS = $(RELOC_LDFLAGS)

minisip_gtkgui_LDADD =	$(MINISIP_LIBS) \
			$(HILDON_LIBS)  \
			$(GLADEMM_LIBS) \
			$(GTHREAD_LIBS)

minisip_gtkgui_SOURCES = MainWindow.cxx \
				CallWidget.cxx \
				ConferenceWidget.cxx \
				TransportList.h \
				TransportList.cxx \
				PhoneBook.cxx \
				SettingsDialog.cxx \
				CertificateDialog.cxx \
				eggtrayicon.cxx \
				TrayIcon.cxx \
				LogWidget.cxx \
				ImWidget.cxx \
				ContactDialog.cxx \
				AccountDialog.cxx \
				AccountsList.cxx \
				AccountsStatusWidget.cxx \
				DtmfWidget.cxx \
				GtkMainUI.cxx \
				GtkMain.cxx

noinst_HEADERS  = MainWindow.h \
			CallWidget.h \
			ConferenceWidget.h \
			PhoneBook.h \
			SettingsDialog.h \
			CertificateDialog.h \
			eggtrayicon.h \
			TrayIcon.h \
			LogWidget.h \
			ImWidget.h \
			ContactDialog.h \
			AccountDialog.h \
			AccountsList.h \
			AccountsStatusWidget.h \
			DtmfWidget.h \
			GtkMainUI.h 

if OS_WIN

if !DEBUG
# Build a GUI application (without console)
AM_LDFLAGS += -mwindows
endif !DEBUG

        res_obj = ../../win32/minisip-res.o
        minisip_gtkgui_LDADD += $(res_obj)
        minisip_gtkgui_DEPENDENCIES = $(res_obj)
endif OS_WIN

MAINTAINERCLEANFILES = $(srcdir)/Makefile.in
