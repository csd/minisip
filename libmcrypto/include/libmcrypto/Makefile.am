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
		init.h \
		base64.h \
		rand.h \
		uuid.h \
		config.h

OTHER_FILES =

if HAVE_EVP_SHA256
OTHER_FILES += hmac256.h sha256.h
endif

scsim_src =
if ENABLE_SCSIM
scsim_src += YOURHEADER.h
endif


noinst_HEADERS = \
		aes.h \
		$(scsim_src) \
		cert.h \
		ZrtpDH.h \
		hmac.h \
		rijndael-alg-fst.h \
		$(OTHER_FILES)

MAINTAINERCLEANFILES = $(srcdir)/Makefile.in
