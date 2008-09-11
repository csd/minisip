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
	cp -a version $(distdir)/.version

# maintainer rules
ACLOCAL_AMFLAGS = -I m4 ${ACLOCAL_FLAGS}
MAINTAINERCLEANFILES = \
	$(srcdir)/configure \
	$(srcdir)/Makefile.in \
	$(srcdir)/config/depcomp \
	$(srcdir)/config/config.sub \
	$(srcdir)/config/config.guess \
	$(srcdir)/config/install-sh \
	$(srcdir)/config/ltmain.sh \
	$(srcdir)/config/missing \
	$(srcdir)/depcomp \
	$(srcdir)/aclocal.m4 \
	$(srcdir)/missing \
	$(srcdir)/install-sh \
	$(srcdir)/config.guess \
	$(srcdir)/config.sub \
	$(srcdir)/ltmain.sh

