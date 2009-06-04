pkgconfigdir = $(libdir)/pkgconfig

SUBDIRS = include m4 win32 . tests debian
DIST_SUBDIRS = $(SUBDIRS)
EXTRA_DIST = libmsip.spec

pkgconfig_DATA = libmsip.pc

lib_LTLIBRARIES = libmsip.la
libmsip_la_CPPFLAGS = $(MINISIP_CFLAGS)
libmsip_la_LDFLAGS = $(MINISIP_LIBRARY_LDFLAGS)
libmsip_la_LIBADD = $(MINISIP_LIBS)

libmsip_la_SOURCES = $(msip_src)

if OS_WIN
res_obj = win32/libmsip-res.o
libmsip_la_LDFLAGS += -Wl,$(res_obj)
libmsip_la_DEPENDENCIES = $(res_obj)
endif OS_WIN

if HAVE_DTLS
msip_dtls_src =      source/transports/SipTransportDtlsUdp.h \
		     source/transports/SipTransportDtlsUdp.cxx
endif HAVE_DTLS

if HAVE_SCTP
msip_sctp_src =	     source/transports/SipTransportSctp.h \
		     source/transports/SipTransportSctp.cxx \
		     source/transports/SipTransportTlsSctp.h \
		     source/transports/SipTransportTlsSctp.cxx
endif HAVE_SCTP

msip_src =           source/SipDialogConfig.cxx \
		     source/SipAuthenticationDigest.cxx \
		     source/SipStack.cxx \
		     source/SipStackInternal.h \
		     source/SipStackInternal.cxx \
		     source/SipLayerDialog.h \
		     source/SipLayerDialog.cxx \
		     source/SipLayerTransaction.h \
		     source/SipLayerTransaction.cxx \
                     source/messages/SipMessage.cxx \
                     source/messages/SipResponse.cxx \
                     source/messages/SipRequest.cxx \
		     source/SipUtils.cxx \
		     source/SipCommandDispatcher.h \
		     source/SipCommandDispatcher.cxx \
		     source/SipLayerTransport.h \
		     source/SipLayerTransport.cxx \
		     source/SipSocketServer.cxx \
		     source/SipSMCommand.cxx \
		     source/SipCommandString.cxx \
		     source/SipTimers.cxx \
		     source/SipMessageContentUnknown.cxx \
		     source/SipMessageContentIM.cxx \
		     source/SipMessageContentFactory.cxx \
		     source/SipMessageContentMime.cxx \
		     source/transactions/SipTransaction.h \
                     source/transactions/SipTransaction.cxx \
		     source/transactions/SipTransactionNonInviteClient.h \
		     source/transactions/SipTransactionNonInviteClient.cxx \
		     source/transactions/SipTransactionNonInviteServer.h \
		     source/transactions/SipTransactionNonInviteServer.cxx \
		     source/transactions/SipTransactionInviteClient.h \
		     source/transactions/SipTransactionInviteClient.cxx \
		     source/transactions/SipTransactionInviteServer.h \
		     source/transactions/SipTransactionInviteServer.cxx \
		     source/transactions/SipTransactionInviteServerUA.h \
		     source/transactions/SipTransactionInviteServerUA.cxx \
		     source/transports/SipTransport.cxx \
		     source/transports/SipTransportUdp.h \
		     source/transports/SipTransportUdp.cxx \
		     source/transports/SipTransportTcp.h \
		     source/transports/SipTransportTcp.cxx \
		     source/transports/SipTransportTls.h \
		     source/transports/SipTransportTls.cxx \
		     $(msip_dtls_src) \
		     $(msip_sctp_src) \
                     source/dialogs/SipTransitionUtils.cxx \
		     source/dialogs/SipDialog.cxx \
		     source/dialogs/SipDialogRegister.cxx \
		     source/dialogs/SipDialogManagement.cxx \
		     source/headers/SipHeader.cxx \
		     source/headers/SipHeaderAccept.cxx \
                     source/headers/SipHeaderAllow.cxx \
                     source/headers/SipHeaderAllowEvents.cxx \
                     source/headers/SipHeaderAuthorization.cxx \
                     source/headers/SipHeaderCSeq.cxx \
                     source/headers/SipHeaderCallID.cxx \
                     source/headers/SipHeaderContact.cxx \
                     source/headers/SipHeaderContentLength.cxx \
                     source/headers/SipHeaderContentType.cxx \
                     source/headers/SipHeaderEvent.cxx \
                     source/headers/SipHeaderExpires.cxx \
                     source/headers/SipHeaderFrom.cxx \
                     source/headers/SipHeaderMaxForwards.cxx \
                     source/headers/SipHeaderProxyAuthenticate.cxx \
                     source/headers/SipHeaderWWWAuthenticate.cxx \
                     source/headers/SipHeaderProxyAuthorization.cxx \
                     source/headers/SipHeaderRAck.cxx \
                     source/headers/SipHeaderRSeq.cxx \
                     source/headers/SipHeaderRecordRoute.cxx \
                     source/headers/SipHeaderReferTo.cxx \
                     source/headers/SipHeaderRequire.cxx \
                     source/headers/SipHeaderRoute.cxx \
                     source/headers/SipHeaderString.cxx \
                     source/headers/SipHeaderSubject.cxx \
                     source/headers/SipHeaderSnakeSM.cxx \
                     source/headers/SipHeaderSubscriptionState.cxx \
                     source/headers/SipHeaderSupported.cxx \
                     source/headers/SipHeaderTo.cxx \
                     source/headers/SipHeaderUnknown.cxx \
                     source/headers/SipHeaderUnsupported.cxx \
                     source/headers/SipHeaderUserAgent.cxx \
                     source/headers/SipHeaderAcceptContact.cxx \
                     source/headers/SipHeaderVia.cxx \
                     source/headers/SipHeaderWarning.cxx

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
