/*
  Copyright (C) 2005, 2004 Erik Eliasson, Johan Bilien, Joachim Orrblad
  
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
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/*
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
 *	    Joachim Orrblad <joachim@orrblad.com>
*/


#ifndef KEYAGREEMENT_H
#define KEYAGREEMENT_H

#include<libmikey/libmikey_config.h>
#include<libmikey/MikeyDefs.h>

#include<assert.h>

#include<libmutil/MemObject.h>
#include<libmikey/KeyValidity.h>
#include<libmikey/MikeyCsIdMap.h>
#include<libmikey/MikeyMessage.h>
#include<libmcrypto/SipSim.h>

#include<iostream>
// different type of key derivation defined in MIKEY
#define KEY_DERIV_TEK          0
#define KEY_DERIV_SALT         1
#define KEY_DERIV_TRANS_ENCR   2
#define KEY_DERIV_TRANS_SALT   3
#define KEY_DERIV_TRANS_AUTH   4
#define KEY_DERIV_ENCR		5
#define KEY_DERIV_AUTH		6

#define KEY_AGREEMENT_TYPE_DH 	0
#define KEY_AGREEMENT_TYPE_PSK 	1
#define KEY_AGREEMENT_TYPE_PK 	2
#define KEY_AGREEMENT_TYPE_DHHMAC 3
#define KEY_AGREEMENT_TYPE_RSA_R 4

class MikeyMessage;

// Class to hold Security Policy (SP) info
class LIBMIKEY_API Policy_type {
	public:
		Policy_type(uint8_t policy_No, uint8_t prot_type, uint8_t policy_type, uint8_t length, byte_t * value);
		~Policy_type();
		uint8_t policy_No;
		uint8_t prot_type;
		uint8_t policy_type;
		uint8_t length;
		byte_t * value;
	private:
};


class LIBMIKEY_API ITgk{
	public:
		virtual ~ITgk();
		/**
		 * If tgk == NULL, generate random TGK of specified size
		 */
		virtual void setTgk( byte_t * tgk, unsigned int tgkLength )=0;
		virtual unsigned int tgkLength()=0;
		virtual byte_t * tgk()=0;
};

class LIBMIKEY_API KeyAgreement : public MObject,
				  public virtual ITgk{
	public:
		KeyAgreement();
		KeyAgreement(MRef<SipSim *> s);
		~KeyAgreement();

		/* Type of key agreement (DH, PSK, PKE) */
		virtual int32_t type()=0;

		/* RAND value exchanged during the key agreement */
		unsigned int randLength();
		byte_t * rand();
		void setRand( byte_t * randData, int randLength );

		/* TEK and SALT values, derived from the TGK */
		void genTek( byte_t cs_id,
			     byte_t * tek, unsigned int tek_length );
		void genSalt( byte_t cs_id,
			      byte_t * salt, unsigned int salt_length );
		
		void genEncr( byte_t cs_id,
			      byte_t * e_key, unsigned int e_keylength );
		void genAuth( byte_t cs_id,
			      byte_t * a_key, unsigned int a_keylength );
		/* CSB ID: should be random in most cases and generated
		 * by the initiator */
		unsigned int csbId();
		virtual void setCsbId( unsigned int );

		/* CS ID map: matches crypto protocol id and CS-id */
		void setCsIdMapType(uint8_t type);
		uint8_t getCsIdMapType();
		MRef<MikeyCsIdMap *> csIdMap();
		void setCsIdMap( MRef<MikeyCsIdMap *> idMap );

		/* Number of cryptosessions (updated when adding streams) (...or IPsec SA) */
		byte_t nCs();
		void setnCs(uint8_t value);

		/* TGK */
		/**
		 * If tgk == NULL, generate random TGK of specified size
		 */
		void setTgk( byte_t * tgk, unsigned int tgkLength );
		unsigned int tgkLength();
		byte_t * tgk();

		/* KeyValidity information, exchanged during the key 
		 * agreement. NULL by default */
		MRef<KeyValidity *> keyValidity();
		void setKeyValidity( MRef<KeyValidity *> kv );


		/* Access the initiator and responder key agreement data
		 * (MIKEY messages when using MIKEY) */
		MRef<MikeyMessage *> initiatorData();
		void setInitiatorData( MRef<MikeyMessage *> );
		MRef<MikeyMessage *> responderData();
		void setResponderData( MRef<MikeyMessage *> );


		//Set the first Parameter Type in a new security policy. Returns the new Policy number.
		uint8_t setPolicyParamType(uint8_t prot_type, uint8_t policy_type, uint8_t length, byte_t * value);
		//Add or modify a parameter in an existing policy
		void setPolicyParamType(uint8_t policy_No, uint8_t prot_type, uint8_t policy_type, uint8_t length, byte_t * value);
		//Create a default policy 
		uint8_t setdefaultPolicy(uint8_t prot_type);
		//Get a policy entry
		Policy_type * getPolicyParamType(uint8_t policy_No, uint8_t prot_type, uint8_t policy_type);
		//For those common cases were the policy type value just is an uint8_t
		//Only use this function if you know the policy type exist or it is not 0
		uint8_t getPolicyParamTypeValue(uint8_t policy_No, uint8_t prot_type, uint8_t policy_type);
		std::list <Policy_type *> * getPolicy() { return &policy; }


		std::string authError();
		void setAuthError( std::string error );

		const std::string &uri() const;
		void setUri( const std::string &uri );

		const std::string &peerUri() const;
		void setPeerUri( const std::string &peerUri );

		virtual std::string getMemObjectType() const {return "KeyAgreement";}

		/* IPSEC Specific */
		void addIpsecSA( uint32_t spi, uint32_t spiSrcaddr, uint32_t spiDstaddr, 
				byte_t policyNo, byte_t csId = 0);
		
		/* SRTP Specific */

		/* Get the CSID given the RTP SSRC */
		byte_t getSrtpCsId( uint32_t ssrc );
		uint32_t getSrtpRoc( uint32_t ssrc );
		uint8_t findpolicyNo( uint32_t ssrc );

		/* Set the parametter in an existing CS (used
		 * by the receiver */
		void setSrtpStreamSsrc( uint32_t ssrc, uint8_t csId );
		void setSrtpStreamRoc( uint32_t roc, uint8_t csId );

		/* Add an SRTP stream to protect to the CSID map 
		 * If csId == 0, add (initiator), else modify existing
		 * (responder) */
		void addSrtpStream( uint32_t ssrc, uint32_t roc=0, 
				    byte_t policyNo=0, byte_t csId=0 );

		virtual MikeyMessage* createMessage()=0;
		
		bool useSim;
		MRef<SipSim *> getSim();

	protected:
		void keyDeriv( byte_t cs_id, unsigned int csb_id,
		        	byte_t * inkey, unsigned int inkey_length,
		        	byte_t * key, unsigned int key_length,
			   	int type );

		MRef<SipSim *> sim;

	private:
		/* Security Policy 
		 */	
		std::list <Policy_type *> policy; //Contains the security policy

		byte_t * tgkPtr;
		unsigned int tgkLengthValue;
		byte_t * randPtr;
		unsigned int randLengthValue;

		unsigned int csbIdValue;

		MRef<KeyValidity *> kvPtr;
		MRef<MikeyCsIdMap *> csIdMapPtr;
		uint8_t nCsValue;
		uint8_t	CsIdMapType;


		MRef<MikeyMessage *> initiatorDataPtr;
		MRef<MikeyMessage *> responderDataPtr;

		std::string authErrorValue;

		std::string uriValue;
		std::string peerUriValue;
};

//If we don't include MikeyMessage, then any user
//of this header file will not compile without including
//it.
#include<libmikey/MikeyMessage.h>


#endif
