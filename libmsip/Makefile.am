SUBDIRS = include
DIST_SUBDIRS = $(SUBDIRS) debian

lib_LTLIBRARIES = libmsip.la
libmsip_la_CPPFLAGS = $(CPPFLAGS) -I../../include/libmsip

libmsip_la_SOURCES = $(msip_src)

msip_src =           source/SipDialogConfig.cxx \
		     source/SipUtils.cxx \
		     source/SipDialogContainer.cxx \
		     source/SipMessageDispatcher.cxx \
		     source/SipMessageTransport.cxx \
		     source/SipSMCommand.cxx \
		     source/SipCommandString.cxx \
		     source/SipURI.cxx \
		     source/SipTimers.cxx \
		     source/SipMessageContentIM.cxx \
		     source/SipMessageContentFactory.cxx \
                     source/transactions/SipTransactionUtils.cxx \
                     source/transactions/SipTransaction.cxx \
		     source/transactions/SipTransactionClient.cxx \
		     source/transactions/SipTransactionServer.cxx \
		     source/transactions/SipTransactionInviteClient.cxx \
		     source/transactions/SipTransactionInviteServer.cxx \
		     source/transactions/SipTransactionInviteClientUA.cxx \
		     source/transactions/SipTransactionInviteServerUA.cxx \
		     source/messages/SipAck.cxx \
		     source/messages/SipBye.cxx \
                     source/messages/SipCancel.cxx \
                     source/messages/SipInvite.cxx \
                     source/messages/SipNotify.cxx \
                     source/messages/SipMessage.cxx \
                     source/messages/SipIMMessage.cxx \
                     source/messages/SipRegister.cxx \
                     source/messages/SipResponse.cxx \
                     source/messages/SipSubscribe.cxx \
		     source/dialogs/SipDialog.cxx \
		     source/dialogs/SipDialogRegister.cxx \
		     source/headers/SipHeader.cxx \
		     source/headers/SipHeaderAccept.cxx \
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
                     source/headers/SipHeaderProxyAuthorization.cxx \
                     source/headers/SipHeaderRecordRoute.cxx \
                     source/headers/SipHeaderRoute.cxx \
                     source/headers/SipHeaderSubject.cxx \
                     source/headers/SipHeaderTo.cxx \
                     source/headers/SipHeaderUnsupported.cxx \
                     source/headers/SipHeaderUserAgent.cxx \
                     source/headers/SipHeaderAcceptContact.cxx \
                     source/headers/SipHeaderVia.cxx \
                     source/headers/SipHeaderWarning.cxx




                                                                                                       


