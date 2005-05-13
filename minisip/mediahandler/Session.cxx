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
#include"DtmfSender.h"
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
#include<libmutil/itoa.h>
#include<libmutil/Timestamp.h>

#define SESSION_LINE "s=Minisip Session"

// pn501 Added for multicodec list operations
using namespace std;

SessionRegistry * Session::registry = NULL;
MRef<KeyAgreement *> Session::precomputedKa = NULL;

Session::Session( string localIp, SipDialogSecurityConfig &securityConfig ):ka(NULL),localIpString(localIp),dtmfSender( this ){
	this->securityConfig = securityConfig; // hardcopy
        this->ka = Session::precomputedKa;
        Session::precomputedKa = NULL;

        if( registry ){
                registry->registerSession( this );
        }
}

void Session::unregister(){
        if( registry ){
                registry->unregisterSession( this );
        }

        if( Session::precomputedKa.isNull() ){
                Session::precomputedKa = new KeyAgreementDH( securityConfig.cert, securityConfig.cert_db, DH_GROUP_OAKLEY5 );
        }
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
	if( securityConfig.secured ){
		MRef<SdpHeaderA *> a;
		keyMgmtMessage = initiatorCreate();  //in KeyAgreement.cxx
		if( ! securityConfig.secured ){
			// something went wrong
			return NULL;
		}
		result->setSessionLevelAttribute( "key-mgmt", keyMgmtMessage );
	}

	for( i = mediaStreamReceivers.begin(); i != mediaStreamReceivers.end(); i++ ){

		type = (*i)->getSdpMediaType();
		localPort = (*i)->getPort();
		m = new SdpHeaderM( type, localPort, 1, "RTP/AVP" );
		
		std::string rtpMapValues = "";

		std::list<uint8_t> listPLT = (*i)->getAllRtpPayloadTypes();
		std::list<uint8_t>::iterator iListPLT;
		std::list<std::string> listM = (*i)->getAllRtpMaps();
		std::list<std::string>::iterator iListM;

		for( iListPLT = listPLT.begin(), iListM = listM.begin(); iListPLT != listPLT.end(); iListPLT ++, iListM ++) {
			m->addFormat( (*iListPLT) );
			rtpMapValues =  itoa( (*iListPLT) ) + " " + (*iListM);
			if( rtpMapValues != "" ){
				rtpMapValues = "rtpmap:" + rtpMapValues;
				MRef<SdpHeaderA*> a = new SdpHeaderA("a=X");
				a->setAttributes( rtpMapValues );
				m->addAttribute( *a );
			}
		}
		result->addHeader( *m );

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
	unsigned int i;
	int j;
	MRef<MediaStream *> receiver;
	IPAddress * remoteAddress;
	// Not used
	int port;

        cerr << "Called Session::setSdpAnswer with: " << answer->getString() << endl;
        cerr << "peerUri = " << peerUri << endl;
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

                /*
                else{
                        if( ka && ka->type() == KEY_AGREEMENT_TYPE_DH ){
                                ((KeyAgreementDH *)*ka)->computeTgk();
                        }
                }
                */
	}
	remoteAddress = answer->getRemoteAddr( port );

	for( i = 0; i < answer->getHeaders().size(); i++ ){
		if( answer->getHeaders()[i]->getType() == SDP_HEADER_TYPE_M ){
			MRef<SdpHeaderM *> m = ((SdpHeaderM*)*(answer->getHeaders()[i]));
                        cerr << "m header: " << m->getString() << endl;
                        cerr << "m NoFormats:  " << m->getNrFormats() << endl;
			
			for( j = 0; j < m->getNrFormats(); j++ ){
				receiver = matchFormat( m, j, remoteAddress );
				if( receiver && m->getPort() == 0 ){
					/* This offer was rejected */
					receiver->disabled = true;
				}
				else if( receiver ){
					/* Be ready to receive */
                                        receiver->start();
				}
			}
		}
	}
	return true;
}

MRef<MediaStream *> Session::matchFormat( MRef<SdpHeaderM *> m, uint32_t iFormat, IPAddress * remoteAddress ){
	list< MRef<MediaStream *> >::iterator iStream;

	/* If we have a sender for this format, activate it */
	mdbg << "Starting senders loop" << end;
	uint8_t j = 1;
        mediaStreamSendersLock.lock();
	for( iStream =  mediaStreamSenders.begin(); iStream != mediaStreamSenders.end(); iStream++,j++ ){
		mdbg << "Trying a sender"<< end;
		if( (*iStream)->matches( m, iFormat ) ){
			mdbg << "Found sender for " << (*iStream)->getSdpMediaType().c_str()<< end;
#if 0
			if( ka ){
				ka->addSrtpStream( (*iStream)->getSsrc(),
					0, /*ROC */
					0, /* policy (fix me) */
					2*j/* CSID */
					);
			}
#endif
	
			(*iStream)->setPort( m->getPort() );
			((MediaStreamSender *)*(*iStream))->setRemoteAddress( remoteAddress );
		}
	}
        mediaStreamSendersLock.unlock();
	/* Look for a receiver */
	mdbg << "Starting receivers loop"<< end;
	for( iStream =  mediaStreamReceivers.begin(); iStream != mediaStreamReceivers.end(); iStream ++ ){
		if( (*iStream)->matches( m, iFormat ) ){
			return (*iStream);
		}
	}

	return NULL;
}

bool Session::setSdpOffer( MRef<SdpPacket *> offer, string peerUri ){ // used by the responder when receiving the first message
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
                    if( answerM->getPort() == 0 ){
                        answerM->setPort( receiver->getPort() );
                    }
                    else if( answerM->getPort() != receiver->getPort() ){
                        /* We have already added a format on that m line
                         * on another port! */
                        continue;
                    }

                    /* found a receiver, accept the offer */
                    std::string rtpMapValues = "";

                    std::list<uint8_t> listPLT = receiver->getAllRtpPayloadTypes();
                    std::list<uint8_t>::iterator iListPLT;
                    std::list<std::string> listM = receiver->getAllRtpMaps();
                    std::list<std::string>::iterator iListM;

                    for( iListPLT = listPLT.begin(), iListM = listM.begin(); iListPLT != listPLT.end(); iListPLT ++, iListM ++) {
                        answerM->addFormat( (*iListPLT) );
                        rtpMapValues =  itoa( (*iListPLT) ) + " " + (*iListM);
                        if( rtpMapValues != "" ){
                            rtpMapValues = "rtpmap:" + rtpMapValues;
                            MRef<SdpHeaderA*> a = new SdpHeaderA("a=X");
                            a->setAttributes( rtpMapValues );
                            answerM->addAttribute( *a );
                        }
                        //           rtpMapValues = "";
                    }
					//receiver->addToM( sdpAnswer ,answerM, j );
					
					attributes = receiver->getSdpAttributes();

					for( iAttribute = attributes.begin(); iAttribute != attributes.end(); iAttribute ++ ){
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
                /*
                else{
                        if( ka && ka->type() == KEY_AGREEMENT_TYPE_DH ){
                                ((KeyAgreementDH *)*ka)->computeTgk();
                        }
                }
                */
		
		sdpAnswer->setSessionLevelAttribute( "key-mgmt", "mikey "+keyMgmtAnswer );
	}
	
	return sdpAnswer;
}
			

void Session::start(){
	list< MRef<MediaStream * > >::iterator i;
        
        if( securityConfig.secured && ka && ka->type() == KEY_AGREEMENT_TYPE_DH ){
                ts.save( TGK_START );
                ((KeyAgreementDH *)*ka)->computeTgk();
                ts.save( TGK_END );
        }


	for( i = mediaStreamReceivers.begin(); i != mediaStreamReceivers.end(); i++ ){
		if( ! (*i)->disabled ){
			if( securityConfig.secured ){
				(*i)->setKeyAgreement( ka );
			}
			(*i)->start();
		}
	}
	
        mediaStreamSendersLock.lock();
	for( i = mediaStreamSenders.begin(); i != mediaStreamSenders.end(); i++ ){
		if( (*i)->getPort() ){
			if( securityConfig.secured ){
				(*i)->setKeyAgreement( ka );
			}
			(*i)->start();
		}
	}
        mediaStreamSendersLock.unlock();
}

void Session::stop(){
	list< MRef<MediaStream * > >::iterator i;

	for( i = mediaStreamReceivers.begin(); i != mediaStreamReceivers.end(); i++ ){
		if( ! (*i)->disabled ){
			(*i)->stop();
		}
	}
	
        mediaStreamSendersLock.lock();
	for( i = mediaStreamSenders.begin(); i != mediaStreamSenders.end(); i++ ){
		if( (*i)->getPort() ){
			(*i)->stop();
		}
	}
        mediaStreamSendersLock.unlock();

	fprintf( stderr, "Session stopped\n" );
}


void Session::addMediaStreamReceiver( MRef<MediaStream *> mediaStream ){
	mediaStreamReceivers.push_back( *mediaStream );
}

void Session::addMediaStreamSender( MRef<MediaStream *> mediaStream ){
        mediaStreamSendersLock.lock();
	mediaStreamSenders.push_back( *mediaStream );
        mediaStreamSendersLock.unlock();
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

string Session::getCallId(){
        return callId;
}

void Session::setCallId( const string callId ){
        this->callId = callId;
}

void Session::sendDtmf( uint8_t symbol ){
        uint32_t * ts = new uint32_t;
        *ts = 0;
        dtmfTOProvider.request_timeout( 0, &dtmfSender, new DtmfEvent( symbol, 10, 0, false, true, ts ) );
        dtmfTOProvider.request_timeout( 5, &dtmfSender, new DtmfEvent( symbol, 10, 0, false, false, ts ) );
        dtmfTOProvider.request_timeout( 10, &dtmfSender, new DtmfEvent( symbol, 10, 0, false, false, ts ) );
        
        dtmfTOProvider.request_timeout( 15, &dtmfSender, new DtmfEvent( symbol, 10, 800, true, false, ts ) );
        dtmfTOProvider.request_timeout( 20, &dtmfSender, new DtmfEvent( symbol, 10, 800, true, false, ts ) );
        dtmfTOProvider.request_timeout( 25, &dtmfSender, new DtmfEvent( symbol, 10, 800, true, false, ts, true ) );
        
}
