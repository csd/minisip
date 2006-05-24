dist_pkgdata_DATA = \
	insecure.png \
	minisip.glade \
	minisip.png \
	noplay.png \
	norecord.png \
	play.png \
	record.png \
	secure.png \
	tray_icon.png

EXTRA_DIST = \
	minisip.png \
	minisip.desktop

if OSSO_SUPPORT
servicedir = $(libdir)/dbus-1.0/services
service_DATA = org.minisip.minisip.service
endif

MAINTAINERCLEANFILES = $(srcdir)/Makefile.in
