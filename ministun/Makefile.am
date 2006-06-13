SUBDIRS = include win32 source
DIST_SUBDIRS = m4 $(SUBDIRS)

#EXTRA_DIST = ministun.spec

#pkgconfigdir = $(libdir)/pkgconfig
#pkgconfig_DATA = ministun.pc

ACLOCAL_AMFLAGS = -I m4 ${ACLOCAL_FLAGS}
MAINTAINERCLEANFILES = \
	$(srcdir)/aclocal.m4 \
	$(srcdir)/configure \
	$(srcdir)/Makefile.in \
	$(srcdir)/INSTALL \
	$(srcdir)/config/depcomp \
	$(srcdir)/config/config.sub \
	$(srcdir)/config/config.guess \
	$(srcdir)/config/install-sh \
	$(srcdir)/config/ltmain.sh \
	$(srcdir)/config/missing

dist-hook:
	cp -a version $(distdir)/.version
