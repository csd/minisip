abs_top_srcdir = @abs_top_srcdir@

SUBDIRS = include
DIST_SUBDIRS = $(SUBDIRS) debian
EXTRA_DIST = libmnetutil.spec

lib_LTLIBRARIES = libmnetutil.la
libmnetutil_la_CPPFLAGS = -I$(abs_top_srcdir)/include

libmnetutil_la_SOURCES = $(mnetutil_src)

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

