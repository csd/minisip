pkgconfigdir = $(libdir)/pkgconfig

SUBDIRS = win32 . examples include m4 tests
DIST_SUBDIRS = $(SUBDIRS)
EXTRA_DIST = libmnetutil.spec

pkgconfig_DATA = libmnetutil.pc

lib_LTLIBRARIES = libmnetutil.la
libmnetutil_la_CPPFLAGS = $(MINISIP_CFLAGS)
libmnetutil_la_LDFLAGS = $(MINISIP_LIBRARY_LDFLAGS)
libmnetutil_la_LIBADD = $(MINISIP_LIBS)

libmnetutil_la_SOURCES = $(mnetutil_src)

if OS_WIN
res_obj = win32/libmnetutil-res.o
libmnetutil_la_LDFLAGS += -Wl,$(res_obj)
libmnetutil_la_DEPENDENCIES = $(res_obj)
endif OS_WIN

ipv6_src =
if HAVE_IPV6
ipv6_src += \
		    source/IP6Address.cxx \
		    source/IP6ServerSocket.cxx
endif

mnetutil_src =      source/IPAddress.cxx \
		    source/IP4Address.cxx \
		    source/IP4ServerSocket.cxx \
		    $(ipv6_src) \
		    source/NetworkException.cxx \
		    source/NetworkFunctions.cxx \
		    source/ServerSocket.cxx \
		    source/TLSServerSocket.cxx \
		    source/Socket.cxx \
		    source/TCPSocket.cxx \
		    source/TLSSocket.cxx \
		    source/NetUtil.cxx \
		    source/UDPSocket.cxx \
		    source/DatagramSocket.cxx \
		    source/StreamSocket.cxx

# maintainer rules
ACLOCAL_AMFLAGS = -I m4 ${ACLOCAL_FLAGS}
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
