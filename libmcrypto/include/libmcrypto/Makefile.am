BUILD_SUBDIRS=
if HAVE_GNUTLS
BUILD_SUBDIRS += gnutls
endif
if HAVE_OPENSSL
BUILD_SUBDIRS += openssl
endif
SUBDIRS = $(BUILD_SUBDIRS)
DIST_SUBDIRS = gnutls openssl

pkginclude_HEADERS = \
		base64.h \
		config.h

noinst_HEADERS = \
		aes.h \
		cert.h \
		hmac.h \
		rijndael-alg-fst.h

MAINTAINERCLEANFILES = $(srcdir)/Makefile.in
