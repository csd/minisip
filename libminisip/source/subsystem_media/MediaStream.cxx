/*
 Copyright (C) 2004-2006 the Minisip Team

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

/* Copyright (C) 2004, 2005
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
 *	    Joachim Orrblad <joachim@orrblad.com>
*/

#include <config.h>

#include<libminisip/media/MediaStream.h>

#include<libmikey/MikeyPayloadSP.h>
#include<libmikey/KeyAgreement.h>
#include<libminisip/signaling/sdp/SdpHeaderM.h>
#include<libminisip/signaling/sdp/SdpHeaderA.h>
#include<libminisip/signaling/sdp/SdpPacket.h>
#include<libmnetutil/UDPSocket.h>
#include<libmutil/stringutils.h>
#include<libmutil/Timestamp.h>
#include<libminisip/media/Media.h>
#include<libminisip/media/RtpReceiver.h>
#include<libminisip/media/codecs/Codec.h>
#include<libminisip/ipprovider/IpProvider.h>
#include<iostream>

#ifdef _WIN32_WCE
#	include"../include/minisip_wce_extra_includes.h"
#endif

using namespace std;

RealtimeMediaStream::RealtimeMediaStream( string cid, MRef<RealtimeMedia *> m) : MediaStream(cid,*m), realtimeMedia(m) /*callId(cid), media(m)*/,ka(NULL) {
	disabled = false;
#ifdef ZRTP_SUPPORT
	zrtpBridge = NULL;
#endif
}

std::string MediaStream::getSdpMediaType(){
	massert(media);
	if( media ){
		return media->getSdpMediaType();
	}
	return "";
}

list<string> MediaStream::getSdpAttributes(){
	return media->getSdpAttributes();
}

bool parseRtpMap(string rtpMap, string &name, string &rate, string &param){
	size_t pos = rtpMap.find('/');

	if( pos == string::npos )
		return false;

	size_t pos2 = rtpMap.find('/', pos + 1);

	name = rtpMap.substr(0, pos);
	rate = rtpMap.substr(pos + 1, pos2 - pos - 1);

	if( pos2 != string::npos )
		param = rtpMap.substr(pos2 + 1);
	else
		param = "1";

	return true;
}

bool RealtimeMediaStream::matches( MRef<SdpHeaderM *> m, uint32_t formatIndex ){
        string sdpRtpMap;
	string sdpFmtpParam;

        //	int i;
        string sdpPayloadType = m->getFormat( formatIndex );

        realtimeMedia->handleMHeader( m );

        // pn507 This checks for "Audio"
        if( m->getMedia() != getSdpMediaType() ){
                return false;
        }

        sdpRtpMap = m->getRtpMap( sdpPayloadType );
	sdpFmtpParam = m->getFmtpParam( sdpPayloadType );

	std::list<MRef<Codec *> > codecs = realtimeMedia->getAvailableCodecs();
	std::list<MRef<Codec *> >::iterator iC;
	string sdpName;
	string sdpRate;
	string sdpParam;

	parseRtpMap(sdpRtpMap, sdpName, sdpRate, sdpParam);

	for( iC = codecs.begin(); iC != codecs.end(); iC ++ ){
		string codecRtpMap;
		int codecPayloadType;

		codecRtpMap = (*iC)->getSdpMediaAttributes();
		codecPayloadType = (*iC)->getSdpMediaType();
		if( (*iC)->getCodecName() == "iLBC" ) {
			if( sdpFmtpParam != "mode=20" ) { //iLBC only supports 20ms frames (in minisip)
				continue;
			} //else ... does not mean we accept it, it still goes through the normal checks ...
		}
                if( sdpRtpMap != "" && codecRtpMap != "" ){
			string codecName;
			string codecRate;
			string codecParam;

			if( !parseRtpMap(codecRtpMap, codecName, codecRate, codecParam) ){
				continue;
			}

			bool sdpRtpMapEqual = !strCaseCmp( codecName.c_str(), sdpName.c_str() ) && codecRate == sdpRate && codecParam == sdpParam;
                        if ( sdpRtpMapEqual ) {
				localPayloadType = itoa(codecPayloadType);
                                return true;
                        }
                        else {
				continue;
			}

                }
                else{
                        if( sdpPayloadType == itoa(codecPayloadType) ){

				localPayloadType = itoa(codecPayloadType);
                                return true;
                        }else{
			}
                }
        }
        return false;
}


ReliableMediaStream::ReliableMediaStream(std::string callId, MRef<ReliableMedia*> m) 
		: MediaStream(callId, *m)
{
	

}


/*
uint16_t ReliableMediaStream::getPort(){
	return 33333; //FIXME: Fixed when RFB source is commited... blame Erik
}

uint16_t ReliableMediaStream::getPort(std::string type){
	return 33333;
}
*/


MRef<CryptoContext *> RealtimeMediaStream::initCrypto( uint32_t ssrc, uint16_t seq_no ){
	MRef<CryptoContext *> cryptoContext;

	kaLock.lock();
	if( !ka ){
		/* Dummy cryptocontext */
		cryptoContext = new CryptoContext( ssrc );
	}
	else {

		unsigned char * masterKey = new unsigned char[16];
		unsigned char * masterSalt = new unsigned char[14];

		uint8_t  csId = ka->getSrtpCsId( ssrc );
		uint32_t roc = ka->getSrtpRoc( ssrc );
		uint8_t  policyNo = ka->findpolicyNo( ssrc );
		//Extract Srtp policy !!! Check the return value if type not available
		uint8_t ealg  = ka->getPolicyParamTypeValue(policyNo, MIKEY_PROTO_SRTP, MIKEY_SRTP_EALG);
		uint8_t ekeyl = ka->getPolicyParamTypeValue(policyNo, MIKEY_PROTO_SRTP, MIKEY_SRTP_EKEYL);
		uint8_t aalg  = ka->getPolicyParamTypeValue(policyNo, MIKEY_PROTO_SRTP, MIKEY_SRTP_AALG);
		uint8_t akeyl = ka->getPolicyParamTypeValue(policyNo, MIKEY_PROTO_SRTP, MIKEY_SRTP_AKEYL);
		uint8_t skeyl = ka->getPolicyParamTypeValue(policyNo, MIKEY_PROTO_SRTP, MIKEY_SRTP_SALTKEYL);
		//uint8_t prf   = ka->getPolicyParamTypeValue(policyNo, MIKEY_PROTO_SRTP, MIKEY_SRTP_PRF);	 //Not used
		uint8_t keydr = ka->getPolicyParamTypeValue(policyNo, MIKEY_PROTO_SRTP, MIKEY_SRTP_KEY_DERRATE);
		uint8_t encr  = ka->getPolicyParamTypeValue(policyNo, MIKEY_PROTO_SRTP, MIKEY_SRTP_ENCR_ON_OFF);
		//uint8_t cencr = ka->getPolicyParamTypeValue(policyNo, MIKEY_PROTO_SRTP, MIKEY_SRTCP_ENCR_ON_OFF);//Not used
		//uint8_t fecor = ka->getPolicyParamTypeValue(policyNo, MIKEY_PROTO_SRTP, MIKEY_SRTP_FEC_ORDER);	 //Not used
		uint8_t auth  = ka->getPolicyParamTypeValue(policyNo, MIKEY_PROTO_SRTP, MIKEY_SRTP_AUTH_ON_OFF);
		uint8_t autht = ka->getPolicyParamTypeValue(policyNo, MIKEY_PROTO_SRTP, MIKEY_SRTP_AUTH_TAGL);
		//uint8_t prefi = ka->getPolicyParamTypeValue(policyNo, MIKEY_PROTO_SRTP, MIKEY_SRTP_PREFIX);	 //Not used
	
#ifdef ENABLE_TS
		ts.save("TEK_START");
#endif

		ka->genTek( csId,  masterKey,  16 );

#ifdef ENABLE_TS	
		ts.save("TEK_STOP");
#endif
		
		ka->genSalt( csId, masterSalt, 14 );

#ifdef DEBUG_OUTPUT
#if 0
		fprintf( stderr, "csId: %i\n", csId );
		cerr << "SSRC: "<< ssrc <<" - TEK: " << binToHex( masterKey, 16 ) << endl;
		cerr << "SSRC: "<< ssrc <<" - SALT: " << binToHex( masterSalt, 14 )<< endl;
#endif
#endif

//		if( csId != 0 ){
			cryptoContext = new CryptoContext( ssrc, roc, seq_no, keydr,
			ealg, aalg, masterKey, 16, masterSalt, 14, ekeyl, akeyl, skeyl, encr, auth, autht );

			cryptoContext->derive_srtp_keys( 0 );
//		}
//		else{
//			cerr << "EEEE: WARNING - csId is NULL"<<endl;
//			cryptoContext = new CryptoContext( ssrc );
//		}
	}

	cryptoContexts.push_back( cryptoContext );
	kaLock.unlock();
	return cryptoContext;
}

MRef<CryptoContext *> RealtimeMediaStream::getCryptoContext( uint32_t ssrc, uint16_t seq_no ){
	kaLock.lock();
	list< MRef<CryptoContext *> >::iterator i;

	for( i = cryptoContexts.begin(); i!= cryptoContexts.end(); i++ ){
		if( (*i)->getSsrc() == ssrc ){
			kaLock.unlock();
			return (*i);
		}
	}

	kaLock.unlock();
	return initCrypto( ssrc, seq_no );
}

void RealtimeMediaStream::setKeyAgreement( MRef<KeyAgreement *> ka_ ){
	kaLock.lock();
	this->ka = ka_;

	/* Reset the CryptoContext since we have a new KeyAgreement */
	cryptoContexts.clear();

	kaLock.unlock();
}

#ifdef ZRTP_SUPPORT
/*
 * The ZRTP implementation (host bridge) calls this to register a new
 * crypto context context for either the receiver or the sender of
 * a receiver / sender. Keep in mind that we may have several senders
 * for one receiver, that is one receiver can handle different
 * incomming RTP sessions, they are identified by the SSRC. A sender
 * handles one stream (media stream) to the remote peer only.
 */
void RealtimeMediaStream::setKeyAgreementZrtp(MRef<CryptoContext *>cx) {

    kaLock.lock();

    list< MRef<CryptoContext *> >::iterator i;

    for( i = cryptoContexts.begin(); i!= cryptoContexts.end(); i++ ){
        if( (*i)->getSsrc() == cx->getSsrc() ) {
	    cryptoContexts.erase(i);
	    break;
	}
    }
    cryptoContexts.push_back(cx);

    kaLock.unlock();
}

void RealtimeMediaStream::setZrtpHostBridge(MRef<ZrtpHostBridgeMinisip *> zsb) {
	zrtpBridge = zsb;
}

MRef<ZrtpHostBridgeMinisip *> RealtimeMediaStream::getZrtpHostBridge() {
	return zrtpBridge;
}

#endif

RealtimeMediaStreamReceiver::RealtimeMediaStreamReceiver( string callid, MRef<RealtimeMedia *> m,
		MRef<RtpReceiver *> rtpRecv, 
		MRef<RtpReceiver *> rtp6Recv ):
			RealtimeMediaStream( callid,  m ),
			rtpReceiver( rtpRecv ),
			rtp6Receiver( rtp6Recv ){
	id = rand();
	externalPort = 0;
	running = false;
	codecList = m->getAvailableCodecs();
}

uint32_t RealtimeMediaStreamReceiver::getId(){
	return id;
}

void RealtimeMediaStreamReceiver::start(){
	if( !running ){
		if( rtpReceiver ){
			rtpReceiver->registerRealtimeMediaStream( this );
		}
		if( rtp6Receiver )
			rtp6Receiver->registerRealtimeMediaStream( this );
		running = true;
	}
}

void RealtimeMediaStreamReceiver::stop(){
	list<uint32_t>::iterator i;
	if( rtpReceiver ){
		rtpReceiver->unregisterRealtimeMediaStream( this );
		rtpReceiver->stop();
	}
	if( rtp6Receiver ){
		rtp6Receiver->unregisterRealtimeMediaStream( this );
		rtp6Receiver->stop();
	}
	if( rtpReceiver )
		rtpReceiver->join();

	if( rtp6Receiver )
		rtp6Receiver->join();

	ssrcListLock.lock();
	for( i = ssrcList.begin(); i != ssrcList.end(); i++ ){
		realtimeMedia->unregisterMediaSource( *i );
	}
	ssrcList.clear();
	ssrcListLock.unlock();

	running = false;
}

uint16_t RealtimeMediaStreamReceiver::getPort(){
	return rtpReceiver ? rtpReceiver->getPort() : ( rtp6Receiver ? rtp6Receiver->getPort() : 0 );
}

#ifdef ZRTP_SUPPORT
void RealtimeMediaStreamReceiver::handleRtpPacketExt(MRef<SRtpPacket *> packet) {
	uint32_t recvSsrc;
	uint16_t seq_no;

	//if packet is null, we had a read timeout from the rtpReceiver
	if( !packet ) {
		return;
	}
        recvSsrc = packet->getHeader().getSSRC();
	seq_no = packet->getHeader().getSeqNo();

        // if (zrtpBridge->isZrtpPacket(packet)) {
        //    packet->checkZrtpChecksum(false);
        // }
        // Look for a CryptoContext for this packet's SSRC
        MRef<CryptoContext *> pcc = getCryptoContext(recvSsrc, seq_no);

	if( packet->unprotect(pcc)) { // Authentication or replay protection failed
		return;
	}
	zrtpBridge->processPacket(packet);
}
#endif

void RealtimeMediaStreamReceiver::handleRtpPacket( const MRef<SRtpPacket *> & packet, string callId, MRef<IPAddress *> from ){
	uint32_t packetSsrc;
	uint16_t seq_no;

	//if packet is null, we had a read timeout from the rtpReceiver
	if( !packet ) {
		return;
	}

	packetSsrc = packet->getHeader().getSSRC();
	seq_no = packet->getHeader().getSeqNo();

#ifdef ZRTP_SUPPORT
        // if (zrtpBridge && packet->getHeader().getExtension() && zrtpBridge->isZrtpPacket(packet)) {
        //    packet->checkZrtpChecksum(false);
        // }
        MRef<CryptoContext *> pcc = getCryptoContext(packetSsrc, seq_no);

        // If empty crypto context for this SSRC but we are already in Secure
        // state then create a real CryptoContext for this SSRC. Assumption:
        // every SSRC stream sent via this connection is secured _and_ uses
        // the same crypto parameters.
        if (zrtpBridge && zrtpBridge->getSsrcSender() != 0 &&
                    zrtpBridge->isSecureState() &&
                   (pcc->getEalg() == MIKEY_SRTP_EALG_NULL)) {
            pcc = zrtpBridge->newCryptoContextForRecvSSRC(packetSsrc, 0, seq_no, 0L);
        }
        if( packet->unprotect(pcc)) { // Authentication or replay protection failed
            return;
        }
        if (zrtpBridge && packet->getHeader().getExtension() &&
            (zrtpBridge->processPacket(packet) == 0)) {
                return;
        }
#else
	if( packet->unprotect( getCryptoContext( packetSsrc, seq_no ) )){
		// Authentication or replay protection failed
		return;
	}

#endif // ZRTP_SUPPORT

	//uint16_t seqNo = packet->getHeader().getSeqNo(); //not used
	//byte_t * data = packet->getContent(); //not used
	//uint32_t size = packet->getContentLength(); //not used
	//bool marker = packet->getHeader().getMarker(); //not used
	//uint32_t ts = packet->getHeader().getTimestamp(); //not used

	gotSsrc( packetSsrc, callId );

	realtimeMedia->playData( *packet );
}

void RealtimeMediaStreamReceiver::gotSsrc( uint32_t ssrc, string callId ){
	list<uint32_t>::iterator i;

	ssrcListLock.lock();
	for( i = ssrcList.begin(); i != ssrcList.end(); i++ ){
		if( (*i) == ssrc ){
			ssrcListLock.unlock();
			return;
		}
	}

	massert(callId.size()>0);
	realtimeMedia->registerMediaSource( ssrc, callId );
	ssrcList.push_back( ssrc );
	ssrcListLock.unlock();
}

std::list<MRef<Codec *> > RealtimeMediaStreamReceiver::getAvailableCodecs(){
	return codecList;
}


uint16_t RealtimeMediaStreamReceiver::getPort( const string &addrType ){
	if( addrType == "IP4" && rtpReceiver )
		return rtpReceiver->getPort();
	else if( addrType == "IP6" && rtp6Receiver )
		return rtp6Receiver->getPort();
	return 0;
}


RealtimeMediaStreamSender::RealtimeMediaStreamSender( string callid, MRef<RealtimeMedia *> m, 
		MRef<UDPSocket *> senderSocket, 
		MRef<UDPSocket *> sender6Socket ):
			RealtimeMediaStream( callid, m ){
	selectedCodec = NULL;
	remotePort = 0;
	seqNo = (uint16_t)rand();
	ssrc = rand();
//cerr<<" ------------------------- streamSender ssrc --------------------:"<<ssrc<<endl;
	lastTs = rand();
        payloadType = "255";
	setMuted( true );
	muteCounter = 0;
//	senderSockHack = new UDPSocket();
	if( senderSocket ){
		this->senderSock = senderSocket;
	}
	else{
		senderSock = new UDPSocket;
		senderSock->setLowDelay();
	}

	this->sender6Sock = sender6Socket;
}


void RealtimeMediaStreamSender:: setSelectedCodecHacked( MRef <RealtimeMedia*> m){
       
	
	 selectedCodec = m-> getCodecInstance() ;
	
	
}

void RealtimeMediaStreamSender::start(){
	realtimeMedia->registerRealtimeMediaSender( this );
}

void RealtimeMediaStreamSender::stop(){
	realtimeMedia->unregisterRealtimeMediaSender( this );
	senderSock = NULL;
	sender6Sock = NULL;

#ifdef ZRTP_SUPPORT
	if (zrtpBridge) {
	    zrtpBridge->stop();
	}
#endif
}

void RealtimeMediaStreamSender::setPort( uint16_t port ){
	remotePort = port;
}

uint16_t RealtimeMediaStreamSender::getPort(){
	return remotePort;
}

static bool first=true;

/*
 * Some of the following stuff was set after looking at the real
 * wire protocol of the original ZRTP / Zfone
 * - it uses the current seq no and does not increment it
 * - it uses the SSRC of "deadbeef" as hex value. We have to keep
 *   that in mind when processing protect / unprotect SRTP operations.
 *   Protect / unprotect use the "real" SSRC to lookup the crypto
 *   contexts.
 */
#ifdef ZRTP_SUPPORT
void RealtimeMediaStreamSender::sendZrtp(unsigned char* data, int length,
                                unsigned char* payload, int payLen) {

	if (this->remoteAddress.isNull()) {
		mdbg("media/zrtp") << " RealtimeMediaStreamSender::sendZrtp called before " <<
			"setRemoteAddress!" << endl;
		return;
	}

	SRtpPacket * packet;
	if (first){
#ifdef ENABLE_TS
		ts.save("rtp_send");
#endif
		first=false;
	}

	senderLock.lock();
	uint32_t ts = time(NULL);
        packet = new SRtpPacket(payload, payLen, zrtpBridge->getZrtpSendSeqNo(),
                                ts, zrtpBridge->getZrtpSendSsrc() );

        packet->getHeader().setPayloadType(13);
	packet->setExtHeader(data, length);

	packet->protect(getCryptoContext(ssrc, seqNo));

        // packet->enableZrtpChecksum();
	packet->sendTo( **senderSock, **remoteAddress, remotePort );
	delete packet;
	senderLock.unlock();

}
#endif // ZRTP_SUPPORT

void RealtimeMediaStreamSender::send( byte_t * data, uint32_t length, uint32_t * givenTs, bool marker, bool dtmf ){
	 

	if (this->remoteAddress.isNull()) {
		mdbg("media") << " RealtimeMediaStreamSender::send called before " <<
			"setRemoteAddress!" << endl;
		return;
	}
	SRtpPacket * packet;
	if (first){
#ifdef ENABLE_TS
		ts.save("rtp_send");
#endif
		first=false;
	}
	
	senderLock.lock();
	if( !(*givenTs) ){
		//FIXME! get it from the CODEC,
		// when we have one CODEC per sender
		increaseLastTs(); //increase lastTs ...
				//lastTs += 160;
		*givenTs = lastTs;
	}
	else{
		lastTs = *givenTs;
	}
	packet = new SRtpPacket( data, length, seqNo++, lastTs, ssrc );
	if( dtmf ){
		packet->getHeader().setPayloadType( 101 );
	}
	else{
		if( payloadType != "255" ){
			packet->getHeader().setPayloadType( atoi(payloadType.c_str() ) );
		}
		else{
			//packet->getHeader().setPayloadType( selectedCodec->getSdpMediaType() );
			 packet->getHeader().setPayloadType( 99 );
		}
	}
	if( marker ){
		packet->getHeader().setMarker( marker );
	}

	packet->protect( getCryptoContext( ssrc, seqNo - 1 ) );
	if( remoteAddress->getAddressFamily() == AF_INET && senderSock ){
		int t = packet->getHeader().getPayloadType();
//		if (primarySsrcHack==0 && t==99)
//			primarySsrcHack =ssrc;
//
//		if (t==99 && ssrc!=primarySsrcHack)
//			packet->sendTo( **senderSockHack, **remoteAddress, remotePort );
//		else
			packet->sendTo( **senderSock, **remoteAddress, remotePort );

	}else if( remoteAddress->getAddressFamily() == AF_INET6 && sender6Sock )
		packet->sendTo( **sender6Sock, **remoteAddress, remotePort );

	delete packet;
	senderLock.unlock();

        /*
         * Start ZRTP engine only after we sent a first real data packet. This is
         * to give the receiver a change to get the real SSRC of us.
         */
#ifdef ZRTP_SUPPORT
	if (zrtpBridge && zrtpBridge->getSsrcSender() == 0) {
            zrtpBridge->setSsrcSender(ssrc);
            zrtpBridge->start();
        }
#endif

}

void RealtimeMediaStreamSender::sendRtpPacket(const MRef<RtpPacket*> & rtp){
	if( remoteAddress->getAddressFamily() == AF_INET && senderSock )
		rtp->sendTo( **senderSock, **remoteAddress, remotePort );
	else if( remoteAddress->getAddressFamily() == AF_INET6 && sender6Sock )
		rtp->sendTo( **sender6Sock, **remoteAddress, remotePort );
}

void RealtimeMediaStreamSender::setRemoteAddress( MRef<IPAddress *> ra){
	mdbg("media") << "RealtimeMediaStreamSender::setRemoteAddress: " <<
		ra->getString() << endl;
	this->remoteAddress = ra;
#ifdef ZRTP_SUPPORT
	if (zrtpBridge) {
	    zrtpBridge->setRemoteAddress(ra);
	}
#endif // ZRTP_SUPPORT
}

#ifdef DEBUG_OUTPUT
string MediaStream::getDebugString() {
	string ret;
	ret = getMemObjectType() + " this=" + itoa(reinterpret_cast<uint64_t>(this)) +
		" media="+media->getSdpMediaType()+" port=" + itoa(getPort());

	return ret;
}
string RealtimeMediaStreamReceiver::getDebugString() {
	string ret;
	ret = getMemObjectType() + " this=" + itoa(reinterpret_cast<uint64_t>(this)) +
		" media="+media->getSdpMediaType()+"; listening port=" + itoa(getPort());
	for( std::list<uint32_t>::iterator it = ssrcList.begin();
				it != ssrcList.end();
				it++) {
		"; ssrc=" + itoa((*it));
	}
	return ret;
}
string RealtimeMediaStreamSender::getDebugString() {
	string ret;

	ret = getMemObjectType() + " this=" + itoa(reinterpret_cast<uint64_t>(this)) +
		" media="+media->getSdpMediaType()+"; port=" + itoa(getPort()) +
		"; remotePort=" + itoa(remotePort);

	if( isMuted() == true )
		ret += "; isMuted=true";
	else ret += "; isMuted=false";

	return ret;
}
#endif

//keep a counter, so we can send a keep alive
//packet every now and then.
//Max indicates every how many silenced packets we send one keep-alive
//It returns true if the packet needs to be let through, false otherwise
bool RealtimeMediaStreamSender::muteKeepAlive( uint32_t max ) {
	bool ret = false;

	//if muted, only return true if packet is keep alive
	if( isMuted() ) {
		muteCounter++;
		if( muteCounter >= max ) {
			ret = true;
			muteCounter = 0;
		}
	} else {
		//if active sender, let through
		ret = true;
	}
	return ret;
}

bool RealtimeMediaStreamSender::matches( MRef<SdpHeaderM *> m, uint32_t formatIndex ){
	bool result = RealtimeMediaStream::matches( m, formatIndex );
	
	if( result && !selectedCodec ){
		selectedCodec = realtimeMedia->createCodecInstance(
				atoi( localPayloadType.c_str() )  );
		payloadType = itoa( (uint8_t)atoi(m->getFormat( formatIndex ).c_str()) )  ;
		result=true;
	}

	return result;
}

uint32_t RealtimeMediaStreamSender::getSsrc(){
	return ssrc;
}

