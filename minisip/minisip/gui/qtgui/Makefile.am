MOC = @MOC@

noinst_LIBRARIES = libminisip_qtgui.a

AM_CPPFLAGS = -I@QT_INCLUDE@

libminisip_qtgui_a_SOURCES = CallDialog.cxx \
			     MinisipMainWindowWidget.cxx \
			     qtguistarter.cxx \
			     SettingsDialog.cxx \
			     CertificateDialog.cxx

nodist_libminisip_qtgui_a_SOURCES = moc_CallDialog.cpp \
			            moc_MinisipMainWindowWidget.cpp \
			            moc_SettingsDialog.cpp \
			            moc_CertificateDialog.cpp


moc_CallDialog.cpp: CallDialog.h
	$(MOC) CallDialog.h -o moc_CallDialog.cpp

moc_MinisipMainWindowWidget.cpp: MinisipMainWindowWidget.h
	$(MOC) MinisipMainWindowWidget.h -o moc_MinisipMainWindowWidget.cpp

moc_SettingsDialog.cpp: SettingsDialog.h
	$(MOC) SettingsDialog.h -o moc_SettingsDialog.cpp

moc_CertificateDialog.cpp: CertificateDialog.h
	$(MOC) CertificateDialog.h -o moc_CertificateDialog.cpp

noinst_HEADERS  = CallDialog.h \
                  MinisipMainWindowWidget.h \
                  qtguistarter.h \
                  SettingsDialog.h \
		  CertificateDialog.h

MAINTAINERCLEANFILES = $(srcdir)/Makefile.in
