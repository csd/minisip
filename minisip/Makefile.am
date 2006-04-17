CPPFLAGS = @CPPFLAGS@ -DDATADIR="$(datadir)" -DPLUGIN_PATH="$(pkglibdir)"
LDFLAGS = @LDFLAGS@

EXTRA_DIST = minisip-libavcodec.patch minisip.spec

if HILDON_SUPPORT
	desktopdir = $(datadir)/applications/hildon
else
	desktopdir = $(datadir)/applications
endif

SUBDIRS = include share minisip debian

dist-hook:
	for i in test_cert Documentation; do \
		cp -a $(srcdir)/$$i $(distdir)/$$i; \
		find $(distdir)/$$i -name '.svn' -a -type d | xargs -r rm -r; \
	done

MAINTAINERCLEANFILES = \
	$(srcdir)/configure \
	$(srcdir)/Makefile.in \
	$(srcdir)/depcomp \
	$(srcdir)/aclocal.m4 \
	$(srcdir)/missing \
	$(srcdir)/install-sh \
	$(srcdir)/config.guess \
	$(srcdir)/config.sub \
	$(srcdir)/ltmain.sh

