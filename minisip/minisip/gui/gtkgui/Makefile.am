noinst_LIBRARIES = libminisip_gtkgui.a
AM_CXXFLAGS = @GLADEMM_CFLAGS@ $(GTHREAD_CFLAGS)
AM_CPPFLAGS = -DMINISIP_DATADIR="\"$(pkgdatadir)\"" -I$(top_srcdir)/include

libminisip_gtkgui_a_SOURCES = MainWindow.cxx \
                              CallWidget.cxx \
			      ConferenceWidget.cxx \
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
			      GtkMainUI.h \
			      GtkMainUI.cxx

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
		  DtmfWidget.h
