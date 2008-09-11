SUBDIRS = \
	win32 \
	include \
	m4 \
	debian \
	source \
	. \
	tests
#DIST_SUBDIRS = $(SUBDIRS) debian

# 
# start of libmcrypto.la rules
lib_LTLIBRARIES = libmcrypto.la

BUILD_SRC =
# if OS_WIN
# BUILD_SRC += libmcrypto.cpp
# endif OS_WIN

libmcrypto_la_SOURCES = $(BUILD_SRC)
libmcrypto_la_CPPFLAGS = $(MINISIP_CFLAGS)
libmcrypto_la_LDFLAGS = $(MINISIP_LIBRARY_LDFLAGS) 
libmcrypto_la_LIBADD = \
	source/libmcrypto_core.la \
	source/libmcrypto_imp.la \
	$(MINISIP_LIBS)
libmcrypto_la_LINK = $(LIBTOOL) --tag=CXX --mode=link $(CXX)	\
	$(AM_CXXFLAGS) $(CXXFLAGS) $(AM_LDFLAGS) $(LDFLAGS) -o $@


if OS_WIN
res_obj = win32/$(PACKAGE)-res.o
libmcrypto_la_LDFLAGS += -Wl,$(res_obj)
libmcrypto_la_DEPENDENCIES = $(res_obj)
endif OS_WIN

# end of libmcrypto.la rules
#

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libmcrypto.pc

# maintainer rules
ACLOCAL_AMFLAGS = -I m4 ${ACLOCAL_FLAGS}
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

dist-hook:
	cp -a version $(distdir)/.version
