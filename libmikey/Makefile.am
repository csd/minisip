pkgconfigdir = $(libdir)/pkgconfig

SUBDIRS = include win32
DIST_SUBDIRS = $(SUBDIRS) debian
EXTRA_DIST = libmikey.spec

pkgconfig_DATA = libmikey.pc

lib_LTLIBRARIES = libmikey.la
libmikey_la_CPPFLAGS = -I$(top_srcdir)/include
libmikey_la_CXXFLAGS = $(MUTIL_CFLAGS)
libmikey_la_LDFLAGS = -no-undefined -version-info $(LT_VERSION_INFO)
libmikey_la_LIBADD =  $(MUTIL_LIBS)

libmikey_la_SOURCES = $(mikey_src) $(ka_src)

if OS_WIN
res_obj = win32/libmikey-res.o
libmikey_la_LDFLAGS += -Wl,$(res_obj)
libmikey_la_DEPENDENCIES = $(res_obj)
endif OS_WIN

mikey_src =          mikey/MikeyException.cxx \
                     mikey/MikeyCsIdMap.cxx \
                     mikey/MikeyMessage.cxx \
                     mikey/MikeyMessageDH.cxx \
                     mikey/MikeyMessagePKE.cxx \
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
