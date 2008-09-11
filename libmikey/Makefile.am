pkgconfigdir = $(libdir)/pkgconfig

SUBDIRS = include m4 win32 . tests debian
DIST_SUBDIRS = $(SUBDIRS)
EXTRA_DIST = libmikey.spec

pkgconfig_DATA = libmikey.pc

lib_LTLIBRARIES = libmikey.la
libmikey_la_CXXFLAGS = $(MINISIP_CFLAGS)
libmikey_la_LDFLAGS = $(MINISIP_LIBRARY_LDFLAGS)

libmikey_la_LIBADD =  $(MINISIP_LIBS)

libmikey_la_SOURCES = $(mikey_src) $(ka_src)

if OS_WIN
res_obj = win32/libmikey-res.o
libmikey_la_LDFLAGS += -Wl,$(res_obj)
libmikey_la_DEPENDENCIES = $(res_obj)
endif OS_WIN

mikey_src = \
                     mikey/Mikey.cxx \
                     mikey/MikeyException.cxx \
                     mikey/MikeyCsIdMap.cxx \
                     mikey/MikeyMessage.cxx \
                     mikey/MikeyMessageDH.h \
                     mikey/MikeyMessageDH.cxx \
                     mikey/MikeyMessagePSK.h \
                     mikey/MikeyMessagePSK.cxx \
                     mikey/MikeyMessageDHHMAC.h \
                     mikey/MikeyMessageDHHMAC.cxx \
                     mikey/MikeyMessagePKE.h \
                     mikey/MikeyMessagePKE.cxx \
                     mikey/MikeyMessageRSAR.h \
                     mikey/MikeyMessageRSAR.cxx \
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

ka_src =             keyagreement/KeyAgreement.cxx \
                     keyagreement/KeyAgreementDH.cxx \
	             keyagreement/KeyAgreementPSK.cxx \
	             keyagreement/KeyAgreementPKE.cxx \
	             keyagreement/KeyAgreementDHHMAC.cxx \
	             keyagreement/KeyAgreementRSAR.cxx \
		     keyagreement/KeyValidity.cxx

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
