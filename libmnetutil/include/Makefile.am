ipv6_headers = 
if HAVE_IPV6
ipv6_headers += \
			libmnetutil/IP6Address.h \
			libmnetutil/IP6ServerSocket.h
endif

pkginclude_HEADERS =    libmnetutil/IP4Address.h \
			libmnetutil/IP4ServerSocket.h \
			$(ipv6_headers) \
			libmnetutil/IPAddress.h \
			libmnetutil/NetworkException.h \
			libmnetutil/NetworkFunctions.h \
			libmnetutil/ServerSocket.h \
			libmnetutil/TLSServerSocket.h \
			libmnetutil/Socket.h \
			libmnetutil/StreamSocket.h \
			libmnetutil/TCPSocket.h \
			libmnetutil/TLSSocket.h \
			libmnetutil/NsLookup.h \
			libmnetutil/NetUtil.h \
			libmnetutil/UDPSocket.h \
			libmnetutil/libmnetutil_config.h

noinst_HEADERS = 	config.h

MAINTAINERCLEANFILES =	$(srcdir)/Makefile.in \
			$(srcdir)/compilation_config.h.in \
			$(srcdir)/compilation_config.h.in~
