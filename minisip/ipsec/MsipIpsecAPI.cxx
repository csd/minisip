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
 * Authors:     Joachim Orrblad <joachim@orrblad.com>
 *
*/

#include <fcntl.h> 
#include <config.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <linux/ipsec.h>
#include "MsipIpsecAPI.h"
#include <libmikey/keyagreement.h>
#include <libmikey/MikeyCsIdMap.h>
#include <libmikey/MikeyMessage.h>
#include <libmikey/MikeyPayloadSP.h>
#include <libmikey/MikeyException.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
extern "C" {
#include "libpfkey.h"
}



//---------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------//
//Minisip IpSec API
//---------------------------------------------------------------------------------------------------//
//Constructor
MsipIpsecAPI::MsipIpsecAPI(string localIpString, SipDialogSecurityConfig &securityConf) {	
	so = pfkey_open();
	assert(so >= 0);
	reqid = (u_int32_t)getpid();
	seq = 3;
	securityConfig = securityConf;
	ka = NULL;
	localIp = inet_addr(localIpString.c_str());
}
//Destructor
MsipIpsecAPI::~MsipIpsecAPI(){
	int i;	
	list <MsipIpsecRequest *>::iterator iter;
	for( iter = madeREQ.begin(); iter != madeREQ.end()  ; iter++ ){
		i = (*iter)->remove();
		if(i == -1)
			fprintf( stderr, "Either a SA or a policy entry was not be removed, fix it!!!");
		delete *iter;
	}
	pfkey_close(so);
	madeREQ.empty();
}

//-MsipIpsecAPI-PUBLIC-------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------//
// Construct offer MIKEY
MRef<SipMimeContent*> MsipIpsecAPI::getMikeyIpsecOffer(){
	MikeyMessage * message;
	try{
		switch( securityConfig.ka_type ){
			case KEY_MGMT_METHOD_MIKEY_DH:
				if( !securityConfig.cert || securityConfig.cert->is_empty() ){
					throw new MikeyException( "No certificate provided for DH key agreement" );
				}
				if( ka && ka->type() != KEY_AGREEMENT_TYPE_DH ){
					ka = NULL;
				}
				if( !ka ){
					ka = new KeyAgreementDH( securityConfig.cert, securityConfig.cert_db, DH_GROUP_OAKLEY5 );
				}
				addSAToKa( ka->setdefaultPolicy(MIKEY_PROTO_IPSEC4) );
				message = new MikeyMessage( ((KeyAgreementDH *)*ka) );
				break;
			case KEY_MGMT_METHOD_MIKEY_PSK:
				ka = new KeyAgreementPSK( securityConfig.psk, securityConfig.psk_length );
				addSAToKa( ka->setdefaultPolicy(MIKEY_PROTO_IPSEC4) );
				((KeyAgreementPSK *)*ka)->generateTgk();
				message = new MikeyMessage( ((KeyAgreementPSK *)*ka) );
				break;
			case KEY_MGMT_METHOD_MIKEY_PK:
				throw new MikeyExceptionUnimplemented(
						"PK KA type not implemented" );
			default:
				throw new MikeyException( "Invalid type of KA" );
		}
		
		string b64Message = message->b64Message();
		delete message;
		return new SipMimeContent("application/mikey", b64Message, "");
	}
	catch( certificate_exception * exc ){
		// FIXME: tell the GUI
		merr << "Could not open certificate" << end;
		securityConfig.ka_type = KEY_MGMT_METHOD_NULL;
		securityConfig.use_ipsec = false;
		return NULL;
	}
	catch( MikeyException * exc ){
		merr << "MikeyException caught: " << exc->message() << end;
		securityConfig.ka_type = KEY_MGMT_METHOD_NULL;
		securityConfig.use_ipsec=false;
		return NULL;
	}
}

//---------------------------------------------------------------------------------------------------//
// Handle offered MIKEY
bool MsipIpsecAPI::setMikeyIpsecOffer(MRef<SipMimeContent*> MikeyM){
	if( MikeyM->getContentType() == "application/mikey" && securityConfig.use_ipsec){
		if( !responderAuthenticate( MikeyM->getString() ) ){
			string errorString =  "Incoming key management message could not be authenticated";
			if( ka ){
				errorString += ka->authError();
			}
			return false;
		}
		else { //Here we set the offer in ka
			MikeyMessage * initMessage = (MikeyMessage *)ka->initiatorData();
			switch( securityConfig.ka_type ){
				case KEY_MGMT_METHOD_MIKEY_DH:
					initMessage->setOffer((KeyAgreementDH *)*ka);
					break;
				case KEY_MGMT_METHOD_MIKEY_PSK:
					initMessage->setOffer((KeyAgreementPSK *)*ka);
					break;
				case KEY_MGMT_METHOD_MIKEY_PK:
					/* Should not happen at this point */
					throw new MikeyExceptionUnimplemented("Public Key key agreement not implemented" );
					break;
				default:
					throw new MikeyExceptionMessageContent("Unexpected type of message in INVITE" );
			}
		}
	}
	else{
		securityConfig.use_ipsec = false;
		securityConfig.ka_type = KEY_MGMT_METHOD_NULL;
		return false;
	}
	cerr <<  "SetMikeyIpsecOffer OK! " << flush << endl;
	return true;
}

//---------------------------------------------------------------------------------------------------//
// Construct responder MIKEY
MRef<SipMimeContent*> MsipIpsecAPI::getMikeyIpsecAnswer(){
	cerr <<  "getMikeyIpsecAnswer 1 " << securityConfig.use_ipsec << flush << endl;
	if( securityConfig.use_ipsec && initMSipIpsec() ){
		cerr <<  "getMikeyIpsecAnswer 2 " << flush << endl;
		// string keyMgmtAnswer;
		// Generate the key management answer message
		if( ! ( securityConfig.ka_type & KEY_MGMT_METHOD_MIKEY ) ){
			merr << "Unknown type of key agreement" << end;
			securityConfig.use_ipsec = false;
			return NULL;
		}
		MikeyMessage * responseMessage = NULL;
		MikeyMessage * initMessage = (MikeyMessage *)ka->initiatorData();
		if( initMessage == NULL ){
			merr << "Uninitialized message, this is a bug" << end;
			securityConfig.use_ipsec = false;
			return NULL;
		}
		try{
			switch( securityConfig.ka_type ){
				case KEY_MGMT_METHOD_MIKEY_DH:
					responseMessage = initMessage->buildResponse((KeyAgreementDH *)*ka);
					break;
				case KEY_MGMT_METHOD_MIKEY_PSK:
					responseMessage = initMessage->buildResponse((KeyAgreementPSK *)*ka);
					break;
				case KEY_MGMT_METHOD_MIKEY_PK:
					/* Should not happen at that point */
					throw new MikeyExceptionUnimplemented(
							"Public Key key agreement not implemented" );
					break;
				default:
					throw new MikeyExceptionMessageContent(
							"Unexpected type of message in INVITE" );
			}
		}
		catch( certificate_exception *exc ){
			// TODO: Tell the GUI
			merr << "Could not open certificate" <<end;
			securityConfig.ka_type = KEY_MGMT_METHOD_NULL;
			securityConfig.use_ipsec = false;
			delete exc;
		}
		catch( MikeyExceptionUnacceptable *exc ){
			merr << "MikeyException caught: "<<exc->message()<<end;
			//FIXME! send SIP Unacceptable with Mikey Error message
			securityConfig.ka_type = KEY_MGMT_METHOD_NULL;
			securityConfig.use_ipsec = false;
			delete exc;
		}
		// Message was invalid
		catch( MikeyExceptionMessageContent *exc ){
			MikeyMessage * error_mes;
			merr << "MikeyExceptionMesageContent caught: " << exc->message() << end;
			if( ( error_mes = exc->errorMessage() ) != NULL )
				responseMessage = error_mes;
			securityConfig.ka_type = KEY_MGMT_METHOD_NULL;
			securityConfig.use_ipsec = false;
			delete exc;
		}
		catch( MikeyException * exc ){
			merr << "MikeyException caught: " << exc->message() << end;
			securityConfig.ka_type = KEY_MGMT_METHOD_NULL;
			securityConfig.use_ipsec = false;
			delete exc;
		}

		if( responseMessage != NULL && securityConfig.use_ipsec){
			if( ka && ka->type() == KEY_AGREEMENT_TYPE_DH )
                                ((KeyAgreementDH *)*ka)->computeTgk();
			cerr << " About to run start" << flush << endl;
			if(start() == -1){
				cerr << " I returned NULL, not good!" << flush << endl;
				return NULL;
			}
			cerr << " About to return responce message!" << flush << endl;
			return new SipMimeContent("application/mikey", responseMessage->b64Message());
		}
	}
	cerr <<  "getMikeyIpsecAnswer 3 " << flush << endl;
	return NULL;
}

//---------------------------------------------------------------------------------------------------//
//Handle responded MIKEY
bool MsipIpsecAPI::setMikeyIpsecAnswer(MRef<SipMimeContent*> MikeyM){

	if (MikeyM->getContentType() != "application/mikey")
		return false;
	if( !initiatorAuthenticate( MikeyM->getString() ) ){
		string errorString = "Could not authenticate the key management message";
		fprintf( stderr, "Auth failed\n");
		return false;
	}
	
	if( ! ( securityConfig.ka_type & KEY_MGMT_METHOD_MIKEY ) ){
		merr << "Unknown type of key agreement" << end;
		securityConfig.use_ipsec = false;
		return false;
	}
	MikeyMessage * responseMessage = NULL;
	try{
		MikeyMessage * initMessage = (MikeyMessage *)ka->responderData();
		if( initMessage == NULL ){
			merr << "Uninitialized MIKEY init message, this is a bug" << end;
			securityConfig.ka_type = KEY_MGMT_METHOD_NULL;
			securityConfig.use_ipsec = false;
			return false;
		}
		switch( securityConfig.ka_type ){
			case KEY_MGMT_METHOD_MIKEY_DH:
				responseMessage = initMessage->parseResponse((KeyAgreementDH *)*ka);
				break;
			case KEY_MGMT_METHOD_MIKEY_PSK:
				responseMessage = initMessage->parseResponse((KeyAgreementPSK *)*ka);
				break;
			case KEY_MGMT_METHOD_MIKEY_PK:
				/* Should not happen at that point */
				throw new MikeyExceptionUnimplemented(
						"Public Key key agreement not implemented" );
				break;
			default:
				throw new MikeyExceptionMessageContent(
						"Unexpected type of message in INVITE" );
		}
	}
	catch( certificate_exception *exc ){
		// TODO: Tell the GUI
		merr << "Could not open certificate" <<end;
		securityConfig.ka_type = KEY_MGMT_METHOD_NULL;
		securityConfig.use_ipsec = false;
		delete exc;
		return false;
	}
	catch( MikeyExceptionUnacceptable *exc ){
		merr << "MikeyException caught: "<<exc->message()<<end;
		//FIXME! send SIP Unacceptable with Mikey Error message
		securityConfig.ka_type = KEY_MGMT_METHOD_NULL;
		securityConfig.use_ipsec = false;
		delete exc;
		return false;
	}
	// Message was invalid
	catch( MikeyExceptionMessageContent *exc ){
		MikeyMessage * error_mes;
		merr << "MikeyExceptionMesageContent caught: " << exc->message() << end;
		if( ( error_mes = exc->errorMessage() ) != NULL ){
			responseMessage = error_mes;
		}
		securityConfig.ka_type = KEY_MGMT_METHOD_NULL;
		securityConfig.use_ipsec = false;
		delete exc;
		return false;
	}
	catch( MikeyException * exc ){
		merr << "MikeyException caught: " << exc->message() << end;
		securityConfig.ka_type = KEY_MGMT_METHOD_NULL;
		securityConfig.use_ipsec = false;
		delete exc;
		return false;
	}
	if( ka && ka->type() == KEY_AGREEMENT_TYPE_DH ){
		((KeyAgreementDH *)*ka)->computeTgk();
	}
	return initMSipIpsec();
}

//---------------------------------------------------------------------------------------------------//
//Remove SA and policy from kernel, -1 = error
int MsipIpsecAPI::stop(){
	int notgood = 0;
	list <MsipIpsecRequest *>::iterator iter;
	for( iter = madeREQ.begin(); iter != madeREQ.end() ; iter++ ){
		if((*iter)->exist)
			if((*iter)->remove() == -1)
				notgood = -1;
	}
	return notgood;
}

//---------------------------------------------------------------------------------------------------//
//Write SA and policy to kernel -1 == error
int MsipIpsecAPI::start(){
	list <MsipIpsecRequest *>::iterator iter;
	for( iter = madeREQ.begin(); iter != madeREQ.end() ; iter++ ){
		cerr << "In the first level!" << flush << endl;
		if(!((*iter)->exist)){
			cerr << "In the second level!" << flush << endl;
			if((*iter)->set() == -1)
				return -1;
		}
	}
	return 0;
}

//---------------------------------------------------------------------------------------------------//
//-MsipIpsecAPI-PRIVATE------------------------------------------------------------------------------//
//Get and reserve a SPI to offer. If 0 id return something went wrong
uint32_t MsipIpsecAPI::getOfferSPI(){
	
	char *srcadr = "0.0.0.0";
	char *dstadr = "0.0.0.0";
	struct sockaddr_in src, dst;
	src.sin_family = AF_INET;
	src.sin_port = htons(0);
	inet_pton(AF_INET, srcadr, &(src.sin_addr));	
	dst.sin_family = AF_INET;
	dst.sin_port = htons(0);
	inet_pton(AF_INET, dstadr, &(dst.sin_addr));
	MsipIpsecSA *sa = new MsipIpsecSA(so, SADB_SATYPE_ESP, IPSEC_MODE_TRANSPORT, reqid, seq++, 
				(struct sockaddr *) &src, (struct sockaddr *) &dst);
	madeREQ.push_back((MsipIpsecRequest*) sa);
	int result = sa->set();
	if (result == -1)
		return 0;
	return result;
}

//---------------------------------------------------------------------------------------------------//
//Set requested SA in CS_ID_MAP
void MsipIpsecAPI::addSAToKa(uint8_t policyNo){
	ka->setCsIdMapType(HDR_CS_ID_MAP_TYPE_IPSEC4_ID);
	uint32_t offerspi = getOfferSPI();
	assert(offerspi);
	ka->addIpsecSA( offerspi, 0, localIp, policyNo);
	/* Placeholder for the receiver to place his SPI */
	ka->addIpsecSA( 0, localIp, 0, policyNo);
}

//---------------------------------------------------------------------------------------------------//
//Set parameters from keyagreement Ipsec then start with start()
bool MsipIpsecAPI::initMSipIpsec(){
	if(ka->getCsIdMapType() == HDR_CS_ID_MAP_TYPE_IPSEC4_ID){
		MikeyCsIdMapIPSEC4 *CsIdMap = (MikeyCsIdMapIPSEC4 *)(*(ka->csIdMap()));
		MikeyIPSEC4Cs *CsId;
		MsipIpsecSA *sa;
		int result, policylen;
		uint8_t policyNo;
		uint32_t spiDstaddr, spiSrcaddr, spi;
		u_int satype, mode, e_type, a_type, e_keylen, a_keylen, flags;
		byte_t * e_key,* a_key;
		char *policy;
		struct sockaddr_in src, dst;
		struct in_addr addr;
		int nCs = (int) ka->nCs();
		cerr <<  "#Cs: " << nCs << flush << endl;
		for(int i = 0 ; i < nCs ; i++){
			CsId = CsIdMap->getCsIdnumber(i+1);
			if(!CsId){
				cerr <<  "Invalid IPSEC CsId: " << flush << endl;
				return false;
			}
			policyNo = CsId->policyNo;
                	spiDstaddr = CsId->spiDstaddr; //Network byte order
			if(spiDstaddr == 0){
				spiDstaddr = localIp;
				CsId->spiDstaddr = localIp;
			}
			spiSrcaddr = CsId->spiSrcaddr; //Network byte order
			if(spiSrcaddr == 0){
				spiSrcaddr = localIp;
				CsId->spiSrcaddr = localIp;
			}
			if (spiDstaddr == spiSrcaddr){
				fprintf( stderr, "Same address!!\n");
				return false;
			}
			spi = CsId->spi;
			if(spi == 0){
				spi = getOfferSPI();
				CsId->spi = spi;
			}
			cerr <<  "SPI: " << spi << " CdId#: " << i+1 << endl;
			sa = findReqSPI(spi);
			if(sa != NULL)
				if( sa->remove() == -1 )
					return false;
			satype 	 = ka->getPolicyParamTypeValue(policyNo, MIKEY_PROTO_IPSEC4, MIKEY_IPSEC_SATYPE);
			mode	 = ka->getPolicyParamTypeValue(policyNo, MIKEY_PROTO_IPSEC4, MIKEY_IPSEC_MODE);
			e_type 	 = ka->getPolicyParamTypeValue(policyNo, MIKEY_PROTO_IPSEC4, MIKEY_IPSEC_EALG);
			a_type 	 = ka->getPolicyParamTypeValue(policyNo, MIKEY_PROTO_IPSEC4, MIKEY_IPSEC_AALG);
			e_keylen = ka->getPolicyParamTypeValue(policyNo, MIKEY_PROTO_IPSEC4, MIKEY_IPSEC_EKEYL);
			a_keylen = ka->getPolicyParamTypeValue(policyNo, MIKEY_PROTO_IPSEC4, MIKEY_IPSEC_AKEYL);
			flags	 = ka->getPolicyParamTypeValue(policyNo, MIKEY_PROTO_IPSEC4, MIKEY_IPSEC_SAFLAG);
			e_key = new byte_t[e_keylen];
			a_key = new byte_t[a_keylen];
			ka->genEncr( i+1, e_key, e_keylen );
			ka->genAuth( i+1, a_key, a_keylen );
			//Making src and dst structs
			src.sin_family = AF_INET;
			src.sin_port   = 0;
			addr.s_addr = spiSrcaddr;
			src.sin_addr = addr;
			dst.sin_family = AF_INET;
			dst.sin_port   = 0;
			addr.s_addr = spiDstaddr;
			dst.sin_addr = addr;
			//Making a SA
			madeREQ.push_back(new MsipIpsecSA(so, satype, mode, reqid, seq++, 
				(struct sockaddr *) &src, (struct sockaddr *) &dst,
				spi, e_type, a_type, e_keylen, a_keylen, (char*)e_key, (char*)a_key, 64, flags,
				1000, 1073741824, 1073741824, 1073741824 ));
			//Making a Traffic Policy. Please make it dynamic
			if (spiDstaddr == localIp)
				policy = "in ipsec esp/transport//require";
			if (spiSrcaddr == localIp)
				policy = "out ipsec esp/transport//require";
			policylen = strlen(policy);
			madeREQ.push_back(new MsipIpsecPolicy(so, 
				(struct sockaddr *) &src, (struct sockaddr *) &dst, 
				0, policy, policylen, seq++, 32, 32 ));
			delete [] e_key;
			delete [] a_key;
		}
		return true;
	}
	else{
		fprintf( stderr, "Wrong CS-id map type");
		return false;
	}
}

//---------------------------------------------------------------------------------------------------//
//Set parameters from keyagreement and start Ipsec -1 == error
int MsipIpsecAPI::setMSipIpsec(){
	if(initMSipIpsec())	
		return start();
	return -1;
}

//---------------------------------------------------------------------------------------------------//
// Find seq for SA with spi, 0 = don't exist
uint32_t MsipIpsecAPI::findSeqSPI(uint32_t spi){
	list <MsipIpsecRequest *>::iterator iter;
	for( iter = madeREQ.begin(); iter != madeREQ.end() ; iter++ ){
		if((*iter)->otype)
			if(((MsipIpsecSA *)(*iter))->spi == spi)
					return (*iter)->seq;
	}
	return 0;
}

//---------------------------------------------------------------------------------------------------//
// Find Req for SA with spi, NULL = don't exist
MsipIpsecSA * MsipIpsecAPI::findReqSPI(uint32_t spi){
	list <MsipIpsecRequest *>::iterator iter;
	for( iter = madeREQ.begin(); iter != madeREQ.end() ; iter++ ){
		if((*iter)->otype)
			if(((MsipIpsecSA *)(*iter))->spi == spi)
					return (MsipIpsecSA *)(*iter);
	}
	return NULL;
}

//---------------------------------------------------------------------------------------------------//
// Authenticate the offered message
bool MsipIpsecAPI::responderAuthenticate( string b64Message ){
	bool authenticated;
	try{
		MikeyMessage * init_mes = new MikeyMessage(b64Message);
		switch( init_mes->type() ){
			case MIKEY_TYPE_DH_INIT:
				if( securityConfig.cert.isNull() ){
					merr << "No certificate available" << end;
					securityConfig.use_ipsec = false;
					securityConfig.ka_type = KEY_MGMT_METHOD_NULL;
					return false;
				}
				if( !securityConfig.dh_enabled ){
					merr << "Cannot handle DH key agreement" << end;
					securityConfig.secured = false;
					securityConfig.ka_type = KEY_MGMT_METHOD_NULL;
					return false;
				}
				if( !ka )
					ka = new KeyAgreementDH( securityConfig.cert, securityConfig.cert_db, DH_GROUP_OAKLEY5 );
				ka->setInitiatorData( init_mes );
				if( init_mes->authenticate( ((KeyAgreementDH *)*ka) ) ){
					merr << "Authentication of the DH init message failed" << end;
					merr << ka->authError() << end;
					securityConfig.use_ipsec = false;
					securityConfig.ka_type = KEY_MGMT_METHOD_NULL;
					return false;
				}
#ifdef DEBUG_OUTPUT
				merr << "Authentication successful, controling the certificate" << end;
#endif
				if( securityConfig.check_cert ){
					if( ((KeyAgreementDH *)*ka)->controlPeerCertificate() == 0){
#ifdef DEBUG_OUTPUT
					merr << "Certificate check failed in the incoming MIKEY message" << end;
#endif
					securityConfig.use_ipsec = false;
					securityConfig.ka_type = KEY_MGMT_METHOD_NULL;
					return false;
					}
				}
				securityConfig.ka_type = KEY_MGMT_METHOD_MIKEY_DH;
				break;
			case MIKEY_TYPE_PSK_INIT:
				if( !securityConfig.psk_enabled ){
					securityConfig.use_ipsec = false;
					securityConfig.ka_type = KEY_MGMT_METHOD_NULL;
					return false;
				}
				ka = new KeyAgreementPSK( securityConfig.psk, securityConfig.psk_length );
				ka->setInitiatorData( init_mes );		
				if( init_mes->authenticate( ((KeyAgreementPSK *)*ka) ) ){
					securityConfig.use_ipsec = false;
					securityConfig.ka_type = KEY_MGMT_METHOD_NULL;
					cerr <<  "responderAuthenticate return false" << flush << endl;
					return false;
				}
				securityConfig.ka_type = KEY_MGMT_METHOD_MIKEY_PSK;
				break;
			case MIKEY_TYPE_PK_INIT:
				securityConfig.secured = false;
				securityConfig.ka_type = KEY_MGMT_METHOD_NULL;
				return false;
			default:
				merr << "Unexpected type of message in INVITE" << end;
				securityConfig.use_ipsec = false;
				securityConfig.ka_type = KEY_MGMT_METHOD_NULL;
				return false;
		}
		securityConfig.use_ipsec = true;
		authenticated = true;
	}
	catch( certificate_exception *exc ){
		// TODO: Tell the GUI
		merr << "Could not open certificate" <<end;
		securityConfig.ka_type = KEY_MGMT_METHOD_NULL;
		securityConfig.use_ipsec = false;
		authenticated = false;
		delete exc;
	}
	catch( MikeyExceptionUnacceptable *exc ){
		merr << "MikeyException caught: "<<exc->message()<<end;
		//FIXME! send SIP Unacceptable with Mikey Error message
		securityConfig.ka_type = KEY_MGMT_METHOD_NULL;
		securityConfig.use_ipsec = false;
		authenticated = false;
		delete exc;
	}
	// Authentication failed
	catch( MikeyExceptionAuthentication *exc ){
		merr << "MikeyExceptionAuthentication caught: "<<exc->message()<<end;
		//FIXME! send SIP Authorization failed with Mikey Error message
		securityConfig.ka_type = KEY_MGMT_METHOD_NULL;
		securityConfig.use_ipsec = false;
		authenticated = false;
		delete exc;
	}
	// Message was invalid
	catch( MikeyExceptionMessageContent *exc ){
		MikeyMessage * error_mes;
		merr << "MikeyExceptionMesageContent caught: " << exc->message() << end;
		if( ( error_mes = exc->errorMessage() ) != NULL ){
		//FIXME: send the error message!
		}
		securityConfig.ka_type = KEY_MGMT_METHOD_NULL;
		securityConfig.use_ipsec = false;
		authenticated = false;
		delete exc;
	}
	catch( MikeyException * exc ){
		merr << "MikeyException caught: " << exc->message() << end;
		securityConfig.ka_type = KEY_MGMT_METHOD_NULL;
		securityConfig.use_ipsec = false;
		authenticated = false;
		delete exc;
	}
	return authenticated;
}


//---------------------------------------------------------------------------------------------------//
// Authenticate the answered message
bool MsipIpsecAPI::initiatorAuthenticate( string message ){

	try{
		if(!securityConfig.use_ipsec)
			return false;
		MikeyMessage * resp_mes = new MikeyMessage( message );
		ka->setResponderData( resp_mes );
		switch( securityConfig.ka_type ){
			case KEY_MGMT_METHOD_MIKEY_DH:	
				if( resp_mes->authenticate( ((KeyAgreementDH *)*ka) ) ){
					throw new MikeyExceptionAuthentication(
						  "Authentication of the DH response message failed" );
					}
				if( securityConfig.check_cert ){
					if( ((KeyAgreementDH *)*ka)->controlPeerCertificate() == 0)
						throw new MikeyExceptionAuthentication(
									"Certificate control failed" );
				}
				securityConfig.use_ipsec = true;
				return true;
			case KEY_MGMT_METHOD_MIKEY_PSK:
				if( resp_mes->authenticate( ((KeyAgreementPSK *)*ka) ) ){
					throw new MikeyExceptionAuthentication(
						"Authentication of the PSK verification message failed" );
				}
				securityConfig.use_ipsec = true;
				return true;
			case KEY_MGMT_METHOD_MIKEY_PK:
				throw new MikeyExceptionUnimplemented(
					"PK type of KA unimplemented" );
			default:
				throw new MikeyException(
					"Invalid type of KA" );
		}
	}
	catch(MikeyExceptionAuthentication *exc){
		merr << "MikeyException caught: " << exc->message() << end;
		//FIXME! send SIP Authorization failed with Mikey Error message
		securityConfig.ka_type = KEY_MGMT_METHOD_NULL;
		securityConfig.use_ipsec=false;
		return false;
	}
	catch(MikeyExceptionMessageContent *exc){
		MikeyMessage * error_mes;
		merr << "MikeyExceptionMessageContent caught: " << exc->message() << end;
		if( ( error_mes = exc->errorMessage() ) != NULL ){
			//FIXME: send the error message!
		}
		securityConfig.ka_type = KEY_MGMT_METHOD_NULL;
		securityConfig.use_ipsec=false;
		return false;
	}		
	catch(MikeyException *exc){
		merr << "MikeyException caught: " << exc->message() << end;
		securityConfig.ka_type = KEY_MGMT_METHOD_NULL;
		securityConfig.use_ipsec=false;
		return false;
	}
}



//---------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------//
//IpSec requests
//---------------------------------------------------------------------------------------------------//
//Constructor
MsipIpsecRequest::MsipIpsecRequest(struct sockaddr *src, struct sockaddr * dst, int so, u_int32_t seq, int otype){
	exist = false;
	this->so = so;
	this->seq = seq;
	//Here might be a good idea to check for ipv6
	struct sockaddr_in *source;
	struct sockaddr_in *destination;
	source = new struct sockaddr_in;
	destination = new struct sockaddr_in;
	source->sin_family = ((struct sockaddr_in *)src)->sin_family;
	source->sin_port   = ((struct sockaddr_in *)src)->sin_port;
	source->sin_addr   = ((struct sockaddr_in *)src)->sin_addr;
	destination->sin_family = ((struct sockaddr_in *)dst)->sin_family;
	destination->sin_port   = ((struct sockaddr_in *)dst)->sin_port;
	destination->sin_addr   = ((struct sockaddr_in *)dst)->sin_addr;
	this->src = (struct sockaddr *) source;
	this->dst = (struct sockaddr *) destination;
	this->otype = otype;
	
}
//Destructor
MsipIpsecRequest::~MsipIpsecRequest(){
	delete dst;
	delete src;
}

//---------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------//
// IpSec SA handling
//---------------------------------------------------------------------------------------------------//
//Constructor
MsipIpsecSA::MsipIpsecSA(int so, u_int satype, u_int mode, u_int32_t reqid, u_int32_t seq, 
				struct sockaddr * src, struct sockaddr * dst,
				u_int32_t spi,
				u_int e_type,
				u_int a_type,
				u_int e_keylen,
				u_int a_keylen,
				char * e_key,
				char * a_key,
				u_int wsize,
				u_int flags,
				u_int32_t l_alloc,
				u_int64_t l_bytes,
				u_int64_t l_addtime,
				u_int64_t l_usetime ):MsipIpsecRequest(src, dst, so, seq, 1){
	this->satype = satype;
	this->mode = mode;
	this->reqid = reqid;
	this->spi = spi;
	this->e_type = e_type;
	this->a_type = a_type;
	this->e_keylen = e_keylen;
	this->a_keylen = a_keylen;
	this->wsize = wsize;
	this->flags = flags;
	this->l_alloc = l_alloc;
	this->l_bytes = l_bytes;
	this->l_addtime = l_addtime;
	this->l_usetime = l_usetime;
	int i;
	this->e_key = (char *) malloc (e_keylen);
	for(i=0; i< e_keylen; i++)
			this->e_key[i] = e_key[i];
	this->a_key = (char *) malloc (a_keylen);
	for(i=0; i< a_keylen; i++)
			this->a_key[i] = a_key[i];
}
//---------------------------------------------------------------------------------------------------//
//Destructor
MsipIpsecSA::~MsipIpsecSA(){
	free(e_key);
	free(a_key);
}

//-MsipIpsecSA-PUBLIC--------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------//
//Set SA into kernel -1 = error, 0 = already exist
int MsipIpsecSA::set(){
	int result, i;
	cerr << "MsipIpsecSA::set()" << flush << endl;
	u_int32_t min = htonl(5000);
	u_int32_t max = htonl(15000);
	struct sadb_msg * msg;
	// There might be a good idea to check the result & return values below!!!
	if(!spi && !exist){
		struct sadb_sa * m_sa;

/* Depending on IPSEC kernel-----------------------------------------------------------**********************/
		result = pfkey_send_getspi(so, satype, mode, src, dst, min, max, reqid, seq);
		msg = pfkey_recv(so);
/* end --------------------------------------------------------------------------------**********************/
		// The result value is -1 wonder why? but it works
		if (result == -1){
			merr << "Problem with IPSEC pfkey_send_getspi" << end;
			return result;
		}
		caddr_t next[18];
		result = pfkey_align (msg, next);
		result = pfkey_check(next);
		m_sa = (struct sadb_sa *)next[SADB_EXT_SA];
		spi = (u_int32_t)ntohl(m_sa->sadb_sa_spi);
		exist = true;
		return (int)spi;
	}
	if(spi && !exist) {
		caddr_t keymat;
		keymat = (caddr_t)malloc(e_keylen + a_keylen);
		strcpy(keymat, e_key);
		strcat(keymat, a_key);
		cerr << "MsipIpsecSA::set() pfkey_send_add" << flush << endl;
/* Depending on IPSEC kernel-----------------------------------------------------------**********************/
		result = pfkey_send_add(so, satype, mode, src, dst, spi, reqid, wsize, 
					keymat, e_type, e_keylen, a_type, a_keylen, 
					flags, l_alloc, l_bytes, l_addtime, l_usetime, seq);
		//msg = pfkey_recv(so);
/* end---------------------------------------------------------------------------------**********************/
		if (result = -1){
			merr << "Problem with IPSEC pfkey_send_add" << end;
			return result;
		}
		exist = true;
		free (keymat);
		return spi;
	}	
	return 0;
}

//---------------------------------------------------------------------------------------------------//
//Update SA in kernel
int MsipIpsecSA::update(){return 0;}

//---------------------------------------------------------------------------------------------------//
//Remove SA from kernel  -1 = error, 0 = don't exist
int MsipIpsecSA::remove(){
	int result;
	struct sadb_msg *msg;
	cerr << "MsipIpsecSA::remove()" << flush << endl;
	if(exist){
/* Depending on IPSEC kernel-----------------------------------------------------------**********************/
		result = pfkey_send_delete (so, satype, mode, src, dst, spi);
		msg = pfkey_recv(so);
/* end --------------------------------------------------------------------------------**********************/
		exist = false;
		return result;
	}
	else
		return 0;
}

//---------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------//
//IpSec policy handling
//---------------------------------------------------------------------------------------------------//
//Constructor
MsipIpsecPolicy::MsipIpsecPolicy(int so, struct sockaddr * src, struct sockaddr * dst, u_int proto,
					char * policy, int policylen, u_int32_t seq,
					u_int prefs, u_int prefd ):MsipIpsecRequest(src, dst, so, seq, 0){
	this->prefs = prefs;
	this->prefd = prefd;
	this->proto = proto;
	this->policylen = policylen;
	this->policy = (char *)malloc(policylen);
	for(int i=0; i< policylen; i++)
			this->policy[i] = policy[i];
}
//---------------------------------------------------------------------------------------------------//
//Destructor
MsipIpsecPolicy::~MsipIpsecPolicy(){
	free(policy);
}

//-MsipIpsecPolicy-PUBLIC----------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------//
//Set policy into kernel -1 = error, 0 = already exist
int MsipIpsecPolicy::set(){
	cerr << "MsipIpsecPolicy::set()" << flush << endl;
	int result;
	struct sadb_msg *msg;
	if(exist)
		return 0;
	caddr_t pol =  ipsec_set_policy(policy,policylen);
	int len = ipsec_get_policylen(policy);
	// There might be a good idea to check the result & return values below!!!
/* Depending on IPSEC kernel-----------------------------------------------------------**********************/
	result = pfkey_send_spdadd(so, src, prefs, dst, prefd, proto, pol, len, seq); 
	msg = pfkey_recv(so);
/* end --------------------------------------------------------------------------------**********************/
	exist = true;
	return result;
}

//---------------------------------------------------------------------------------------------------//
//Update policy in kernel
int MsipIpsecPolicy::update(){return 0;}

//---------------------------------------------------------------------------------------------------//
//Remove policy from kernel -1 = error, 0 = don't exist
int MsipIpsecPolicy::remove(){
	int result;
	struct sadb_msg *msg;
	if(exist){
/* Depending on IPSEC kernel-----------------------------------------------------------**********************/
		result = pfkey_send_spddelete(so, src, prefs, dst, prefd, proto, policy, policylen, seq);
		msg = pfkey_recv(so);
/* end --------------------------------------------------------------------------------**********************/
		exist = false;
		return result;
	}
	return 0;
}










