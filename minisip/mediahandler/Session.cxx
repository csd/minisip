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
 *	    Joachim Orrblad <joachim@orrblad.com>
*/

#include<config.h>
#include"Session.h"
#include"MediaStream.h"
#include"Media.h"
#include"RtpReceiver.h"
#include"../codecs/Codec.h"
#include"../minisip/ipprovider/IpProvider.h"
#include"../sdp/SdpPacket.h"
#include"../sdp/SdpHeaderV.h"
#include"../sdp/SdpHeaderT.h"
#include"../sdp/SdpHeaderC.h"
#include"../sdp/SdpHeaderA.h"
#include"../sdp/SdpHeaderM.h"
#include"../sdp/SdpHeaderS.h"
#include"../sdp/SdpHeaderO.h"
#include<libmikey/keyagreement.h>
#include<libmikey/keyagreement_dh.h>
#include<libmutil/dbg.h>

#define SESSION_LINE "s=Minisip Session"

Session::Session( string localIp, SipDialogSecurityConfig &securityConfig ):ka(NULL),localIpString(localIp){
	this->securityConfig = securityConfig; // hardcopy
}

MRef<SdpPacket *> Session::emptySdp(){
	MRef<SdpPacket *> result;

	result = new SdpPacket;

	MRef<SdpHeader*> v = new SdpHeaderV(0);
	result->addHeader(v);

	/* FIXME */
	string addrtype = "IP4";

	MRef<SdpHeader*> o = new SdpHeaderO("","3344","3344","IN", 
			addrtype, localIpString );
	result->addHeader(o);

	MRef<SdpHeader*> s = new SdpHeaderS(SESSION_LINE);
	result->addHeader(s);

	MRef<SdpHeader*> c = new SdpHeaderC("IN", addrtype, localIpString );
	result->addHeader(c);

	MRef<SdpHeader*> t = new SdpHeaderT(0,0);
	result->addHeader(t);

	return result;
}

MRef<SdpPacket *> Session::getSdpOffer(){ // used by the initiator when creating the first message
	MRef<SdpPacket *> result;
	list< MRef<MediaStream *> >::iterator i;
	std::list<std::string>::iterator iAttribute;
	std::list<std::string> attributes;
	string type;
	uint16_t localPort;
	uint32_t rtpPayloadType;
	string rtpMap;
	MRef<SdpHeaderM *> m;
	string keyMgmtMessage;

	result = emptySdp();
	fprintf( stderr, "secure before create: %i\n",securityConfig.secured );
	if( securityConfig.secured ){
		fprintf( stderr, "The call is secure, adding MIKEY header\n");
		MRef<SdpHeaderA *> a;
		keyMgmtMessage = initiatorCreate();  //in KeyAgreement.cxx
		if( ! securityConfig.secured ){
			// something went wrong
			return NULL;
		}
		a = new SdpHeaderA("a=X");
		a->setAttributes( "key-mgmt:"+keyMgmtMessage );
		result->addHeader( *a );
		
	}


	for( i = mediaStreamReceivers.begin(); i != mediaStreamReceivers.end();
			i++ ){

		type = (*i)->getSdpMediaType();
		localPort = (*i)->getPort();
		rtpPayloadType = (*i)->getRtpPayloadType();
		rtpMap = (*i)->getRtpMap();

		m = new SdpHeaderM( type, localPort, 1, "RTP/AVP" );
		m->addFormat( rtpPayloadType );

		result->addHeader( *m );

		if( rtpMap != "" ){
			MRef<SdpHeaderA*> a = new SdpHeaderA("a=X");
			a->setAttributes( "rtpmap:" + itoa(rtpPayloadType) +
					" " + rtpMap );
			m->addAttribute( *a );
		}

		attributes = (*i)->getSdpAttributes();

		for( iAttribute = attributes.begin(); iAttribute != attributes.end(); iAttribute ++ ){
			MRef<SdpHeaderA*> a = new SdpHeaderA("a=X");
			a->setAttributes( *iAttribute );
			m->addAttribute( *a );
		}

	}

	return result;
}

bool Session::setSdpAnswer( MRef<SdpPacket *> answer, string peerUri ){
	fprintf( stderr, "setSdpAnswer started\n" );
	unsigned int i;
	int j;
	MRef<MediaStream *> receiver;
	IPAddress * remoteAddress;
	// Not used
	int port;

	this->peerUri = peerUri;
	
	if( securityConfig.secured ){
		/* get the keymgt: attribute */
		string keyMgmtMessage = 
			answer->getSessionLevelAttribute( "key-mgmt" );
		if( !initiatorAuthenticate( keyMgmtMessage ) ){
			errorString = "Could not authenticate the key management message";
			fprintf( stderr, "Auth failed\n");
			return false;
		}

		string mikeyErrorMsg = initiatorParse();
		if( mikeyErrorMsg != "" ){
			errorString = "Could not parse the key management message. ";
			errorString += mikeyErrorMsg;
			fprintf( stderr, "Parse failed\n");
			return false;
		}

                else{
                        if( ka && ka->type() == KEY_AGREEMENT_TYPE_DH ){
                                ((KeyAgreementDH *)*ka)->computeTgk();
                        }
                }
	}

	remoteAddress = answer->getRemoteAddr( port );

	fprintf( stderr, "Starting loop\n" );

	for( i = 0; i < answer->getHeaders().size(); i++ ){
		if( answer->getHeaders()[i]->getType() == SDP_HEADER_TYPE_M ){
			MRef<SdpHeaderM *> m = 
				((SdpHeaderM*)*(answer->getHeaders()[i]));
			
			for( j = 0; j < m->getNrFormats(); j++ ){
				fprintf( stderr, "trying format %i\n", j );
				receiver = matchFormat( m, j, remoteAddress );
				if( receiver && m->getPort() == 0 ){
					/* This offer was rejected */
					receiver->disabled = true;
				}
			}
		}
	}
	fprintf( stderr,"setSdpAnswer returns true\n" );
	return true;
}

MRef<MediaStream *> Session::matchFormat( MRef<SdpHeaderM *> m, uint32_t iFormat, IPAddress * remoteAddress ){
	list< MRef<MediaStream *> >::iterator iStream;

	/* If we have a sender for this format, activate it */
	//fprintf( stderr, "Starting senders loop\n" );
	mdbg << "Starting senders loop" << end;
	uint8_t j = 1;
	for( iStream =  mediaStreamSenders.begin();
			iStream != mediaStreamSenders.end(); iStream++,j++ ){
		//fprintf( stderr, "Trying a sender\n" );
		mdbg << "Trying a sender"<< end;
		if( (*iStream)->matches( m, iFormat ) ){
			//fprintf( stderr, "Found sender for %s\n", (*iStream)->getSdpMediaType().c_str() );
			mdbg << "Found sender for " << (*iStream)->getSdpMediaType().c_str()<< end;
			if( ka ){
				ka->addSrtpStream( (*iStream)->getSsrc(),
					0, /*ROC */
					0, /* policy (fix me) */
					2*j/* CSID */
					);
			}
			(*iStream)->setPort( m->getPort() );
			((MediaStreamSender *)*(*iStream))->setRemoteAddress( remoteAddress );
		}
	}
	/* Look for a receiver */
	//fprintf( stderr, "Starting receivers loop\n" );
	mdbg << "Starting receivers loop"<< end;
	for( iStream =  mediaStreamReceivers.begin();
			iStream != mediaStreamReceivers.end(); iStream ++ ){
		if( (*iStream)->matches( m, iFormat ) ){
			return (*iStream);
		}
	}

	return NULL;
}

bool Session::setSdpOffer( MRef<SdpPacket *> offer, string peerUri ){ // used by the responder when receiving the first message
	fprintf( stderr, "setSdpOffer started\n" );
	unsigned int i;
	int j;
	MRef<MediaStream *> receiver;
	MRef<SdpPacket *> packet;
	IPAddress * remoteAddress;
	// Not used
	int port;
	string keyMgmtMessage;
	std::list<std::string>::iterator iAttribute;
	std::list<std::string> attributes;

	this->peerUri = peerUri;

	keyMgmtMessage = offer->getSessionLevelAttribute( "key-mgmt" );

	if( keyMgmtMessage != "" ){
		if( !responderAuthenticate( keyMgmtMessage ) ){
			errorString =  "Incoming key management message could not be authenticated";
			if( ka ){
				errorString += ka->authError();
			}
			return false;
		}
		else //Here we set the offer in ka
			setMikeyOffer();
	}
	else{
		securityConfig.secured = false;
		securityConfig.ka_type = KEY_MGMT_METHOD_NULL;
	}
		

	remoteAddress = offer->getRemoteAddr( port );

	sdpAnswer = emptySdp();

	for( i = 0; i < offer->getHeaders().size(); i++ ){

		if( offer->getHeaders()[i]->getType() == SDP_HEADER_TYPE_M ){
			MRef<SdpHeaderM *> offerM = (SdpHeaderM*)*(offer->getHeaders()[i]);
			
			MRef<SdpHeaderM *> answerM = new SdpHeaderM(
					offerM->getMedia(), 0, 0,
					offerM->getTransport() );

			sdpAnswer->addHeader( *answerM );

			for( j = 0; j < offerM->getNrFormats(); j++ ){
				receiver = matchFormat( offerM, j, remoteAddress );

				if( receiver ){
				/* found a receiver, accept the offer */
					receiver->addToM( sdpAnswer ,answerM );
					
					attributes = receiver->getSdpAttributes();

					for( iAttribute = attributes.begin(); 
					     iAttribute != attributes.end(); 
					     iAttribute ++ ){
						MRef<SdpHeaderA*> a = new SdpHeaderA("a=X");
						a->setAttributes( *iAttribute );
						answerM->addAttribute( *a );
					}

				}
			}
		}
	}
	return true;
}

MRef<SdpPacket *> Session::getSdpAnswer(){
	if( securityConfig.secured ){
		string keyMgmtAnswer;
		// Generate the key management answer message
		keyMgmtAnswer = responderParse();
		
		if( !securityConfig.secured ){
			// Something went wrong
			errorString = "Could not parse key management message.";
			fprintf(stderr, "responderParse failed\n" );
			return NULL;
		}
                else{
                        if( ka && ka->type() == KEY_AGREEMENT_TYPE_DH ){
                                ((KeyAgreementDH *)*ka)->computeTgk();
                        }
                }
		
		sdpAnswer->setSessionLevelAttribute( "key-mgmt", "mikey "+keyMgmtAnswer );
	}
	
	return sdpAnswer;
}
			

void Session::start(){
	list< MRef<MediaStream * > >::iterator i;


	for( i = mediaStreamReceivers.begin(); i != mediaStreamReceivers.end(); i++ ){
		if( ! (*i)->disabled ){
			(*i)->setKeyAgreement( ka );
			(*i)->start();
		}
	}
	
	for( i = mediaStreamSenders.begin(); i != mediaStreamSenders.end(); i++ ){
		if( (*i)->getPort() ){
			(*i)->setKeyAgreement( ka );
			(*i)->start();
		}
	}
}

void Session::stop(){
	list< MRef<MediaStream * > >::iterator i;

	for( i = mediaStreamReceivers.begin(); i != mediaStreamReceivers.end(); i++ ){
		if( ! (*i)->disabled ){
			(*i)->stop();
		}
	}
	
	for( i = mediaStreamSenders.begin(); i != mediaStreamSenders.end(); i++ ){
		if( (*i)->getPort() ){
			(*i)->stop();
		}
	}

	fprintf( stderr, "Session stopped\n" );
}


void Session::addMediaStreamReceiver( MRef<MediaStream *> mediaStream ){
	mediaStreamReceivers.push_back( *mediaStream );
}

void Session::addMediaStreamSender( MRef<MediaStream *> mediaStream ){
	mediaStreamSenders.push_back( *mediaStream );
}

	
string Session::getErrorString(){
	return errorString;
}

uint16_t Session::getErrorCode(){
	return errorCode;
}

bool Session::isSecure(){
	return securityConfig.secured;
}
