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
scsim_src += SipSimPk.h \
	     SipSimDh.h \
	     SmartCard.h \
	     SipSimSmartCardGD.h \
	     GDEnum.h \
	     SmartCardException.h
endif

srp_src =
if ENABLE_SRP
srp_src += TlsSrpSocket.h
endif



pkginclude_HEADERS = \
		aes.h \
		DtlsSocket.h \
		SipSim.h \
		SipSimSoft.h \
		cert.h \
		init.h \
		$(scsim_src) \
		base64.h \
		$(srp_src) \
		hmac.h \
		OakleyDH.h \
		rand.h \
		sha1.h \
		uuid.h \
		config.h \
		TlsException.h \
		TlsServerSocket.h \
		TlsSocket.h \
		ZrtpDH.h \
		$(OTHER_FILES)

OTHER_FILES = 	CacheManager.h \
		CertificateFinder.h \
		CertificatePathFinderUcd.h

if HAVE_EVP_SHA256
OTHER_FILES += hmac256.h sha256.h
endif

noinst_HEADERS = \
		rijndael-alg-fst.h

MAINTAINERCLEANFILES = $(srcdir)/Makefile.in
