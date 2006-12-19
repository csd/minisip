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

/* Copyright (C) 2004 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
 *	    Joachim Orrblad <joachim@orrblad.com>
*/

#include <config.h>

#include<libminisip/mediahandler/Session.h>
#include<libminisip/mediahandler/MediaStream.h>

#include<libmutil/Timestamp.h>
#include<libmutil/dbg.h>

#include<libmikey/keyagreement.h>
#include<libmikey/keyagreement_dh.h>
#include<libmikey/keyagreement_psk.h>
#include<libmikey/MikeyException.h>

#ifdef _WIN32_WCE
#	include"../include/minisip_wce_extra_includes.h"
#endif

#define MIKEY_PROTO_SRTP	0


using namespace std;


bool Session::responderAuthenticate( string message ){
	
	bool authenticated;
	
	if(message.substr(0,6) == "mikey "){

		string b64Message = message.substr(6, message.length()-6);

		if( message == "" )
			throw MikeyException( "No MIKEY message received" );
		else {
			try{
				MikeyMessage * init_mes = MikeyMessage::parse(b64Message);
				
//				MikeyMessage * resp_mes = NULL;
				switch( init_mes->type() ){
					case MIKEY_TYPE_DH_INIT:

						if( !identity->getSim() || identity->getSim()->getCertificateChain().isNull() /*securityConfig.cert.isNull()*/ ){
							merr << "No certificate available" << end;
						//	throw MikeyExceptionUnacceptable(
						//			"Cannot handle DH key agreement, no certificate" );
							/*securityConfig.*/secured = false;
							/*securityConfig.*/ka_type = KEY_MGMT_METHOD_NULL;
							return false;
						}
							

						if( /*!securityConfig.dh_enabled*/ !identity->dhEnabled ){
							merr << "Cannot handle DH key agreement" << end;
							//throw MikeyExceptionUnacceptable(
							//		"Cannot handle DH key agreement" );
							/*securityConfig.*/secured = false;
							/*securityConfig.*/ka_type = KEY_MGMT_METHOD_NULL;
							return false;
						}

						if( !ka ){
							ka = new KeyAgreementDH( /*securityConfig.cert*/ identity->getSim()->getCertificateChain(), 
									/*securityConfig.cert_db*/ identity->getSim()->getCAs(), DH_GROUP_OAKLEY5 );
						}
						ka->setInitiatorData( init_mes );

#ifdef ENABLE_TS
						ts.save( AUTH_START );
#endif
						if( init_mes->authenticate( ((KeyAgreementDH *)*ka) ) ){
							merr << "Authentication of the DH init message failed" << end;
//							throw MikeyExceptionAuthentication(
//								"Authentication of the DH init message failed" );
							merr << ka->authError() << end;
							/*securityConfig.*/secured = false;
							/*securityConfig.*/ka_type = KEY_MGMT_METHOD_NULL;
							return false;
						}

						merr << "Authentication successful, controling the certificate" << end;

#ifdef ENABLE_TS
						ts.save( TMP );
#endif
						if( /*securityConfig.check_cert*/ identity->checkCert ){
							if( ((KeyAgreementDH *)*ka)->controlPeerCertificate() == 0){
#ifdef DEBUG_OUTPUT
								merr << "Certificate check failed in the incoming MIKEY message" << end;
#endif
								/*securityConfig.*/secured = false;
								/*securityConfig.*/ka_type = KEY_MGMT_METHOD_NULL;
								return false;
							}
						}
#ifdef ENABLE_TS
						ts.save( AUTH_END );
#endif

						/*securityConfig.*/ka_type = KEY_MGMT_METHOD_MIKEY_DH;

						break;
					case MIKEY_TYPE_PSK_INIT:
						if( /*!securityConfig.psk_enabled*/ !identity->pskEnabled ){
							//throw MikeyExceptionUnacceptable(
							//		"Cannot handle PSK key agreement" );

							/*securityConfig.*/secured = false;
							/*securityConfig.*/ka_type = KEY_MGMT_METHOD_NULL;
							return false;
						}

							// ka keeps own local copy of private key
						ka = new KeyAgreementPSK( /*securityConfig.psk*/ (byte_t*)identity->getPsk().c_str(), /*securityConfig.psk_length*/ identity->getPsk().size() );
						ka->setInitiatorData( init_mes );
						
#ifdef ENABLE_TS
						ts.save( AUTH_START );
#endif

						if( init_mes->authenticate( ((KeyAgreementPSK *)*ka) ) ){
//							throw MikeyExceptionAuthentication(
//								"Authentication of the PSK init message failed" );
							/*securityConfig.*/secured = false;
							/*securityConfig.*/ka_type = KEY_MGMT_METHOD_NULL;
							return false;
						}
						
#ifdef ENABLE_TS
						ts.save( AUTH_END );
#endif

						/*securityConfig.*/ka_type = KEY_MGMT_METHOD_MIKEY_PSK;
						break;
					case MIKEY_TYPE_PK_INIT:
						//throw MikeyExceptionUnimplemented(
						//	"Public Key key agreement not implemented" );
						/*securityConfig.*/secured = false;
						/*securityConfig.*/ka_type = KEY_MGMT_METHOD_NULL;
						return false;
					default:
						merr << "Unexpected type of message in INVITE" << end;
						/*securityConfig.*/secured = false;
						/*securityConfig.*/ka_type = KEY_MGMT_METHOD_NULL;
						return false;
				}

				/*securityConfig.*/secured = true;
				authenticated = true;
			}
			catch( certificate_exception & ){
				// TODO: Tell the GUI
				merr << "Could not open certificate" <<end;
				/*securityConfig.*/ka_type = KEY_MGMT_METHOD_NULL;
				/*securityConfig.*/secured = false;
				authenticated = false;
			}
			catch( MikeyExceptionUnacceptable &exc ){
				merr << "MikeyException caught: "<<exc.what()<<end;
				//FIXME! send SIP Unacceptable with Mikey Error message
				/*securityConfig.*/ka_type = KEY_MGMT_METHOD_NULL;
				/*securityConfig.*/secured = false;
				authenticated = false;
			}
			// Authentication failed
			catch( MikeyExceptionAuthentication &exc ){
				merr << "MikeyExceptionAuthentication caught: "<<exc.what()<<end;
				//FIXME! send SIP Authorization failed with Mikey Error message
				/*securityConfig.*/ka_type = KEY_MGMT_METHOD_NULL;
				/*securityConfig.*/secured = false;
				authenticated = false;
			}
			// Message was invalid
			catch( MikeyExceptionMessageContent &exc ){
				MikeyMessage * error_mes;
				merr << "MikeyExceptionMesageContent caught: " << exc.what() << end;
				if( ( error_mes = exc.errorMessage() ) != NULL ){
					//FIXME: send the error message!
				}
				/*securityConfig.*/ka_type = KEY_MGMT_METHOD_NULL;
				/*securityConfig.*/secured = false;
				authenticated = false;
			}
			catch( MikeyException & exc ){
				merr << "MikeyException caught: " << exc.what() << end;
				/*securityConfig.*/ka_type = KEY_MGMT_METHOD_NULL;
				/*securityConfig.*/secured = false;
				authenticated = false;
			}
		
		}
	}
	else {
		merr << "Unknown type of key agreement" << end;
		/*securityConfig.*/ka_type = KEY_MGMT_METHOD_NULL;
		/*securityConfig.*/secured = false;
		authenticated = true;
	}
	return authenticated;
}

string Session::responderParse(){
	
	if( ! ( /*securityConfig.*/ka_type & KEY_MGMT_METHOD_MIKEY ) ){
		merr << "Unknown type of key agreement" << end;
		/*securityConfig.*/secured = false;
		return "";
	}
	
	MikeyMessage * responseMessage = NULL;
	MikeyMessage * initMessage = (MikeyMessage *)ka->initiatorData();

	if( initMessage == NULL ){
		merr << "Uninitialized message, this is a bug" << end;
		/*securityConfig.*/secured = false;
		return "";
	}
	
	try{
#ifdef ENABLE_TS
		ts.save( MIKEY_PARSE_START );
#endif
				
		addStreamsToKa( false );

		responseMessage = initMessage->buildResponse( *ka );
#ifdef ENABLE_TS
		ts.save( MIKEY_PARSE_END );
#endif
	}
	catch( certificate_exception & ){
		// TODO: Tell the GUI
		merr << "Could not open certificate" <<end;
		/*securityConfig.*/ka_type = KEY_MGMT_METHOD_NULL;
		/*securityConfig.*/secured = false;
	}
	catch( MikeyExceptionUnacceptable & exc ){
		merr << "MikeyException caught: "<<exc.what()<<end;
		//FIXME! send SIP Unacceptable with Mikey Error message
		/*securityConfig.*/ka_type = KEY_MGMT_METHOD_NULL;
		/*securityConfig.*/secured = false;
	}
	// Message was invalid
	catch( MikeyExceptionMessageContent & exc ){
		MikeyMessage * error_mes;
		merr << "MikeyExceptionMesageContent caught: " << exc.what() << end;
		if( ( error_mes = exc.errorMessage() ) != NULL ){
			responseMessage = error_mes;
		}
		/*securityConfig.*/ka_type = KEY_MGMT_METHOD_NULL;
		/*securityConfig.*/secured = false;
	}
	catch( MikeyException & exc ){
		merr << "MikeyException caught: " << exc.what() << end;
		/*securityConfig.*/ka_type = KEY_MGMT_METHOD_NULL;
		/*securityConfig.*/secured = false;
	}

	if( responseMessage != NULL ){
		//merr << "Created response message" << responseMessage->get_string() << end;
		return responseMessage->b64Message();
	}
	else{
		//merr << "No response message" << end;
		return string("");
	}


}


string Session::initiatorCreate(){
	MikeyMessage * message;
	
	
	try{
		switch( /*securityConfig.*/ka_type ){
			case KEY_MGMT_METHOD_MIKEY_DH:
				//if( !securityConfig.cert || securityConfig.cert->is_empty() ){
				if( !identity->getSim() || !identity->getSim()->getCertificateChain() ){
					throw MikeyException( "No certificate provided for DH key agreement" );
				}
#ifdef ENABLE_TS
				ts.save( DH_PRECOMPUTE_START );
#endif

				if( ka && ka->type() != KEY_AGREEMENT_TYPE_DH ){
					ka = NULL;
				}
				if( !ka ){
					ka = new KeyAgreementDH( /*securityConfig.cert*/ identity->getSim()->getCertificateChain() , 
							/*securityConfig.cert_db*/ identity->getSim()->getCAs(), 
							DH_GROUP_OAKLEY5 );
				}
				addStreamsToKa();
#ifdef ENABLE_TS
				ts.save( DH_PRECOMPUTE_END );
#endif
				message = MikeyMessage::create( ((KeyAgreementDH *)*ka) );
#ifdef ENABLE_TS
				ts.save( MIKEY_CREATE_END );
#endif
				break;
			case KEY_MGMT_METHOD_MIKEY_PSK:
#ifdef ENABLE_TS
				ts.save( DH_PRECOMPUTE_START );
#endif
					//ka stores local copy of key
				ka = new KeyAgreementPSK( /*securityConfig.psk*/ (byte_t*)identity->getPsk().c_str(), 
						/*securityConfig.psk_length*/ identity->getPsk().size() );
				addStreamsToKa();
#ifdef ENABLE_TS
				ts.save( DH_PRECOMPUTE_END );
#endif
				((KeyAgreementPSK *)*ka)->generateTgk();
#ifdef ENABLE_TS
				ts.save( MIKEY_CREATE_START );
#endif
				message = MikeyMessage::create( ((KeyAgreementPSK *)*ka) );
#ifdef ENABLE_TS
				ts.save( MIKEY_CREATE_END );
#endif
				break;
			case KEY_MGMT_METHOD_MIKEY_PK:
				throw MikeyExceptionUnimplemented(
						"PK KA type not implemented" );
			default:
				throw MikeyException( "Invalid type of KA" );
		}
		
		string b64Message = message->b64Message();
		delete message;
		return "mikey "+b64Message;
	}
	catch( certificate_exception & ){
		// FIXME: tell the GUI
		merr << "Could not open certificate" <<end;
		/*securityConfig.*/ka_type = KEY_MGMT_METHOD_NULL;
		/*securityConfig.*/secured = false;
		return "";
	}
	catch( MikeyException & exc ){
		merr << "MikeyException caught: " << exc.what() << end;
		/*securityConfig.*/ka_type = KEY_MGMT_METHOD_NULL;
		/*securityConfig.*/secured=false;
		return "";
	}
}
			
bool Session::initiatorAuthenticate( string message ){

		
	if (message.substr(0,6) == "mikey ")
	{

		// get rid of the "mikey "
		message = message.substr(6,message.length()-6);
		if(message == ""){
			merr << "No MIKEY message received" << end;
			/*securityConfig.*/secured = false;
			return false;
		} else {
			try{
				MikeyMessage * resp_mes = MikeyMessage::parse( message );
				ka->setResponderData( resp_mes );

				switch( /*securityConfig.*/ka_type ){
					case KEY_MGMT_METHOD_MIKEY_DH:
						
#ifdef ENABLE_TS
						ts.save( AUTH_START );
#endif
						if( resp_mes->authenticate( ((KeyAgreementDH *)*ka) ) ){
							throw MikeyExceptionAuthentication(
							  "Authentication of the DH response message failed" );
						}
						
#ifdef ENABLE_TS
						ts.save( TMP );
#endif
						if( /*securityConfig.check_cert*/ identity->checkCert ){
							if( ((KeyAgreementDH *)*ka)->controlPeerCertificate() == 0)
								throw MikeyExceptionAuthentication(
									"Certificate control failed" );
						}
#ifdef ENABLE_TS
						ts.save( AUTH_END );
#endif
						/*securityConfig.*/secured = true;
						return true;

						/*
						if( resp_mes->get_type() == MIKEY_TYPE_DH_RESP )
							((MikeyMessageDH*)resp_mes)->parse_response((KeyAgreementDH *)(key_agreement));
						else
							throw MikeyExceptionMessageContent(
								"Unexpected MIKEY Message type" );
						
						((KeyAgreementDH *)key_agreement)->compute_tgk();*/
						
					case KEY_MGMT_METHOD_MIKEY_PSK:

#ifdef ENABLE_TS
						ts.save( AUTH_START );
#endif
						if( resp_mes->authenticate( ((KeyAgreementPSK *)*ka) ) ){
							throw MikeyExceptionAuthentication(
							"Authentication of the PSK verification message failed" );
						}
#ifdef ENABLE_TS
						ts.save( AUTH_END );
#endif
					/*	
						if( resp_mes->get_type() == MIKEY_TYPE_PSK_RESP )
							((MikeyMessagePSK*)resp_mes)->parse_response((KeyAgreementPSK *)(key_agreement));
						else
							throw MikeyExceptionMessageContent(
								"Unexpected MIKEY Message type" );
						
						break;*/
						/*securityConfig.*/secured = true;
						return true;

					case KEY_MGMT_METHOD_MIKEY_PK:
						throw MikeyExceptionUnimplemented(
								"PK type of KA unimplemented" );
					default:
						throw MikeyException(
								"Invalid type of KA" );
				}

				//transii->getDialog()->getPhone()->log(LOG_INFO, "Negociated the TGK: " + print_hex( key_agreement->get_tgk(), key_agreement->get_tgk_length() ) );
			}
			catch(MikeyExceptionAuthentication &exc){
				merr << "MikeyException caught: " << exc.what() << end;
				//FIXME! send SIP Authorization failed with Mikey Error message
				/*securityConfig.*/ka_type = KEY_MGMT_METHOD_NULL;
				/*securityConfig.*/secured=false;
				return false;
			}
			catch(MikeyExceptionMessageContent &exc){
				MikeyMessage * error_mes;
				merr << "MikeyExceptionMessageContent caught: " << exc.what() << end;
				if( ( error_mes = exc.errorMessage() ) != NULL ){
					//FIXME: send the error message!
				}
				/*securityConfig.*/ka_type = KEY_MGMT_METHOD_NULL;
				/*securityConfig.*/secured=false;
				return false;
			}
				
			catch(MikeyException &exc){
				merr << "MikeyException caught: " << exc.what() << end;
				/*securityConfig.*/ka_type = KEY_MGMT_METHOD_NULL;
				/*securityConfig.*/secured=false;
				return false;
			}
		}
	}
	else{
		merr << "Unknown key management method" << end;
		/*securityConfig.*/secured = false;
		return false;
	}

}

string Session::initiatorParse(){


	if( ! ( /*securityConfig.*/ka_type & KEY_MGMT_METHOD_MIKEY ) ){
		merr << "Unknown type of key agreement" << end;
		/*securityConfig.*/secured = false;
		return "";
	}
	
	MikeyMessage * responseMessage = NULL;
	
	try{
		MikeyMessage * initMessage = (MikeyMessage *)ka->responderData();

		if( initMessage == NULL ){
			merr << "Uninitialized MIKEY init message, this is a bug" << end;
			/*securityConfig.*/ka_type = KEY_MGMT_METHOD_NULL;
			/*securityConfig.*/secured = false;
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
	catch( certificate_exception & ){
		// TODO: Tell the GUI
		merr << "Could not open certificate" <<end;
		/*securityConfig.*/ka_type = KEY_MGMT_METHOD_NULL;
		/*securityConfig.*/secured = false;
	}
	catch( MikeyExceptionUnacceptable &exc ){
		merr << "MikeyException caught: "<<exc.what()<<end;
		//FIXME! send SIP Unacceptable with Mikey Error message
		/*securityConfig.*/ka_type = KEY_MGMT_METHOD_NULL;
		/*securityConfig.*/secured = false;
	}
	// Message was invalid
	catch( MikeyExceptionMessageContent &exc ){
		MikeyMessage * error_mes;
		merr << "MikeyExceptionMesageContent caught: " << exc.what() << end;
		if( ( error_mes = exc.errorMessage() ) != NULL ){
			responseMessage = error_mes;
		}
		/*securityConfig.*/ka_type = KEY_MGMT_METHOD_NULL;
		/*securityConfig.*/secured = false;
	}
	catch( MikeyException & exc ){
		merr << "MikeyException caught: " << exc.what() << end;
		/*securityConfig.*/ka_type = KEY_MGMT_METHOD_NULL;
		/*securityConfig.*/secured = false;
	}

	if( responseMessage != NULL )
		return responseMessage->b64Message();
	else
		return string("");

}

void Session::addStreamsToKa( bool initiating ){
	list< MRef<MediaStreamSender *> >::iterator iSender;
	ka->setCsIdMapType(HDR_CS_ID_MAP_TYPE_SRTP_ID);
	uint8_t j = 1;
	for( iSender = mediaStreamSenders.begin(); 
	     iSender != mediaStreamSenders.end();
	     iSender ++, j++ ){

		if( initiating ){ 
			uint8_t policyNo = ka->setdefaultPolicy( MIKEY_PROTO_SRTP );
			ka->addSrtpStream( (*iSender)->getSsrc(), 0/*ROC*/, 
					policyNo );
			/* Placeholder for the receiver to place his SSRC */
			ka->addSrtpStream( 0, 0/*ROC*/, 
					policyNo );
		}
		else{
			ka->setSrtpStreamSsrc( (*iSender)->getSsrc(), 2*j );
			ka->setSrtpStreamRoc ( 0, 2*j );
		}

	}
}

void Session::setMikeyOffer(){
	MikeyMessage * initMessage = (MikeyMessage *)ka->initiatorData();
	initMessage->setOffer( *ka );
}

