/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/* Copyright (C) 2004, 2005 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
 *	    Joachim Orrblad <joachim@orrblad.com>
*/

#include<config.h>
#include<libmikey/MikeyPayloadSP.h>
#include<libmikey/keyagreement.h>
#include"../sdp/SdpHeaderM.h"
#include"../sdp/SdpHeaderA.h"
#include"../sdp/SdpPacket.h"
#include<libmnetutil/UDPSocket.h>
#include<libmutil/itoa.h>
#include<libmutil/print_hex.h>
#include"MediaStream.h"
#include"Media.h"
#include"RtpReceiver.h"
#include"../codecs/Codec.h"
#include"../rtp/SRtpPacket.h"
#include"../minisip/ipprovider/IpProvider.h"
#include<iostream>

using namespace std;


#ifdef _MSC_VER
static int nocaseequal(char c1, char c2){
	if ( ((c1>='A') && (c1<='Z')) ){
		return (c1==c2) || (c1 == (c2 - ('a'-'A')));
	}
	if ( (c1>='a') && (c1<='z') ){
		return (c1==c2) || (c1 == (c2 + ('a'-'A')));
	}
	return c1==c2;
}

static int strcasecmp(const char *s1, const char *s2){
	for (int i=0; s1[i]!=0 && s2[i]!=0; i++){
		if ( nocaseequal(s1[i],s2[i]) ){
			if (s1[i]<s2[i])
				return -1;
			else
				return 1;
		}
	}
	if (s2[i]!=0)
		return -1;
	return 0;
}
#endif

MediaStream::MediaStream( MRef<Media *> media ):media(media),ka(NULL){
	disabled = false;
	
}

std::string MediaStream::getSdpMediaType(){
	if( media ){
		return media->getSdpMediaType();
	}
	return "";

}

uint8_t MediaStream::getRtpPayloadType(){
	if( media ){
		return media->getRtpPayloadType();
	}
	return 0;
}

list<string> MediaStream::getSdpAttributes(){
		
	return media->getSdpAttributes();
}
	

string MediaStream::getRtpMap(){
	if( media ){
		return media->getRtpMap();
	}
	return "";
}

bool MediaStream::matches( MRef<SdpHeaderM *> m, uint32_t formatIndex ){
	string rtpmap;
//	int i;
	uint8_t rtpPayloadType = m->getFormat(formatIndex);

	media->handleMHeader( m );

	if( m->getMedia() != getSdpMediaType() ){
		return false;
	}
	
	/* If we have an rtpmap:, it should be the same */
	rtpmap = m->getRtpMap( rtpPayloadType );
	if( rtpmap != "" && getRtpMap() != "" ){
		// FIXME
		size_t s1 = getRtpMap().find("/");
		size_t s2 = rtpmap.find("/");
		bool rtpmapEqual = !strcasecmp( getRtpMap().substr(0, s1).c_str(), rtpmap.substr(0,s2).c_str() );
		return rtpmapEqual;
	}

	/* else check that the RTP payload type matches */
	return getRtpPayloadType() == rtpPayloadType;
}

void MediaStream::addToM( MRef<SdpPacket*> packet, MRef<SdpHeaderM *> m ){
	if( m->getPort() == 0 ){
		m->setPort( getPort() );
	}
	else if( m->getPort() != getPort() ){
		/* We have already added a format on that m line
		 * on another port! */
		return;
	}

	m->addFormat( getRtpPayloadType() );

	if( getRtpMap() != "" ){
		MRef<SdpHeaderA *> a = new SdpHeaderA( "a=x" );
		a->setAttributes( "rtpmap:" + itoa( getRtpPayloadType() ) + 
				  " " + getRtpMap() );
		packet->addHeader( *a );
	}
		
}

MRef<CryptoContext *> MediaStream::initCrypto( uint32_t ssrc ){
	MRef<CryptoContext *> cryptoContext;
	
	kaLock.lock();
	if( !ka ){
		/* Dummy cryptocontext */
		cryptoContext = new CryptoContext( ssrc );
	}
	else{
		
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
		uint8_t prf   = ka->getPolicyParamTypeValue(policyNo, MIKEY_PROTO_SRTP, MIKEY_SRTP_PRF);	 //Not used
		uint8_t keydr = ka->getPolicyParamTypeValue(policyNo, MIKEY_PROTO_SRTP, MIKEY_SRTP_KEY_DERRATE);
		uint8_t encr  = ka->getPolicyParamTypeValue(policyNo, MIKEY_PROTO_SRTP, MIKEY_SRTP_ENCR_ON_OFF); 
		uint8_t cencr = ka->getPolicyParamTypeValue(policyNo, MIKEY_PROTO_SRTP, MIKEY_SRTCP_ENCR_ON_OFF);//Not used
		uint8_t fecor = ka->getPolicyParamTypeValue(policyNo, MIKEY_PROTO_SRTP, MIKEY_SRTP_FEC_ORDER);	 //Not used
		uint8_t auth  = ka->getPolicyParamTypeValue(policyNo, MIKEY_PROTO_SRTP, MIKEY_SRTP_AUTH_ON_OFF); 
		uint8_t autht = ka->getPolicyParamTypeValue(policyNo, MIKEY_PROTO_SRTP, MIKEY_SRTP_AUTH_TAGL);	 //Not used
		uint8_t prefi = ka->getPolicyParamTypeValue(policyNo, MIKEY_PROTO_SRTP, MIKEY_SRTP_PREFIX);	 //Not used
		
		ka->genTek( csId,  masterKey,  16 );
		ka->genSalt( csId, masterSalt, 14 );

#ifdef DEBUG_OUTPUT
		fprintf( stderr, "csId: %i\n", csId );
		cerr << "SSRC: "<< ssrc <<" - TEK: " << print_hex( masterKey, 16 ) << endl;
		cerr << "SSRC: "<< ssrc <<" - SALT: " << print_hex( masterSalt, 14 )<< endl;
#endif

		if( csId != 0 ){
			cryptoContext = new CryptoContext( ssrc, roc, keydr, 
			ealg, aalg, masterKey, 16, masterSalt, 14, ekeyl, akeyl, skeyl, encr, auth );

			cryptoContext->derive_srtp_keys( 0 );
		}
		else{
			cryptoContext = new CryptoContext( ssrc );
		}
	}

	cryptoContexts.push_back( cryptoContext );
	kaLock.unlock();
	return cryptoContext;
}

MRef<CryptoContext *> MediaStream::getCryptoContext( uint32_t ssrc ){
	kaLock.lock();
	list< MRef<CryptoContext *> >::iterator i;

	for( i = cryptoContexts.begin(); i!= cryptoContexts.end(); i++ ){
		if( (*i)->getSsrc() == ssrc ){
			kaLock.unlock();
			return (*i);
		}
	}

	kaLock.unlock();
	return initCrypto( ssrc );
}

uint32_t MediaStream::getSsrc(){
	return ssrc;
}

void MediaStream::setKeyAgreement( MRef<KeyAgreement *> ka ){
	kaLock.lock();
	this->ka = ka;

	/* Reset the CryptoContext since we have a new KeyAgreement */
	cryptoContexts.clear();

	kaLock.unlock();
}



MediaStreamReceiver::MediaStreamReceiver( MRef<Media *> media, 
		MRef<RtpReceiver *> rtpReceiver, MRef<IpProvider *> ipProvider ):
			MediaStream( media ),
			rtpReceiver( rtpReceiver ),
			ipProvider( ipProvider ){
	id = rand();
	externalPort = 0;
	ssrc = 0;
	running = false;
}

uint32_t MediaStreamReceiver::getId(){
	return id;
}

void MediaStreamReceiver::start(){
	if( !running ){
		rtpReceiver->registerMediaStream( this );
		running = true;
	}
}

void MediaStreamReceiver::stop(){
	list<uint32_t>::iterator i;
	rtpReceiver->unregisterMediaStream( this );

	ssrcListLock.lock();
	for( i = ssrcList.begin(); i != ssrcList.end(); i++ ){
		media->unRegisterMediaSource( *i );
	}
	ssrcList.clear();
	ssrcListLock.unlock();
	
	running = false;
}


void MediaStreamReceiver::setPort( uint16_t port ){
}

uint16_t MediaStreamReceiver::getPort(){
	return rtpReceiver->getPort();
}

void MediaStreamReceiver::handleRtpPacket( SRtpPacket * packet ){
	uint32_t packetSsrc = packet->getHeader().getSSRC();
	
	if( packet->unprotect( getCryptoContext( packetSsrc ) )){
		// Authentication or replay protection failed
		return;
	}

	uint16_t seqNo = packet->getHeader().getSeqNo();
	byte_t * data = packet->getContent();
	uint32_t size = packet->getContentLength();
	bool marker = packet->getHeader().getMarker();
	uint32_t ts = packet->getHeader().getTimestamp();

	gotSsrc( packetSsrc );

	media->playData( packet );
}

void MediaStreamReceiver::gotSsrc( uint32_t ssrc ){
	list<uint32_t>::iterator i;

	ssrcListLock.lock();
	for( i = ssrcList.begin(); i != ssrcList.end(); i++ ){
		if( (*i) == ssrc ){
			ssrcListLock.unlock();
			return;
		}
	}
	fprintf( stderr, "Registering media stream %s\n", media->getSdpMediaType().c_str() );
	
	media->registerMediaSource( ssrc );
	ssrcList.push_back( ssrc );
	ssrcListLock.unlock();
}

MediaStreamSender::MediaStreamSender( MRef<Media *> media, MRef<UDPSocket *> senderSocket ):
	MediaStream( media ){
	remotePort = 0; 
	seqNo = 0;
	ssrc = rand();
	if( senderSocket ){
		this->senderSock = senderSocket;
	}
	else{
		senderSock = new UDPSocket;
	}
}

void MediaStreamSender::start(){
	media->registerMediaSender( this );
}

void MediaStreamSender::stop(){
	media->unRegisterMediaSender( this );
}

void MediaStreamSender::setPort( uint16_t port ){
	remotePort = port;
}

uint16_t MediaStreamSender::getPort(){
	return remotePort;
}

void MediaStreamSender::send( byte_t * data, uint32_t length, uint32_t ts, bool marker ){
	SRtpPacket * packet;
	
	packet = new SRtpPacket( data, length, seqNo++, ts, ssrc );
	
	packet->getHeader().setPayloadType( getRtpPayloadType() );
	if( marker ){
		packet->getHeader().setMarker( marker );
	}

	packet->protect( getCryptoContext( ssrc ) );

	packet->sendTo( **senderSock, *remoteAddress, remotePort );
	delete packet;
}

void MediaStreamSender::setRemoteAddress( IPAddress * remoteAddress ){
	this->remoteAddress = remoteAddress;
}
