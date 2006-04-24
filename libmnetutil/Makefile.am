abs_top_srcdir = @abs_top_srcdir@
pkgconfigdir = $(libdir)/pkgconfig

SUBDIRS = include m4 win32
DIST_SUBDIRS = $(SUBDIRS) debian
EXTRA_DIST = libmnetutil.spec

pkgconfig_DATA = libmnetutil.pc

lib_LTLIBRARIES = libmnetutil.la
libmnetutil_la_CPPFLAGS = -I$(abs_top_srcdir)/include
libmnetutil_la_CXXFLAGS = $(MUTIL_CFLAGS)
libmnetutil_la_LDFLAGS = -no-undefined -version-info $(LT_VERSION_INFO)
libmnetutil_la_LIBADD = $(MUTIL_LIBS)

libmnetutil_la_SOURCES = $(mnetutil_src)

if OS_WIN
res_obj = win32/libmnetutil-res.o
libmnetutil_la_LDFLAGS += -Wl,$(res_obj)
libmnetutil_la_DEPENDENCIES = $(res_obj)
endif OS_WIN

mnetutil_src =      source/IPAddress.cxx \
		    source/IP4Address.cxx \
		    source/IP4ServerSocket.cxx \
		    source/IP6Address.cxx \
		    source/IP6ServerSocket.cxx \
		    source/NetworkException.cxx \
		    source/NetworkFunctions.cxx \
		    source/ServerSocket.cxx \
		    source/TLSServerSocket.cxx \
		    source/Socket.cxx \
		    source/TCPSocket.cxx \
		    source/TLSSocket.cxx \
		    source/NsLookup.cxx \
		    source/NetUtil.cxx \
		    source/UDPSocket.cxx \
		    source/StreamSocket.cxx

MAINTAINERCLEANFILES = \
		$(srcdir)/Makefile.in \
		$(srcdir)/aclocal.m4 \
		$(srcdir)/configure \
		$(srcdir)/config/depcomp \
		$(srcdir)/config/config.sub \
		$(srcdir)/config/config.guess \
		$(srcdir)/config/install-sh \
		$(srcdir)/config/ltmain.sh \
		$(srcdir)/config/missing \
		$(srcdir)/depcomp \
		$(srcdir)/config.sub \
		$(srcdir)/config.guess \
		$(srcdir)/install-sh \
		$(srcdir)/ltmain.sh \
		$(srcdir)/missing
