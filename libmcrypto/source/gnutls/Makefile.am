noinst_LTLIBRARIES = \
		libmcgnutls.la

srp_src =
if ENABLE_SRP
srp_src += TlsSrpSocket.cxx
srp_libs = $(GNUTLS_EXTRA_LIBS)
endif


libmcgnutls_la_CPPFLAGS = $(MINISIP_CFLAGS)
libmcgnutls_la_SOURCES = \
		aes.cxx \
		cert.cxx \
		hmac256.cxx \
		hmac.cxx \
		init.cxx \
		OakleyDH.cxx \
		rand.cxx \
		sha1.cxx \
		sha256.cxx \
		TlsServerSocket.cxx \
		TlsSocket.cxx \
		$(srp_src) \
		ZrtpDH.cxx


libmcgnutls_la_LIBADD = $(GNUTLS_LIBS) $(srp_libs)

MAINTAINERCLEANFILES = \
		$(srcdir)/Makefile.in
