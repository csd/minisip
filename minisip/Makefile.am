CPPFLAGS = @CPPFLAGS@ -DDATADIR="$(datadir)" -DPLUGIN_PATH="$(pkglibdir)"
LDFLAGS = @LDFLAGS@

noinst_HEADERS = include/*.h 

dist_pkgdata_DATA = share/tray_icon.png share/secure.png share/insecure.png share/record.png share/norecord.png share/play.png share/noplay.png share/minisip.glade
EXTRA_DIST = test_cert/*.pem Documentation/* minisip-libavcodec.patch share/minisip.png share/minisip.desktop minisip.spec

if HILDON_SUPPORT
	desktopdir = $(datadir)/applications/hildon
else
	desktopdir = $(datadir)/applications
endif

if OSSO_SUPPORT
	servicedir = $(libdir)/dbus-1.0/services
	service_DATA = share/org.minisip.minisip.service
endif

SUBDIRS = minisip
DIST_SUBDIRS = debian
