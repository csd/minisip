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



#ifndef MIKEYPAYLOADSP_H
#define MIKEYPAYLOADSP_H

#include<libmikey/libmikey_config.h>

#include<libmikey/MikeyPayload.h>
#include<list>
#include<libmutil/MemObject.h>

#define MIKEYPAYLOAD_SP_PAYLOAD_TYPE 10
//
// Constants for SRTP
//
#define MIKEY_PROTO_SRTP		0
//SRTP encryption algorithms (RFC 3830)
#define MIKEY_SRTP_EALG_NULL		0
#define MIKEY_SRTP_EALG_AESCM		1
#define MIKEY_SRTP_EALG_AESF8		2
//SRTP authentication algorithms (RFC 3830)
#define MIKEY_SRTP_AALG_NULL		0
#define MIKEY_SRTP_AALG_SHA1HMAC	1
//SRTP pseudo-random (RFC 3830)
#define MIKEY_SRTP_PRF_AESCM		0
//FEC order (RFC 3830)
#define MIKEY_FEC_ORDER_FEC_SRTP	0
// Policy Param type for SRTP (RFC 3830)
#define MIKEY_SRTP_EALG			0
#define MIKEY_SRTP_EKEYL		1
#define MIKEY_SRTP_AALG			2
#define MIKEY_SRTP_AKEYL		3
#define MIKEY_SRTP_SALTKEYL		4
#define MIKEY_SRTP_PRF			5
#define MIKEY_SRTP_KEY_DERRATE		6
#define MIKEY_SRTP_ENCR_ON_OFF		7
#define MIKEY_SRTCP_ENCR_ON_OFF		8
#define MIKEY_SRTP_FEC_ORDER		9   
#define MIKEY_SRTP_AUTH_ON_OFF 		10
#define MIKEY_SRTP_AUTH_TAGL		11
#define MIKEY_SRTP_PREFIX		12    
//
// Constants for IPSEC
//
#define MIKEY_PROTO_IPSEC4		7
//IPSEC encryption algorithms (RFC 2367)
#define MIKEY_IPSEC_EALG_NONE		0
#define MIKEY_IPSEC_EALG_DESCBC		2
#define MIKEY_IPSEC_EALG_3DESCBC	3
#define MIKEY_IPSEC_EALG_NULL		11
//IPSEC authentication algorithms (RFC 2367)
#define MIKEY_IPSEC_AALG_NONE		0
#define MIKEY_IPSEC_AALG_MD5HMAC	2
#define MIKEY_IPSEC_AALG_SHA1HMAC	3
// IPSEC SA type (RFC 2367)
#define MIKEY_IPSEC_SATYPE_UNSPEC	0
#define MIKEY_IPSEC_SATYPE_AH		2
#define MIKEY_IPSEC_SATYPE_ESP		3
// IPSEC SA type 
#define MIKEY_IPSEC_MODE_ANY		0
#define MIKEY_IPSEC_MODE_TRANSPORT	1
#define MIKEY_IPSEC_MODE_TUNNEL		2
// options defined for SA
#define MIKEY_IPSEC_SAFLAG_NONE		0x0000	 //i.e. new format. 
#define MIKEY_IPSEC_SAFLAG_OLD		0x0001	 //old format. 
#define MIKEY_IPSEC_SAFLAG_IV4B		0x0010	 //IV length of 4 bytes in use 
#define MIKEY_IPSEC_SAFLAG_DERIV	0x0020	 //DES derived 
#define MIKEY_IPSEC_SAFLAG_CYCSEQ	0x0040	 //allowing to cyclic sequence. 
	 //three of followings are exclusive flags each them 
#define MIKEY_IPSEC_SAFLAG_PSEQ		0x0000	 //sequencial padding for ESP 
#define MIKEY_IPSEC_SAFLAG_PRAND	0x0100	 //random padding for ESP 
#define MIKEY_IPSEC_SAFLAG_PZERO	0x0200	 //zero padding for ESP 
#define MIKEY_IPSEC_SAFLAG_PMASK	0x0300	 //mask for padding flag 
#define MIKEY_IPSEC_SAFLAG_RAWCPI	0x0080	 //use well known CPI (IPComp) 
// Policy Param type for IPSEC
#define MIKEY_IPSEC_SATYPE		0
#define MIKEY_IPSEC_MODE		1
#define MIKEY_IPSEC_SAFLAG		2
#define MIKEY_IPSEC_EALG		3
#define MIKEY_IPSEC_EKEYL		4
#define MIKEY_IPSEC_AALG		5
#define MIKEY_IPSEC_AKEYL		6



/**
 * @author Erik Eliasson, Johan Bilien, Joachim Orrblad
*/


class LIBMIKEY_API MikeyPolicyParam{
        public:
                MikeyPolicyParam( uint8_t type, uint8_t length, byte_t * value );
		~MikeyPolicyParam();
                uint8_t type;	// As defined above
                uint8_t length;	// Length of value in bytes
                byte_t * value; // type value
};

class LIBMIKEY_API MikeyPayloadSP : public MikeyPayload{
	public:
		//Constructor when constructing new MikeyPayloadSP message, policy type entries added later with MikeyPayloadSP::addMikeyPolicyParam
		MikeyPayloadSP(uint8_t policy_no, uint8_t prot_type);
		//Constructor when receiving Mikey message i.e. contruct MikeyPayloadSP from bytestream.
		MikeyPayloadSP(byte_t *start_of_header, int lengthLimit);
		//Destructor
		~MikeyPayloadSP();
		//Add one policy type with type, size & value 
		void addMikeyPolicyParam( uint8_t type, uint8_t length, byte_t * value);
		//Get policy param with type type
		MikeyPolicyParam * getParameterType(uint8_t type);
		//Generate bytestream
		virtual void writeData(byte_t *start, int expectedLength);
		//Return total length of the MikeyPayloadSP data in bytes
		virtual int length();
		//Return number of policy param entries
		int noOfPolicyParam();
		std::string debugDump();

		uint8_t policy_no;
		uint8_t prot_type;
		
	private:
		//Delete the MikeyPolicyParam in list<MikeyPolicyParam *> param with type type
		void deleteMikeyPolicyParam(uint8_t type);
		
		//Total length of all policy params in bytes
		uint16_t policy_param_length;
		//Container holding policy params
		std::list<MikeyPolicyParam *> param;
		//MRef<MikeyPolicyParamList *> ParamListPtr;
};


#endif
