noinst_LIBRARIES = libminisip_gtkgui.a
datadir = @datadir@/minisip
AM_CXXFLAGS = @CXXFLAGS@ @GLADEMM_CFLAGS@
AM_CPPFLAGS = @CPPFLAGS@ -DMINISIP_DATADIR="\"$(datadir)\""

libminisip_gtkgui_a_SOURCES = MainWindow.cxx \
                              CallWidget.cxx \
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
			      DtmfWidget.cxx
			  
	

noinst_HEADERS  = MainWindow.h \
                  CallWidget.h \
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
		  DtmfWidget.h
