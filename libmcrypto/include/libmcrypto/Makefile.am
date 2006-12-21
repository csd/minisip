BUILD_SUBDIRS=
if HAVE_GNUTLS
BUILD_SUBDIRS += gnutls
endif
if HAVE_OPENSSL
BUILD_SUBDIRS += openssl
endif
SUBDIRS = $(BUILD_SUBDIRS)
DIST_SUBDIRS = gnutls openssl


scsim_src =
if ENABLE_SCSIM
scsim_src += SipSim.h \
	     SipSimPk.h \
	     SipSimSoft.h \
	     SipSimDh.h \
	     SmartCard.h \
	     SipSimSmartCardGD.h \
	     SmartCardException.h
endif


pkginclude_HEADERS = \
		aes.h \
		cert.h \
		init.h \
		$(scsim_src) \
		base64.h \
		hmac.h \
		OakleyDH.h \
		rand.h \
		uuid.h \
		config.h \
		TLSException.h \
		TLSServerSocket.h \
		TLSSocket.h \
		ZrtpDH.h \
		$(OTHER_FILES)

OTHER_FILES =

if HAVE_EVP_SHA256
OTHER_FILES += hmac256.h sha256.h
endif

noinst_HEADERS = \
		rijndael-alg-fst.h

MAINTAINERCLEANFILES = $(srcdir)/Makefile.in
