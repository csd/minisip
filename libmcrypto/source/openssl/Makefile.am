noinst_LTLIBRARIES = \
		libmcopenssl.la

libmcopenssl_la_CPPFLAGS = $(MINISIP_CFLAGS)
libmcopenssl_la_SOURCES = \
		init.cxx \
		aes.cxx \
		DH.cxx \
		cert.cxx \
		hmac.cxx

libmcopenssl_la_LIBADD = $(OPENSSL_LIBS)

MAINTAINERCLEANFILES = \
		$(srcdir)/Makefile.in
