pkgconfigdir = $(libdir)/pkgconfig

SUBDIRS = include m4 win32 . tests
DIST_SUBDIRS = $(SUBDIRS)
EXTRA_DIST = libmikey.spec

pkgconfig_DATA = libmikey.pc

lib_LTLIBRARIES = libmikey.la
libmikey_la_CXXFLAGS = $(MINISIP_CFLAGS)
libmikey_la_LDFLAGS = $(MINISIP_LIBRARY_LDFLAGS)

libmikey_la_LIBADD =  $(MINISIP_LIBS)

libmikey_la_SOURCES = $(mikey_src) $(ka_src)

if HAVE_OPENSSL
libmikey_la_SOURCES += $(ossl_src)
libmikey_la_LIBADD += $(OPENSSL_LIBS)
endif HAVE_OPENSSL

if OS_WIN
res_obj = win32/libmikey-res.o
libmikey_la_LDFLAGS += -Wl,$(res_obj)
libmikey_la_DEPENDENCIES = $(res_obj)
endif OS_WIN

mikey_src =          mikey/MikeyException.cxx \
                     mikey/MikeyCsIdMap.cxx \
                     mikey/MikeyMessage.cxx \
                     mikey/MikeyMessageDH.h \
                     mikey/MikeyMessageDH.cxx \
                     mikey/MikeyMessagePSK.h \
                     mikey/MikeyMessagePSK.cxx \
                     mikey/MikeyPayloadCERT.cxx \
                     mikey/MikeyPayloadCHASH.cxx \
                     mikey/MikeyPayload.cxx \
                     mikey/MikeyPayloadDH.cxx \
                     mikey/MikeyPayloadERR.cxx \
                     mikey/MikeyPayloadGeneralExtension.cxx \
                     mikey/MikeyPayloadHDR.cxx \
                     mikey/MikeyPayloadID.cxx \
                     mikey/MikeyPayloadKEMAC.cxx \
                     mikey/MikeyPayloadKeyData.cxx \
                     mikey/MikeyPayloadPKE.cxx \
                     mikey/MikeyPayloadRAND.cxx \
                     mikey/MikeyPayloadSIGN.cxx \
                     mikey/MikeyPayloadSP.cxx \
		     mikey/MikeyPayloadT.cxx \
		     mikey/MikeyPayloadV.cxx

ka_src =             keyagreement/keyagreement.cxx \
                     keyagreement/keyagreement_dh.cxx \
	             keyagreement/keyagreement_psk.cxx \
	             keyagreement/KeyAgreementPKE.cxx \
		     keyagreement/keyvalidity.cxx

ossl_src = \
		mikey/MikeyMessagePKE.h \
		mikey/MikeyMessagePKE.cxx

# maintainer rules
ACLOCAL_AMFLAGS = -I m4 ${ACLOCAL_FLAGS}
MAINTAINERCLEANFILES = \
		$(srcdir)/Makefile.in \
		$(srcdir)/configure \
		$(srcdir)/aclocal.m4 \
		$(srcdir)/config/depcomp \
		$(srcdir)/config/config.sub \
		$(srcdir)/config/config.guess \
		$(srcdir)/config/install-sh \
		$(srcdir)/config/ltmain.sh \
		$(srcdir)/config/missing \
		$(srcdir)/depcomp \
		$(srcdir)/config.sub \
		$(srcdir)/config.guess \
		$(srcdir)/install-sh \
		$(srcdir)/ltmain.sh \
		$(srcdir)/missing


dist-hook:
	cp -a version $(distdir)/.version
