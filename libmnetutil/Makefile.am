pkgconfigdir = $(libdir)/pkgconfig

SUBDIRS = udns win32 . examples include m4 tests debian
DIST_SUBDIRS = $(SUBDIRS)
EXTRA_DIST = libmnetutil.spec

AM_CPPFLAGS = $(MINISIP_CFLAGS) -I$(srcdir)/include -I$(srcdir)/udns/source

pkgconfig_DATA = libmnetutil.pc

lib_LTLIBRARIES = libmnetutil.la
# libmnetutil_la_CPPFLAGS = 
libmnetutil_la_LDFLAGS = $(MINISIP_LIBRARY_LDFLAGS)
libmnetutil_la_LIBADD = udns/libudns.la  $(LTLIBOBJS) $(MINISIP_LIBS) $(LDAP_LIBS)
libmnetutil_la_DEPENDENCIES = $(LTLIBOBJS)

libmnetutil_la_SOURCES = $(mnetutil_src)

if OS_WIN
res_obj = win32/libmnetutil-res.o
libmnetutil_la_LDFLAGS += -Wl,$(res_obj)
libmnetutil_la_DEPENDENCIES += $(res_obj)
win32_src = \
		source/NetworkFunctionsWin32.h \
		source/NetworkFunctionsWin32.cxx
endif OS_WIN

ipv6_src =
if HAVE_IPV6
ipv6_src += \
		    source/IP6Address.cxx
endif

sctp_src=
if ENABLE_SCTP
sctp_src +=	    source/SctpSocket.cxx \
		    source/SctpServerSocket.cxx
endif ENABLE_SCTP

ldap_src = \
		    source/LdapConnection.cxx \
                    source/LdapEntry.cxx \
                    source/LdapException.cxx \
		    source/LdapUrl.cxx \
		    source/LdapDownloader.cxx

mnetutil_src = \
		    source/init.cxx \
		    source/DnsNaptr.cxx \
		    source/IPAddress.cxx \
		    source/IP4Address.cxx \
		    $(ipv6_src) \
		    $(ldap_src) \
		    source/NetworkException.cxx \
		    source/NetworkFunctions.cxx \
		    source/ServerSocket.cxx \
		    source/Socket.cxx \
		    source/SocketServer.cxx \
		    source/TCPSocket.cxx \
		    source/TcpServerSocket.cxx \
		    $(sctp_src) \
		    source/NetUtil.cxx \
		    source/UDPSocket.cxx \
		    source/DatagramSocket.cxx \
		    $(win32_src) \
		    source/StreamSocket.cxx \
		    source/Downloader.cxx \
		    source/HttpDownloader.cxx \
		    source/FileUrl.cxx \
		    source/FileDownloaderException.cxx \
		    source/FileDownloader.cxx \
		    source/HttpFileSystem.cxx \
		    source/DirectorySet.cxx \
		    source/DirectorySetItem.cxx

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

dist-hook:
	cp -a version $(distdir)/.version
