BUILD_DIRS =
BUILD_LIBS =
if HAVE_OPENSSL
BUILD_DIRS += openssl
BUILD_LIBS += openssl/libmcopenssl.la
endif
if HAVE_GNUTLS
BUILD_DIRS += gnutls
BUILD_LIBS += gnutls/libmcgnutls.la
endif

SUBDIRS = $(BUILD_DIRS) .
DIST_SUBDIRS = openssl gnutls

AM_CPPFLAGS = $(MINISIP_CFLAGS)
noinst_LTLIBRARIES = \
		libmcrypto_core.la \
		libmcrypto_imp.la

libmcrypto_core_la_SOURCES = \
		init.cxx \
		base64.cxx \
		rijndael-alg-fst.cxx

libmcrypto_imp_la_SOURCES = # none
libmcrypto_imp_la_LIBADD = $(BUILD_LIBS)

MAINTAINERCLEANFILES = $(srcdir)/Makefile.in
