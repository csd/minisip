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

/* Copyright (C) 2004 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#include<config.h>
#include<libmikey/keyagreement.h>
#include<libmsip/SdpHeaderM.h>
#include<libmsip/SdpHeaderA.h>
#include<libmsip/SdpPacket.h>
#include<libmnetutil/UDPSocket.h>
#include<libmutil/itoa.h>
#include"MediaStream.h"
#include"Media.h"
#include"RtpReceiver.h"
#include"../codecs/Codec.h"
#include"../rtp/SRtpPacket.h"
#include"../minisip/ipprovider/IpProvider.h"
#include<iostream>

using namespace std;

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
	int i;
	uint8_t rtpPayloadType = m->getFormat(formatIndex);

	media->handleMHeader( m );

	if( m->getMedia() != getSdpMediaType() ){
		return false;
	}
	
	/* If we have an rtpmap:, it should be the same */
	rtpmap = m->getRtpMap( rtpPayloadType );
	if( getRtpMap() != "" ){
		// FIXME
		size_t s1 = getRtpMap().find("/");
		size_t s2 = rtpmap.find("/");
		return getRtpMap().substr(0, s1) == rtpmap.substr(0,s2);
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
	
	if( !ka ){
		/* Dummy cryptocontext */
		cryptoContext = new CryptoContext( ssrc );
	}
	else{
		uint8_t csId;
		uint32_t roc;
		unsigned char * masterKey = new unsigned char[16];
		unsigned char * masterSalt = new unsigned char[14];

		csId = ka->getSrtpCsId( ssrc );
		roc = ka->getSrtpRoc( ssrc );

		ka->genTek( csId,  masterKey,  16 );
		ka->genSalt( csId, masterSalt, 14 );

		cerr << "SSRC: "<< ssrc <<" - TEK: " << print_hex( masterKey, 16 ) << endl;
		cerr << "SSRC: "<< ssrc <<" - SALT: " << print_hex( masterSalt, 14 )<< endl;

		if( csId != 0 ){
			cryptoContext = new CryptoContext( ssrc, roc, 
			0/*key_deriv_rate FIXME */, 
			/* FIXME: get those from the MIKEY SP! */
			aes_cm, hmacsha1, masterKey, 16, masterSalt, 14 );

			cryptoContext->derive_srtp_keys( 0 );
		}
		else{
			cryptoContext = new CryptoContext( ssrc );
		}
	}

	cryptoContexts.push_back( cryptoContext );
	return cryptoContext;
}

MRef<CryptoContext *> MediaStream::getCryptoContext( uint32_t ssrc ){
	list< MRef<CryptoContext *> >::iterator i;

	for( i = cryptoContexts.begin(); i!= cryptoContexts.end(); i++ ){
		if( (*i)->getSsrc() == ssrc ){
			return (*i);
		}
	}

	return initCrypto( ssrc );
}

uint32_t MediaStream::getSsrc(){
	return ssrc;
}

void MediaStream::setKeyAgreement( MRef<KeyAgreement *> ka ){
	this->ka = ka;
}



MediaStreamReceiver::MediaStreamReceiver( MRef<Media *> media, 
		MRef<RtpReceiver *> rtpReceiver, MRef<IpProvider *> ipProvider ):
			MediaStream( media ),
			rtpReceiver( rtpReceiver ),
			ipProvider( ipProvider ){
	id = rand();
	externalPort = 0;
	ssrc = 0;
}

uint32_t MediaStreamReceiver::getId(){
	return id;
}

void MediaStreamReceiver::start(){
	media->registerMediaReceiver( this );
	rtpReceiver->registerMediaStream( this );
}

void MediaStreamReceiver::stop(){
	rtpReceiver->unregisterMediaStream( this );
	media->unRegisterMediaReceiver( this );
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
		cerr << "Marker was: "<< packet->getHeader().getMarker()<< endl;
		return;
	}

	uint16_t seqNo = packet->getHeader().getSeqNo();
	byte_t * data = packet->getContent();
	uint32_t size = packet->getContentLength();
	bool marker = packet->getHeader().getMarker();
	uint32_t ts = packet->getHeader().getTimestamp();

	media->playData( id, data, size, packetSsrc, seqNo, marker, ts );
}

MediaStreamSender::MediaStreamSender( MRef<Media *> media ):
	MediaStream( media ){
	remotePort = 0; 
	seqNo = 0;
	ssrc = rand();
	senderSock = new UDPSocket;
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

	packet->sendTo( *senderSock, *remoteAddress, remotePort );
	delete packet;
}

void MediaStreamSender::setRemoteAddress( IPAddress * remoteAddress ){
	this->remoteAddress = remoteAddress;
}
