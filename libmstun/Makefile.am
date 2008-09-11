SUBDIRS = include m4 win32 source debian . 
#DIST_SUBDIRS = $(SUBDIRS) debian

#EXTRA_DIST = libmstun.spec

# 
# start of libmcrypto.la rules
lib_LTLIBRARIES = libmstun.la


libmstun_la_SOURCES = # no sources; libraries only
libmstun_la_CPPFLAGS = $(MINISIP_CFLAGS)
libmstun_la_LDFLAGS = $(MINISIP_LIBRARY_LDFLAGS) 

#libmstun_la_LIBADD = source/libmstunc.la source/libmstund.la $(MINISIP_LIBS)
libmstun_la_LIBADD = source/libmstunc.la $(MINISIP_LIBS)

libmstun_la_LINK = $(LIBTOOL) --tag=CXX --mode=link $(CXX) \
	$(AM_CXXFLAGS) $(CXXFLAGS) $(AM_LDFLAGS) $(LDFLAGS) -o $@

if OS_WIN
res_obj = win32/$(PACKAGE)-res.o
libmstun_la_LDFLAGS += -Wl,$(res_obj)
libmstun_la_DEPENDENCIES = $(res_obj)
endif OS_WIN

# end of libmcrypto.la rules
#

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libmstun.pc

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
