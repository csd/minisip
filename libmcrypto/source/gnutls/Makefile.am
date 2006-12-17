noinst_LTLIBRARIES = \
		libmcgnutls.la

libmcgnutls_la_CPPFLAGS = $(MINISIP_CFLAGS)
libmcgnutls_la_SOURCES = \
		aes.cxx \
		cert.cxx \
		hmac256.cxx \
		hmac.cxx \
		init.cxx \
		OakleyDH.cxx \
		rand.cxx \
		sha256.cxx \
		TLSServerSocket.cxx \
		TLSSocket.cxx \
		ZrtpDH.cxx


libmcgnutls_la_LIBADD = $(GNUTLS_LIBS)

MAINTAINERCLEANFILES = \
		$(srcdir)/Makefile.in
