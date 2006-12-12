BUILD_DIRS =
BUILD_LIBS = uuid/libuuid.la
if HAVE_OPENSSL
BUILD_DIRS += openssl
BUILD_LIBS += openssl/libmcopenssl.la
else
if HAVE_GNUTLS
BUILD_DIRS += gnutls
BUILD_LIBS += gnutls/libmcgnutls.la
endif
endif

SUBDIRS = $(BUILD_DIRS) uuid .
DIST_SUBDIRS = openssl gnutls uuid

AM_CPPFLAGS = $(MINISIP_CFLAGS)
noinst_LTLIBRARIES = \
		libmcrypto_core.la \
		libmcrypto_imp.la

scsim_src =
if ENABLE_SCSIM
scsim_src += SipSim.cxx \
	     SipSimPk.cxx \
	     SipSimDh.cxx \
	     SmartCard.cxx \
	     SipSimSmartCardGD.cxx \
	     SmartCardException.cxx
endif


libmcrypto_core_la_SOURCES = \
		init.cxx \
		$(scsim_src) \
		base64.cxx \
		uuid.cxx \
		rijndael-alg-fst.cxx

libmcrypto_imp_la_SOURCES = # none
libmcrypto_imp_la_LIBADD = $(BUILD_LIBS)

MAINTAINERCLEANFILES = $(srcdir)/Makefile.in
