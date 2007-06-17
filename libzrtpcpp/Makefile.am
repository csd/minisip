# Copyright (C) 2006 Werner Dittmann.
pkgconfigdir = $(libdir)/pkgconfig

#EXTRA_DIST = libzrtpcpp.spec

AM_CPPFLAGS = -I$(top_srcdir)/src $(MINISIP_CFLAGS)

pkgconfig_DATA = libzrtpcpp.pc


lib_LTLIBRARIES = libzrtpcpp.la
libzrtpcpp_la_LDFLAGS = $(MINISIP_LIBRARY_LDFLAGS)
libzrtpcpp_la_LIBADD = $(MINISIP_LIBS)

# http://www.gnu.org/software/libtool/manual.html

# if ZRTP_OPENSSL
# zrtp_src_o =    src/libzrtpcpp/crypto/openssl/ZrtpDH.cxx \
# 		src/libzrtpcpp/crypto/openssl/hmac256.cxx \
# 		src/libzrtpcpp/crypto/openssl/sha256.cxx \
# 		src/libzrtpcpp/crypto/openssl/InitializeOpenSSL.cxx
# endif

# if FALSE
# if ZRTP_GCRYPT
# zrtp_src_g =	src/libzrtpcpp/crypto/gcrypt/gcryptZrtpDH.cxx \
# 		src/libzrtpcpp/crypto/gcrypt/gcrypthmac256.cxx \
# 		src/libzrtpcpp/crypto/gcrypt/gcryptsha256.cxx \
# 		src/libzrtpcpp/crypto/gcrypt/InitializeGcrypt.cxx
# endif
# endif



libzrtpcpp_la_SOURCES = src/ZIDFile.cxx \
		src/ZIDRecord.cxx \
		src/ZRtp.cxx \
		src/ZrtpPacketCommit.cxx \
		src/ZrtpPacketConf2Ack.cxx \
		src/ZrtpPacketConfirm.cxx \
		src/ZrtpPacketDHPart.cxx \
		src/ZrtpPacketGoClear.cxx \
		src/ZrtpPacketClearAck.cxx \
		src/ZrtpPacketError.cxx \
		src/ZrtpPacketHelloAck.cxx \
		src/ZrtpPacketHello.cxx \
		src/ZrtpStateClass.cxx \
		src/ZrtpTextData.cxx \
		src/Base32.cxx $(zrtp_src_g) $(zrtp_src_o)

SUBDIRS = m4 src
DIST_SUBDIRS = $(SUBDIRS)
