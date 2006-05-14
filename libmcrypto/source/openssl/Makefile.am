noinst_LTLIBRARIES = \
		libmcopenssl.la

libmcopenssl_la_CPPFLAGS = $(MINISIP_CFLAGS)
libmcopenssl_la_SOURCES = \
		aes.cxx \
		cert.cxx \
		hmac.cxx \
		DH.cxx \
		initOpenSSL.cxx	

libmcopenssl_la_LIBADD = $(OPENSSL_LIBS)

MAINTAINERCLEANFILES = \
		$(srcdir)/Makefile.in
