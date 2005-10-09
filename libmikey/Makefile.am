abs_top_srcdir = @abs_top_srcdir@

SUBDIRS = include
DIST_SUBDIRS = $(SUBDIRS) debian
EXTRA_DIST = libmikey.spec

lib_LTLIBRARIES = libmikey.la
libmikey_la_CPPFLAGS = $(CPPFLAGS) -I$(abs_top_srcdir)/include/

libmikey_la_SOURCES = $(mikey_src) $(ka_src)

mikey_src =          mikey/MikeyException.cxx \
                     mikey/MikeyCsIdMap.cxx \
                     mikey/MikeyMessage.cxx \
                     mikey/MikeyMessageDH.cxx \
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
		     keyagreement/keyvalidity.cxx
