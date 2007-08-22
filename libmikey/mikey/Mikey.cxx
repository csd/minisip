/*
 Copyright (C) 2004-2007 the Minisip Team
 
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

/* Copyright (C) 2004 - 2007
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
 *	    Joachim Orrblad <joachim@orrblad.com>
 *          Mikael Magnusson <mikma@users.sourceforge.net>
*/

#include <config.h>

#include<libmikey/Mikey.h>

#include<libmutil/Timestamp.h>
#include<libmutil/dbg.h>

#include<libmcrypto/SipSim.h>

#include<libmikey/KeyAgreement.h>
#include<libmikey/KeyAgreementDH.h>
#include<libmikey/KeyAgreementPSK.h>
#include<libmikey/KeyAgreementPKE.h>
#include<libmikey/KeyAgreementDHHMAC.h>
#include<libmikey/KeyAgreementRSAR.h>
#include<libmikey/MikeyException.h>
#include<libmikey/MikeyMessage.h>

#ifdef _WIN32_WCE
#	include"../include/minisip_wce_extra_includes.h"
#endif

#define MIKEY_PROTO_SRTP	0

/*
 * TODO
 * Cache D-H
 * Add support for initiating Public-Key method
 */

using namespace std;

IMikeyConfig::~IMikeyConfig(){
}

Mikey::Mikey( MRef<IMikeyConfig*> aConfig ):state(STATE_START),
					    config(aConfig)
{
}

Mikey::~Mikey(){
}

bool Mikey::responderAuthenticate( const string &message,
				   const string &peerUri ){
	
	setState( STATE_RESPONDER );

	if(message.substr(0,6) == "mikey "){

		string b64Message = message.substr(6, message.length()-6);

		if( message == "" )
			throw MikeyException( "No MIKEY message received" );
		else {
			try{
				MRef<MikeyMessage *> init_mes = MikeyMessage::parse(b64Message);
				createKeyAgreement( init_mes->keyAgreementType() );
				if( !ka ){
					throw MikeyException(
						"Can't handle key agreement" );
				}

				ka->setPeerUri( peerUri );
				ka->setInitiatorData( init_mes );
						
#ifdef ENABLE_TS
				ts.save( AUTH_START );
#endif
				if( init_mes->authenticate( *ka ) ){
					string msg = "Authentication of the MIKEY init message failed: " + ka->authError();
					throw MikeyExceptionAuthentication(
						msg.c_str() );
				}
						
#ifdef ENABLE_TS
				ts.save( TMP );
#endif
				if( config->isCertCheckEnabled() ){
					PeerCertificates *peers =
						dynamic_cast<PeerCertificates*>(*ka);
					if( peers ){
						if( peers->controlPeerCertificate( ka->peerUri() ) == 0){
							throw MikeyExceptionAuthentication(
								"Certificate check failed in the incoming MIKEY message" );
						}
					}
				}
#ifdef ENABLE_TS
				ts.save( AUTH_END );
#endif
				secured = true;
				setState( STATE_AUTHENTICATED );
			}
			catch( CertificateException &e ){
				// TODO: Tell the GUI
				merr << "Could not open certificate " << e.what() << endl;
				setState( STATE_ERROR );
			}
			catch( MikeyExceptionUnacceptable &exc ){
				merr << "MikeyException caught: "<<exc.what()<<endl;
				//FIXME! send SIP Unacceptable with Mikey Error message
				setState( STATE_ERROR );
			}
			// Authentication failed
			catch( MikeyExceptionAuthentication &exc ){
				merr << "MikeyExceptionAuthentication caught: "<<exc.what()<<endl;
				//FIXME! send SIP Authorization failed with Mikey Error message
				setState( STATE_ERROR );
			}
			// Message was invalid
			catch( MikeyExceptionMessageContent &exc ){
				MRef<MikeyMessage *> error_mes;
				merr << "MikeyExceptionMesageContent caught: " << exc.what() << endl;
				error_mes = exc.errorMessage();
				if( !error_mes.isNull() ){
					//FIXME: send the error message!
				}
				setState( STATE_ERROR );
			}
			catch( MikeyException & exc ){
				merr << "MikeyException caught: " << exc.what() << endl;
				setState( STATE_ERROR );
			}
		
		}
	}
	else {
		merr << "Unknown type of key agreement" << endl;
		secured = false;
		setState( STATE_AUTHENTICATED );
	}

	return state == STATE_AUTHENTICATED;
}

string Mikey::responderParse(){
	
	if( !ka ){
		merr << "Unknown type of key agreement" << endl;
		setState( STATE_ERROR );
		return "";
	}
	
	MRef<MikeyMessage *> responseMessage = NULL;
	MRef<MikeyMessage *> initMessage = ka->initiatorData();

	if( initMessage.isNull() ){
		merr << "Uninitialized message, this is a bug" << endl;
		setState( STATE_ERROR );
		return "";
	}
	
	try{
#ifdef ENABLE_TS
		ts.save( MIKEY_PARSE_START );
#endif
				
		addStreamsToKa();

		responseMessage = initMessage->buildResponse( *ka );
#ifdef ENABLE_TS
		ts.save( MIKEY_PARSE_END );
#endif
	}
	catch( CertificateException &e ){
		// TODO: Tell the GUI
		merr << "Could not open certificate " << e.what() << endl;
		setState( STATE_ERROR );
	}
	catch( MikeyExceptionUnacceptable & exc ){
		merr << "MikeyException caught: "<<exc.what()<<endl;
		//FIXME! send SIP Unacceptable with Mikey Error message
		setState( STATE_ERROR );
	}
	// Message was invalid
	catch( MikeyExceptionMessageContent & exc ){
		MRef<MikeyMessage *> error_mes;
		merr << "MikeyExceptionMesageContent caught: " << exc.what() << endl;
		error_mes = exc.errorMessage();
		if( !error_mes.isNull() ){
			responseMessage = error_mes;
		}
		setState( STATE_ERROR );
	}
	catch( MikeyException & exc ){
		merr << "MikeyException caught: " << exc.what() << endl;
		setState( STATE_ERROR );
	}

	if( !responseMessage.isNull() ){
		//merr << "Created response message" << responseMessage->get_string() << endl;
		return responseMessage->b64Message();
	}
	else{
		//merr << "No response message" << end;
		return string("");
	}


}


string Mikey::initiatorCreate( int type, const string &peerUri ){
	MRef<MikeyMessage *> message;

	setState( STATE_INITIATOR );
	
	try{
		createKeyAgreement( type );
		if( !ka ){
			throw MikeyException( "Can't create key agreement" );
		}

		ka->setPeerUri( peerUri );
		message = ka->createMessage();

		string b64Message = message->b64Message();
		return "mikey "+b64Message;
	}
	catch( CertificateException &e ){
		// FIXME: tell the GUI
		merr << "Could not open certificate " << e.what() << endl;
		setState( STATE_ERROR );
		return "";
	}
	catch( MikeyException & exc ){
		merr << "MikeyException caught: " << exc.what() << endl;
		setState( STATE_ERROR );
		return "";
	}
}
			
bool Mikey::initiatorAuthenticate( string message ){

		
	if (message.substr(0,6) == "mikey ")
	{

		// get rid of the "mikey "
		message = message.substr(6,message.length()-6);
		if(message == ""){
			merr << "No MIKEY message received" << endl;
			return false;
		} else {
			try{
				MRef<MikeyMessage *> resp_mes = MikeyMessage::parse( message );
				ka->setResponderData( resp_mes );

#ifdef ENABLE_TS
				ts.save( AUTH_START );
#endif
				if( resp_mes->authenticate( *ka ) ){
					throw MikeyExceptionAuthentication(
						"Authentication of the response message failed" );
				}
						
#ifdef ENABLE_TS
				ts.save( TMP );
#endif
				if( config->isCertCheckEnabled() ){
					PeerCertificates *peers =
						dynamic_cast<PeerCertificates*>(*ka);
					if( peers ){
						if( peers->controlPeerCertificate( ka->peerUri() ) == 0){
							throw MikeyExceptionAuthentication(
								"Certificate control failed" );
						}
					}
				}
#ifdef ENABLE_TS
				ts.save( AUTH_END );
#endif
				secured = true;
				setState( STATE_AUTHENTICATED );
			}
			catch(MikeyExceptionAuthentication &exc){
				merr << "MikeyException caught: " << exc.what() << endl;
				//FIXME! send SIP Authorization failed with Mikey Error message
				setState( STATE_ERROR );
			}
			catch(MikeyExceptionMessageContent &exc){
				MRef<MikeyMessage *> error_mes;
				merr << "MikeyExceptionMessageContent caught: " << exc.what() << endl;
				error_mes = exc.errorMessage();
				if( !error_mes.isNull() ){
					//FIXME: send the error message!
				}
				setState( STATE_ERROR );
			}
				
			catch(MikeyException &exc){
				merr << "MikeyException caught: " << exc.what() << endl;
				setState( STATE_ERROR );
			}
		}
	}
	else{
		merr << "Unknown key management method" << endl;
		setState( STATE_ERROR );
	}

	return state == STATE_AUTHENTICATED;
}

string Mikey::initiatorParse(){


	if( !ka ){
		merr << "Unknown type of key agreement" << endl;
		setState( STATE_ERROR );
		return "";
	}
	
	MRef<MikeyMessage *> responseMessage = NULL;
	
	try{
		MRef<MikeyMessage *> initMessage = ka->responderData();

		if( initMessage.isNull() ){
			merr << "Uninitialized MIKEY init message, this is a bug" << endl;
			setState( STATE_ERROR );
			return "";
		}
			
#ifdef ENABLE_TS
		ts.save( MIKEY_PARSE_START );
#endif
		responseMessage = initMessage->parseResponse( *ka );
#ifdef ENABLE_TS
		ts.save( MIKEY_PARSE_END );
#endif

	}
	catch( CertificateException &e ){
		// TODO: Tell the GUI
		merr << "Could not open certificate " << e.what() << endl;
		setState( STATE_ERROR );
	}
	catch( MikeyExceptionUnacceptable &exc ){
		merr << "MikeyException caught: "<<exc.what()<<endl;
		//FIXME! send SIP Unacceptable with Mikey Error message
		setState( STATE_ERROR );
	}
	// Message was invalid
	catch( MikeyExceptionMessageContent &exc ){
		MRef<MikeyMessage *> error_mes;
		merr << "MikeyExceptionMesageContent caught: " << exc.what() << endl;
		error_mes = exc.errorMessage();
		if( !error_mes.isNull() ){
			responseMessage = error_mes;
		}
		setState( STATE_ERROR );
	}
	catch( MikeyException & exc ){
		merr << "MikeyException caught: " << exc.what() << endl;
		setState( STATE_ERROR );
	}

	if( !responseMessage.isNull() ){
		return responseMessage->b64Message();
	}
	else
		return string("");

}

void Mikey::addStreamsToKa(){
	Streams::iterator iSender;
	ka->setCsIdMapType(HDR_CS_ID_MAP_TYPE_SRTP_ID);
	uint8_t j = 1;
	for( iSender = mediaStreamSenders.begin(); 
	     iSender != mediaStreamSenders.end();
	     iSender ++, j++ ){

		uint32_t ssrc = *iSender;

		if( isInitiator() ){ 
			uint8_t policyNo = ka->setdefaultPolicy( MIKEY_PROTO_SRTP );
			ka->addSrtpStream( ssrc, 0/*ROC*/, 
					policyNo );
			/* Placeholder for the receiver to place his SSRC */
			ka->addSrtpStream( 0, 0/*ROC*/, 
					policyNo );
		}
		else{
			ka->setSrtpStreamSsrc( ssrc, 2*j );
			ka->setSrtpStreamRoc ( 0, 2*j );
		}

	}
}

void Mikey::setMikeyOffer(){
	MRef<MikeyMessage *> initMessage = ka->initiatorData();
	initMessage->setOffer( *ka );
}

bool Mikey::error() const{
	return state == STATE_ERROR;
}

bool Mikey::isSecured() const{
	return secured && !error();
}

bool Mikey::isInitiator() const{
	return state == STATE_INITIATOR;
}

MRef<KeyAgreement*> Mikey::getKeyAgreement() const{
	return ka;
}

void Mikey::addSender( uint32_t ssrc ){
	mediaStreamSenders.push_back( ssrc );
}

string Mikey::authError() const{
	return ka ? ka->authError() : "";
}

const std::string &Mikey::peerUri() const{
	static string empty;

	if( state != STATE_AUTHENTICATED )
		return empty;

	return ka->peerUri();
}

void Mikey::setState( State newState ){
	state = newState;
}

void Mikey::createKeyAgreement( int type )
{
	ka = NULL;

	if( !config->isMethodEnabled( type ) ){
		throw MikeyException( "Cannot handle key agreement method" );
	}

	MRef<SipSim*> sim = config->getSim();
	MRef<CertificateChain*> cert_chain =
		sim->getCertificateChain();
	MRef<CertificateChain*> peer_chain;
// 		config->getPeerCertificate();
	MRef<CertificateSet*> cert_db =
		sim->getCAs();
	const byte_t* psk = config->getPsk();
	size_t psk_len = config->getPskLength();

	switch( type ){
		case KEY_AGREEMENT_TYPE_DH:{
			if ( cert_chain.isNull() ){
				throw MikeyException( "No certificate provided for DH key agreement" );
			}

			KeyAgreementDH *kaDH =
				new KeyAgreementDH( sim );

			if( isInitiator() ){
				kaDH->setGroup( DH_GROUP_OAKLEY5 );
			}

			ka = kaDH;
			break;
		}
		case KEY_AGREEMENT_TYPE_PK:
			if( cert_chain.isNull() ){
				throw MikeyException( "No certificate provided for Public-Key method" );
			}

			if( isInitiator() ){
				if( peer_chain.isNull() ){
					throw MikeyException( "No peer certificate provided for Public-Key init" );
				}
				ka = new KeyAgreementPKE( cert_chain, peer_chain );
			}
			else{
				if( cert_db.isNull() ){
					throw MikeyException( "No CA db provided for Public-Key responce" );
				}
				ka = new KeyAgreementPKE( cert_chain, cert_db );
			}
			break;
		case KEY_AGREEMENT_TYPE_RSA_R:
			if( cert_chain.isNull() ){
				throw MikeyException( "No certificate provided for RSA-R method" );
			}

			if( cert_db.isNull() ){
				throw MikeyException( "No CA db provided for RSA-R method" );
			}

			ka = new KeyAgreementRSAR( cert_chain, cert_db );
			break;
		case KEY_AGREEMENT_TYPE_PSK:
		case KEY_AGREEMENT_TYPE_DHHMAC:
			if (!psk || psk_len <= 0) {
				throw MikeyException( "No pre-shared key provided" );
			}

			if( type == KEY_AGREEMENT_TYPE_PSK ){
				ka = new KeyAgreementPSK(psk, (int)psk_len);
			}
			else{
				KeyAgreementDHHMAC *kaDH =
					new KeyAgreementDHHMAC(psk, (int)psk_len);
				if( isInitiator() ){
					kaDH->setGroup( DH_GROUP_OAKLEY5 );
				}

				ka = kaDH;
			}
			break;
		default:
			throw MikeyExceptionUnimplemented( "Unsupported type of KA" );
	}

	if( ka->type() != KEY_AGREEMENT_TYPE_DHHMAC ){
		// Generate TGK for PSK, PK and RSA-R
		KeyAgreementPSK* pskKa =
			dynamic_cast<KeyAgreementPSK*>(*ka);
		if( pskKa ){
			cerr << "Generate Tgk" << endl;
			pskKa->generateTgk();
		}
	}

	ka->setUri( config->getUri() );

	if( isInitiator() ){
		addStreamsToKa();
	}
}

