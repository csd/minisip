noinst_LTLIBRARIES = \
		libmcopenssl.la

OTHER_FILES =

if HAVE_EVP_SHA256
OTHER_FILES += hmac256.cxx
endif

libmcopenssl_la_CPPFLAGS = $(MINISIP_CFLAGS)
libmcopenssl_la_SOURCES = \
		init.cxx \
		aes.cxx \
		cert.cxx \
		DH.cxx \
		hmac.cxx \
		$(OTHER_FILES)

libmcopenssl_la_LIBADD = $(OPENSSL_LIBS)

MAINTAINERCLEANFILES = \
		$(srcdir)/Makefile.in
