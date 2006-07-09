/*
  Copyright (C) 2006 Werner Dittmann 
 
  This library is free software; you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the
  Free Software Foundation; either version 2.1 of the License, or (at your
  option) any later version.
 
  This library is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
  for more details.
 
  You should have received a copy of the GNU Lesser General Public License
  along with this library; if not, write to the Free Software Foundation,
  Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

/*
 * Authors: Werner Dittmann <Werner.Dittmann@t-online.de>
 */

#include<config.h>

#include <libminisip/zrtp/ZrtpHostBridgeMinisip.h>
#include <libminisip/zrtp/ZIDFile.h>
#include <libminisip/zrtp/ZrtpStateClass.h>
#include <libminisip/zrtp/ZrtpHostBridgeMinisip.h>

#include <libmikey/MikeyPayloadSP.h>
#include <libminisip/configbackend/UserConfig.h>
#include <libmutil/CommandString.h>

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
} 

ZrtpHostBridgeMinisip::~ZrtpHostBridgeMinisip() {

    freeStateMachine();		// to clean up the TimeoutProvider
    delete zrtpEngine;
}


void ZrtpHostBridgeMinisip::start() {
    ZIDFile *zid = ZIDFile::getInstance();
    const uint8_t* ownZid = zid->getZid();

    if (zrtpEngine == NULL) {
        zrtpEngine = new ZRtp((uint8_t*)ownZid, (ZrtpCallback*)this);
        zrtpEngine->startZrtpEngine();
    }
}

void ZrtpHostBridgeMinisip::stop() {
    zrtpEngine->stopZrtp();
    delete zrtpEngine;
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
     * It's a ZRTP packet, check if ZRTP already started. If not return zero
     * to dismiss packet.
     */
    if (zrtpEngine == NULL) {
        return 0;
    }
    receiverSeqNo = packet->getHeader().getSeqNo();

    int ret = zrtpEngine->processExtensionHeader(extHeader, packet->getContent());

    // Fail is only a fail of the protocol state, already handled but
    // payload usually not affected - thus caller may process it
    return ((ret == Fail || ret == Done) ? 1 : 0);
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
    MRef<CryptoContext *> cryptoContext;
    int64_t keydr = 1;
    char buffer[128];
    
    if (part == ForSender || part == (EnableSecurity)ForSender+ForReceiver) {
    // encrypting packets, intiator uses initiator keys, responder uses responders keys
	if (secrets->role == Initiator) {
	    cryptoContext = new CryptoContext(
		    senderSsrc, 
                    0 /*roc*/, 
                    sStream->getSeqNo(), 
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
            cryptoContext = new CryptoContext(
		    senderSsrc, 
                    0 /*roc*/, 
                    sStream->getSeqNo(), 
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
        cryptoContext->derive_srtp_keys( sStream->getSeqNo() );   // TODO check this
        sStream->setKeyAgreementZrtp(cryptoContext);
        snprintf(buffer, 120, "SAS Value(S): %s\n", secrets->sas.c_str());
        sendInfo(Info, buffer);
        secureParts += (int32_t)ForSender;

    }
    if (part == ForReceiver || part == (EnableSecurity)ForSender+ForReceiver) {
    // decrypting packets, intiator uses responder keys, responder initiator keys
	if (secrets->role == Initiator) {
	    cryptoContext = new CryptoContext(
		    receiverSsrc, 
                    0 /*roc*/, 
                    receiverSeqNo, 
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
	    cryptoContext = new CryptoContext(
		    receiverSsrc, 
                    0 /*roc*/, 
                    receiverSeqNo, 
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
        // roc << 16 | seqNo
        cryptoContext->derive_srtp_keys( sStream->getSeqNo() );   // TODO check this
        rStream->setKeyAgreementZrtp(cryptoContext);
        snprintf(buffer, 120, "SAS Value: %s\n", secrets->sas.c_str());
        sendInfo(Info, buffer);
        secureParts += (int32_t)ForReceiver;
    }
    if (secureParts == ForSender+ForReceiver) {
        CommandString cmd(callId, "zrtp_security_change", "secure", secrets->sas);
        messageRouterCallback->handleCommand("gui", cmd);
    }
}

void ZrtpHostBridgeMinisip::srtpSecretsOff(EnableSecurity part) {
    MRef<CryptoContext *> cryptoContext;

    if (part == ForSender || part == (EnableSecurity)ForSender+ForReceiver) {
	cryptoContext = new CryptoContext(senderSsrc);
	sStream->setKeyAgreementZrtp(cryptoContext);
    }
    if (part == ForReceiver || part == (EnableSecurity)ForSender+ForReceiver) {
	cryptoContext = new CryptoContext(receiverSsrc);
	sStream->setKeyAgreementZrtp(cryptoContext);
    }
}

void ZrtpHostBridgeMinisip::rtpSessionError() {
    MRef<CryptoContext *> cryptoContext;

    cryptoContext = new CryptoContext(senderSsrc);
    sStream->setKeyAgreementZrtp(cryptoContext);
    
    cryptoContext = new CryptoContext(receiverSsrc);
    sStream->setKeyAgreementZrtp(cryptoContext);
    
    sendInfo(Alert, "RTP session error - security switched off!");
    
    CommandString cmd(callId, "zrtp_security_change", "insecure");
    messageRouterCallback->handleCommand("gui", cmd);
}



