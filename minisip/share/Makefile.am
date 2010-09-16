dist_pkgdata_DATA = \
	insecure.png \
	FreeSans.ttf \
	minisip.glade \
	minisip.png \
	icon_gnome.png \
	noplay.png \
	norecord.png \
	play.png \
	record.png \
	secure.png \
	tray_icon.png \
	exit.raw \
	hangup.raw \
	invite.raw \
	no.raw \
	person.raw \
	rightarrow.raw \
	settings.raw \
	yes.raw

EXTRA_DIST = \
	minisip.png \
	minisip.desktop

if OSSO_SUPPORT
servicedir = $(libdir)/dbus-1.0/services
service_DATA = org.minisip.minisip.service
endif

MAINTAINERCLEANFILES = $(srcdir)/Makefile.in
