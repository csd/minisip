srp_src =
if ENABLE_SRP
srp_src += libmnetutil/TlsSrpSocket.h
endif

pkginclude_HEADERS =    \
			$(srp_src) \
			libmnetutil/init.h \
			libmnetutil/DnsNaptr.h \
			libmnetutil/IPAddress.h \
			libmnetutil/NetworkException.h \
			libmnetutil/NetworkFunctions.h \
			libmnetutil/ServerSocket.h \
			libmnetutil/Socket.h \
			libmnetutil/SocketServer.h \
			libmnetutil/StreamSocket.h \
			libmnetutil/TCPSocket.h \
			libmnetutil/NetUtil.h \
			libmnetutil/UDPSocket.h \
			libmnetutil/DatagramSocket.h \
			libmnetutil/libmnetutil_config.h

noinst_HEADERS = 	\
			config.h \
			libmnetutil/IP4Address.h \
			libmnetutil/IP4ServerSocket.h \
			libmnetutil/IP6Address.h \
			libmnetutil/IP6ServerSocket.h

MAINTAINERCLEANFILES =	$(srcdir)/Makefile.in \
			$(srcdir)/compilation_config.h.in \
			$(srcdir)/compilation_config.h.in~
