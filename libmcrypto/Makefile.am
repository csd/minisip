SUBDIRS = include m4

lib_LTLIBRARIES = libmcrypto.la
libmcrypto_la_CPPFLAGS = $(CPPFLAGS) -I../include/libmcrypto
libmcrypto_la_LDFLAGS = $(MINISIP_LIBRARY_LDFLAGS)

libmcrypto_la_SOURCES = source/certificate.cxx

MAINTAINERCLEANFILES = \
	$(srcdir)/configure \
	$(srcdir)/Makefile.in \
	$(srcdir)/aclocal.m4 \
	$(srcdir)/config/depcomp \
	$(srcdir)/config/config.sub \
	$(srcdir)/config/config.guess \
	$(srcdir)/config/install-sh \
	$(srcdir)/config/ltmain.sh \
	$(srcdir)/config/missing \
	$(srcdir)/depcomp \
	$(srcdir)/missing \
	$(srcdir)/install-sh \
	$(srcdir)/config.guess \
	$(srcdir)/config.sub \
	$(srcdir)/ltmain.sh

