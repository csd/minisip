/*
  Copyright (C) 2006 Werner Dittmann

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */

/*
 * Authors: Werner Dittmann <Werner.Dittmann@t-online.de>
 */

#include <config.h>

#include <libminisip/media/zrtp/ZrtpHostBridgeMinisip.h>
#include <libzrtpcpp/ZIDFile.h>
#include <libzrtpcpp/ZrtpStateClass.h>

#include <libmikey/MikeyPayloadSP.h>
#include <libminisip/config/UserConfig.h>
#include <libmutil/CommandString.h>

#include <libminisip/media/MediaStream.h>

#ifdef ZRTP_SUPPORT

static MRef<TimeoutProvider<std::string, MRef<StateMachine<SipSMCommand,std::string>*> > *>staticTimeoutProvider;

int32_t ZrtpHostBridgeMinisip::initialize(MRef<TimeoutProvider<std::string,
                                          MRef<StateMachine<SipSMCommand,std::string>*> > *>tp,
                                          const char *zidFilename) {

    std::string fname;
    staticTimeoutProvider = tp;
    if (zidFilename == NULL) {
        fname = UserConfig::getFileName("minisip.zid");
        zidFilename = fname.c_str();
    }
    ZIDFile *zf = ZIDFile::getInstance();
    zf->open((char *)zidFilename);
    return 1;
}

ZrtpHostBridgeMinisip::ZrtpHostBridgeMinisip(std::string id, MRef<CommandReceiver*> callback):
        StateMachine<SipSMCommand, std::string>(staticTimeoutProvider),
        callId(id),
        messageRouterCallback(callback) {

    secureParts = 0;
    zrtpEngine = NULL;

    senderSecure = 0;
    receiverSecure = 0;

    receiverSsrc = 0;
    senderSsrc = 0;

    rStream = NULL;
    sStream = NULL;

    senderCryptoContext = NULL;
    senderZrtpSsrc = 0xdeadbeef;         // may be a different value (random) as well
    senderZrtpSeqNo = 1;

    recvCryptoContext = NULL;
}

ZrtpHostBridgeMinisip::~ZrtpHostBridgeMinisip() {

    cancelTimer();
    freeStateMachine();		// to clean up the TimeoutProvider
    delete zrtpEngine;
}

void ZrtpHostBridgeMinisip::setReceiver(MRef<RealtimeMediaStreamReceiver *> r) {
	rStream = r;
}

void ZrtpHostBridgeMinisip::setSender(MRef<RealtimeMediaStreamSender *> s) {
	sStream = s;
}

void ZrtpHostBridgeMinisip::start() {
    ZIDFile *zid = ZIDFile::getInstance();
    const uint8_t* ownZid = zid->getZid();

    if (zrtpEngine == NULL) {
        zrtpEngine = new ZRtp((uint8_t*)ownZid, (ZrtpCallback*)this);
        zrtpEngine->setClientId(clientId);
        zrtpEngine->startZrtpEngine();
    }
}

void ZrtpHostBridgeMinisip::stop() {
    zrtpEngine->stopZrtp();
    delete zrtpEngine;
}

bool ZrtpHostBridgeMinisip::isZrtpPacket(MRef<SRtpPacket *> packet) {
    unsigned char* extHeader = packet->getExtensionHeader();
    uint16_t magic = *((uint16_t*)extHeader);

    magic = ntoh16(magic);

    // If not a ZRTP packet - back to caller for further actions
    if (magic == ZRTP_EXT_PACKET) {
        return true;
    }
    return false;
}

int32_t ZrtpHostBridgeMinisip::processPacket(MRef<SRtpPacket *> packet) {

    unsigned char* extHeader = packet->getExtensionHeader();
    uint16_t magic = *((uint16_t*)extHeader);

    magic = ntoh16(magic);

    // If not a ZRTP packet - back to caller for further actions
    if (magic != ZRTP_EXT_PACKET) {
	return 1;
    }
    /*
     * It's a ZRTP packet, check if ZRTP already started. If not and no other
     * content return zero to dismiss packet, otherwise return 1 for further
     * actions. This can happen for "piggy-back" ZRTP packets.
     */
    if (zrtpEngine == NULL) {
        if (packet->getContentLength() > 0) {
            return  1;
        }
        else {
            return 0;
        }
    }
    recvZrtpSeqNo = packet->getHeader().getSeqNo();
    recvZrtpSsrc = packet->getHeader().getSSRC();

    if (zrtpEngine->handleGoClear(extHeader)) {
        return 0;
    }
    int ret = zrtpEngine->processExtensionHeader(extHeader, packet->getContent());

    // Fail is only a fail of the protocol state, already handled but
    // payload usually not affected - thus caller may process it, e.g.
    // in case of "piggy-back" ZRTP packets.
    return ((ret == Fail || ret == Done) ? 1 : 0);
}

bool ZrtpHostBridgeMinisip::isSecureState()
{
    return zrtpEngine->checkState(SecureState);
}

int32_t ZrtpHostBridgeMinisip::sendDataRTP(const unsigned char *data, int32_t length) {
    sStream->sendZrtp((unsigned char*)data, length, NULL, 0);
    return 1;
}

int32_t ZrtpHostBridgeMinisip::sendDataSRTP(const unsigned char *dataHeader, int32_t lengthHeader,
					    char *dataContent, int32_t lengthContent) {
    sStream->sendZrtp((unsigned char*)dataHeader, lengthHeader,
		       (unsigned char*)dataContent, lengthContent);
    return 1;
}

void ZrtpHostBridgeMinisip::srtpSecretsReady(SrtpSecret_t* secrets, EnableSecurity part) {

    MRef<CryptoContext *> pcc;

    if (part == ForSender) {
    // encrypting packets, intiator uses initiator keys, responder uses responders keys
	if (secrets->role == Initiator) {
            senderCryptoContext = new CryptoContext(
		    0,
                    0,
                    0,
                    0L,                                      // keydr << 48,
                    MIKEY_SRTP_EALG_AESCM,                   // encryption algo
                    MIKEY_SRTP_AALG_SHA1HMAC,                // authtication algo
                    (unsigned char*)secrets->keyInitiator,   // Master Key
                    secrets->initKeyLen / 8,                 // Master Key length
                    (unsigned char*)secrets->saltInitiator,  // Master Salt
                    secrets->initSaltLen / 8,                // Master Salt length
                    secrets->initKeyLen / 8,                 // encryption keyl
                    20,                                      // authentication key len
                    secrets->initSaltLen / 8,                // session salt len
                    1,
                    1,
                    secrets->srtpAuthTagLen / 8);            // authentication tag lenA
	}
        else {
            senderCryptoContext = new CryptoContext(
		    0,
                    0,
                    0,
                    0L,                                      // keydr << 48,
                    MIKEY_SRTP_EALG_AESCM,                   // encryption algo
                    MIKEY_SRTP_AALG_SHA1HMAC,                // authtication algo
                    (unsigned char*)secrets->keyResponder,   // Master Key
                    secrets->respKeyLen / 8,                 // Master Key length
                    (unsigned char*)secrets->saltResponder,  // Master Salt
                    secrets->respSaltLen / 8,                // Master Salt length
                    secrets->respKeyLen / 8,                 // encryption keyl
                    20,                                      // authentication key len
                    secrets->respSaltLen / 8,                // session salt len
                    1,
                    1,
                    secrets->srtpAuthTagLen / 8);            // authentication tag len
        }
        pcc = senderCryptoContext->newCryptoContextForSSRC(senderZrtpSsrc, 0, senderZrtpSeqNo, 0L);
        pcc->derive_srtp_keys(senderZrtpSeqNo);
        sStream->setKeyAgreementZrtp(pcc);

        // create a crypto context for real SSRC sender stream. Note: this
        // can be done at this point only if the key derivation rate is 0
        // (disabled) or greater 2^16. For ZRTP this is the case: the key
        // derivation is defined as 2^48 which is effectively 0.
        pcc = senderCryptoContext->newCryptoContextForSSRC(senderSsrc, 0, sStream->getSeqNo(), 0L);
        pcc->derive_srtp_keys(sStream->getSeqNo());
        sStream->setKeyAgreementZrtp(pcc);

        secureParts++;
    }
    if (part == ForReceiver) {
    // decrypting packets, intiator uses responder keys, responder initiator keys
	if (secrets->role == Initiator) {
            recvCryptoContext = new CryptoContext(
		    0,
                    0,
                    0,
                    0L,                                      // keydr << 48,
                    MIKEY_SRTP_EALG_AESCM,                   // encryption algo
                    MIKEY_SRTP_AALG_SHA1HMAC,                // authtication algo
                    (unsigned char*)secrets->keyResponder,   // Master Key
                    secrets->respKeyLen / 8,                 // Master Key length
                    (unsigned char*)secrets->saltResponder,  // Master Salt
                    secrets->respSaltLen / 8,                // Master Salt length
                    secrets->respKeyLen / 8,                 // encryption keyl
                    20,                                      // authentication key len
                    secrets->respSaltLen / 8,                // session salt len
                    1,
                    1,
                    secrets->srtpAuthTagLen / 8);            // authentication tag len
	}
	else {
            recvCryptoContext = new CryptoContext(
		    0,
                    0,
                    0,
                    0L,                                      // keydr << 48,
                    MIKEY_SRTP_EALG_AESCM,                   // encryption algo
                    MIKEY_SRTP_AALG_SHA1HMAC,                // authtication algo
                    (unsigned char*)secrets->keyInitiator,   // Master Key
                    secrets->initKeyLen / 8,                 // Master Key length
                    (unsigned char*)secrets->saltInitiator,  // Master Salt
                    secrets->initSaltLen / 8,                // Master Salt length
                    secrets->initKeyLen / 8,                 // encryption keyl
                    20,                                      // authentication key len
                    secrets->initSaltLen / 8,                // session salt len
                    1,
                    1,
                    secrets->srtpAuthTagLen / 8);            // authentication tag len
	}
        pcc = recvCryptoContext->newCryptoContextForSSRC(recvZrtpSsrc, 0, recvZrtpSeqNo, 0L);
        pcc->derive_srtp_keys(recvZrtpSeqNo);
        rStream->setKeyAgreementZrtp(pcc);

        secureParts++;
    }
}

MRef<CryptoContext *>
ZrtpHostBridgeMinisip::newCryptoContextForRecvSSRC(uint32_t ssrc, int roc,
                                                   uint16_t seq,
                                                   int64_t keyDerivRate)
{
    MRef<CryptoContext *> pcc;

    pcc = recvCryptoContext->newCryptoContextForSSRC(ssrc, roc, seq, keyDerivRate);
    pcc->derive_srtp_keys(seq);
    rStream->setKeyAgreementZrtp(pcc);
    return pcc;
}

void ZrtpHostBridgeMinisip::srtpSecretsOn(const char* c, const char* s)
{

    if (s != NULL) {
        CommandString cmd(callId, "zrtp_security_change", "secure", s);
        messageRouterCallback->handleCommand("gui", cmd);
    }
//    if (s != NULL && zrtpUserCallback != NULL) {
//        zrtpUserCallback->showSAS(s);
//    }
}

void ZrtpHostBridgeMinisip::srtpSecretsOff(EnableSecurity part) {
    MRef<CryptoContext *> cryptoContext;

    if (part == ForSender) {
	cryptoContext = new CryptoContext(senderSsrc);
	sStream->setKeyAgreementZrtp(cryptoContext);

        cryptoContext = new CryptoContext(senderZrtpSsrc);
        sStream->setKeyAgreementZrtp(cryptoContext);
        secureParts--;
    }
    if (part == ForReceiver) {
	cryptoContext = new CryptoContext(receiverSsrc);
	sStream->setKeyAgreementZrtp(cryptoContext);

        cryptoContext = new CryptoContext(recvZrtpSsrc);
        sStream->setKeyAgreementZrtp(cryptoContext);
        secureParts--;
    }

    CommandString cmd(callId, "zrtp_security_change", "insecure");
    messageRouterCallback->handleCommand("gui", cmd);

}

void ZrtpHostBridgeMinisip::rtpSessionError() {
    MRef<CryptoContext *> cryptoContext;

    cryptoContext = new CryptoContext(senderZrtpSsrc);
    sStream->setKeyAgreementZrtp(cryptoContext);

    cryptoContext = new CryptoContext(senderSsrc);
    sStream->setKeyAgreementZrtp(cryptoContext);

    cryptoContext = new CryptoContext(recvZrtpSsrc);
    sStream->setKeyAgreementZrtp(cryptoContext);

    cryptoContext = new CryptoContext(receiverSsrc);
    sStream->setKeyAgreementZrtp(cryptoContext);

    sendInfo(Alert, "RTP session error - security switched off!");

    CommandString cmd(callId, "zrtp_security_change", "insecure");
    messageRouterCallback->handleCommand("gui", cmd);
}

void ZrtpHostBridgeMinisip::zrtpNegotiationFailed(MessageSeverity severity, const char* msg)
{
    fprintf(stderr, "Severity: %d - %s\n", severity, msg);
}

void ZrtpHostBridgeMinisip::zrtpNotSuppOther() {

    fprintf(stderr, "The other (remote) client does not support ZRTP\n");
}

#endif

