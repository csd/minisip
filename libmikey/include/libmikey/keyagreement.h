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

#ifndef KEYAGREEMENT_H
#define KEYAGREEMENT_H
#include<assert.h>
#include<iostream>
#include<libmutil/MemObject.h>
#include<libmikey/MikeyDefs.h>
#include<libmikey/keyvalidity.h>
#include<libmikey/MikeyCsIdMap.h>

// different type of key derivation defined in MIKEY
#define KEY_DERIV_TEK          0
#define KEY_DERIV_SALT         1
#define KEY_DERIV_TRANS_ENCR   2
#define KEY_DERIV_TRANS_SALT   3
#define KEY_DERIV_TRANS_AUTH   4

#define KEY_AGREEMENT_TYPE_DH 	0
#define KEY_AGREEMENT_TYPE_PSK 	1
#define KEY_AGREEMENT_TYPE_PK 	2


// Class to hold Security Policy (SP) info
class Policy_type {
	public:
		Policy_type(uint8_t policy_No, uint8_t prot_type, uint8_t policy_type, uint16_t length, byte_t * value);
		~Policy_type();
		uint8_t policy_No;
		uint8_t prot_type;
		uint8_t policy_type;
		uint16_t length;
		byte_t * value;
	private:
};


class KeyAgreement : public MObject{
	public:
		KeyAgreement();
		~KeyAgreement();

		/* Type of key agreement (DH, PSK, PKE) */
		int32_t type(){ return typeValue; };

		/* RAND value exchanged during the key agreement */
		unsigned int randLength();
		byte_t * rand();
		void setRand( byte_t * randData, int randLength );

		/* TEK and SALT values, derived from the TGK */
		void genTek( byte_t cs_id,
			     byte_t * tek, unsigned int tek_length );
		void genSalt( byte_t cs_id,
			      byte_t * salt, unsigned int salt_length );

		/* CSB ID: should be random in most cases and generated
		 * by the initiator */
		unsigned int csbId();
		virtual void setCsbId( unsigned int );

		/* CS ID map: matches crypto protocol id and CS-id */
		void setCsIdMapType(uint8_t type);
		uint8_t getCsIdMapType();
		MRef<MikeyCsIdMap *> csIdMap();
		void setCsIdMap( MRef<MikeyCsIdMap *> idMap );

		/* Number of cryptosessions (updated when adding streams) */
		byte_t nCs();
		void setnCs(uint8_t value);

		/* TGK */
		void setTgk( byte_t * tgk, unsigned int tgkLength );
		unsigned int tgkLength();
		byte_t * tgk();

		/* KeyValidity information, exchanged during the key 
		 * agreement. NULL by default */
		MRef<KeyValidity *> keyValidity();
		void setKeyValidity( MRef<KeyValidity *> kv );


		/* Access the initiator and responder key agreement data
		 * (MIKEY messages when using MIKEY) */
		void * initiatorData();
		void setInitiatorData( void * );
		void * responderData();
		void setResponderData( void * );


		/* Security Policy 
		 */	
		std::list <Policy_type *> policy; //Contains the security policy
		//Set the first Parameter Type in a new security policy. Returns the new Policy number.
		uint8_t setPolicyParamType(uint8_t prot_type, uint8_t policy_type, uint16_t length, byte_t * value);
		//Add or modify a parameter in an existing policy
		void setPolicyParamType(uint8_t policy_No, uint8_t prot_type, uint8_t policy_type, uint16_t length, byte_t * value);
		//Create a default policy 
		uint8_t setdefaultPolicy(uint8_t prot_type);
		//Get a policy entry
		Policy_type * getPolicyParamType(uint8_t policy_No, uint8_t prot_type, uint8_t policy_type);
		//For those common cases were the policy type value just is an uint8_t
		//Only use this function if you know the policy type exist or it is not 0
		uint8_t getPolicyParamTypeValue(uint8_t policy_No, uint8_t prot_type, uint8_t policy_type);
		list <Policy_type *> * getPolicy() { return &policy; }


		std::string authError();
		void setAuthError( std::string error );

		virtual std::string getMemObjectType(){return "KeyAgreement";}

		/* SRTP Specific */

		/* Get the CSID given the RTP SSRC */
		byte_t getSrtpCsId( uint32_t ssrc );
		uint32_t getSrtpRoc( uint32_t ssrc );
		uint8_t findpolicyNo( uint32_t ssrc );

		/* Add an SRTP stream to protect to the CSID map 
		 * If csId == 0, add (initiator), else modify existing
		 * (responder) */
		void addSrtpStream( uint32_t ssrc, uint32_t roc=0, 
				    byte_t policyNo=0, byte_t csId=0 );


	protected:
		void keyDeriv( byte_t cs_id, unsigned int csb_id,
		        	byte_t * inkey, unsigned int inkey_length,
		        	byte_t * key, unsigned int key_length,
			   	int type );
		byte_t * tgkPtr;
		unsigned int tgkLengthValue;
		byte_t * randPtr;
		unsigned int randLengthValue;

		unsigned int csbIdValue;
		int32_t typeValue;

		MRef<KeyValidity *> kvPtr;
		MRef<MikeyCsIdMap *> csIdMapPtr;
		uint8_t nCsValue;
		uint8_t	CsIdMapType;


		void * initiatorDataPtr;
		void * responderDataPtr;

		std::string authErrorValue;
};



#endif
