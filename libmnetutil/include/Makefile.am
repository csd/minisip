ldap_src =
if ENABLE_LDAP
ldap_src += \
			libmnetutil/LdapConnection.h \
			libmnetutil/LdapCredentials.h \
			libmnetutil/LdapDirectoryLocator.h \
			libmnetutil/LdapEntry.h \
			libmnetutil/LdapException.h \
			libmnetutil/LdapUrl.h \
 			libmnetutil/LdapDownloader.h
endif

pkginclude_HEADERS =    \
			$(ldap_src) \
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
			libmnetutil/libmnetutil_config.h \
			libmnetutil/Downloader.h \
			libmnetutil/HttpDownloader.h \
			libmnetutil/FileUrl.h \
			libmnetutil/FileDownloaderException.h \
			libmnetutil/FileDownloader.h \
			libmnetutil/DirectorySet.h \
			libmnetutil/DirectorySetItem.h

noinst_HEADERS = 	\
			config.h \
			libmnetutil/IP4Address.h \
			libmnetutil/IP4ServerSocket.h \
			libmnetutil/IP6Address.h \
			libmnetutil/IP6ServerSocket.h

MAINTAINERCLEANFILES =	$(srcdir)/Makefile.in \
			$(srcdir)/compilation_config.h.in \
			$(srcdir)/compilation_config.h.in~
