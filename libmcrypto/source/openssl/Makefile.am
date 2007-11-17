noinst_LTLIBRARIES = \
		libmcopenssl.la

OTHER_FILES =

if HAVE_EVP_SHA256
OTHER_FILES += hmac256.cxx sha256.cxx
endif

if HAVE_OPENSSL_DTLS
OTHER_FILES += DtlsSocket.cxx DtlsSocket.h
endif

libmcopenssl_la_CPPFLAGS = $(MINISIP_CFLAGS)
libmcopenssl_la_SOURCES = \
		init.cxx \
		aes.cxx \
		cert.cxx \
		OakleyDH.cxx \
		ZrtpDH.cxx \
		hmac.cxx \
		rand.cxx \
		sha1.cxx \
		TlsException.cxx \
		TlsServerSocket.cxx \
		TlsSocket.cxx \
		$(OTHER_FILES)

libmcopenssl_la_LIBADD = $(OPENSSL_LIBS)

MAINTAINERCLEANFILES = \
		$(srcdir)/Makefile.in
