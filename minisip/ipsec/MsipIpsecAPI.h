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
#ifndef MINISIPIPSEC_H
#define MINISIPIPSEC_H

#include<libmikey/keyagreement.h>
#include<libmikey/MikeyPayloadSP.h>
#include"../sip/SipDialogSecurityConfig.h"
#include<libmutil/MemObject.h>
#include<libmsip/SipMIMEContent.h>


class MsipIpsecRequest{
	public:
		MsipIpsecRequest(struct sockaddr * src, struct sockaddr * dst, int so, u_int32_t seq, int otype);
		~MsipIpsecRequest();
		virtual int set()=0;	// -1 = error, 0 = already exist
		virtual int update()=0;	// -1 = error, 0 = no change
		virtual int remove()=0;	// -1 = error, 0 = don't exist
		bool exist;
		int so;
		u_int32_t seq;
		struct sockaddr *src;
		struct sockaddr *dst;
		int otype; // (0 = policy, 1 = sa)

};


class MsipIpsecSA : public MsipIpsecRequest{
	public:
		MsipIpsecSA(int so, u_int satype, u_int mode, u_int32_t reqid, u_int32_t seq, 
				struct sockaddr * src, struct sockaddr * dst,
				u_int32_t spi = 0,
				u_int e_type = 0,
				u_int a_type = 0,
				u_int e_keylen = 0,
				u_int a_keylen = 0,
				char * e_key = NULL,
				char * a_key = NULL,
				u_int wsize = 64,
				u_int flags = 0,
				u_int32_t l_alloc = 1000,
				u_int64_t l_bytes = 1073741824,
				u_int64_t l_addtime = 1073741824,
				u_int64_t l_usetime = 1073741824 );
		~MsipIpsecSA();
		
		virtual int set();
		virtual int update();
		virtual int remove();

		u_int satype;
		u_int mode;	// IPSEC SA mode
		u_int32_t reqid;// reqid: id of who owns this SA
		u_int32_t spi;
		u_int e_type, a_type;
		u_int e_keylen, a_keylen;// The length in bytes of the key
		char * e_key;
		char * a_key;
		u_int wsize; // window size for replay protection
		u_int flags;
		// the number of different connections, endpoints, or flows that the association may be allocated towards before it expires
		u_int32_t l_alloc;
		// number of bytes that may be processed using this security association before it expires
		u_int64_t l_bytes;
		// the number of seconds after the creation of the association until it expires.
		u_int64_t l_addtime;
		// the number of seconds after the first use of the association until it expires.
		u_int64_t l_usetime;

	private:
		 
};

class MsipIpsecPolicy : public MsipIpsecRequest{
	public:
		MsipIpsecPolicy(int so, struct sockaddr * src, struct sockaddr * dst, u_int proto,
				char * policy, int policylen, u_int32_t seq,
				u_int prefs = 32, u_int prefd = 32);
		~MsipIpsecPolicy();
		
		virtual int set();
		virtual int update();
		virtual int remove();

		u_int prefs, prefd, proto;
		char * policy;
		int policylen;
};


class MsipIpsecAPI : public MObject{
        public:
		MsipIpsecAPI(string localIp, SipDialogSecurityConfig &securityConfig);
		~MsipIpsecAPI();
		virtual std::string getMemObjectType(){return "MsipIpsecAPI";}

		// Get initial MIKEY offer
		MRef<SipMimeContent*> getMikeyIpsecOffer();
		// Handle received offer
		bool setMikeyIpsecOffer(MRef<SipMimeContent*> MikeyM);
		// Build answer
		MRef<SipMimeContent*> getMikeyIpsecAnswer();
		// Handle received answer
                bool setMikeyIpsecAnswer(MRef<SipMimeContent*> MikeyM);
		//Remove SA and policy from kernel, -1 == error
		int stop();
		//Write SA and policy to kernel -1 == error
		int start();

	private:


		// Get and reserve a SPI to offer
		uint32_t getOfferSPI();
		//Set requested SA in CS_ID_MAP
		void addSAToKa(uint8_t policyNo);
		// Set parameters from keyagreement. Ipsec then start with start()
		bool initMSipIpsec();
		// Set parameters from keyagreement and start Ipsec
		int setMSipIpsec();
		

		// Find seq for SA with spi, 1 = don't exist 0 = error
		uint32_t findSeqSPI(uint32_t spi);
		// Find Requst for SA with spi, NULL = don't exist
		MsipIpsecSA * findReqSPI(uint32_t spi);

		// Authenticate the offered message
		bool responderAuthenticate( string b64Message );
		bool initiatorAuthenticate( string b64Message );

		u_int32_t reqid;
		SipDialogSecurityConfig securityConfig;
		uint32_t localIp; //Network byte order
		MRef<KeyAgreement *> ka;
		int so; 	// IPSEC kernel socket number
		u_int32_t seq; 	// Contains the sequence number of this message. This field, along with sadb_msg_pid, MUST be used to 					// uniquely identify requests to a process. The sender is responsible for filling in this field. This 					// responsibility also includes matching the sadb_msg_seq of a request (e.g. SADB_ACQUIRE).
		list <MsipIpsecRequest*> madeREQ; // matching of seq to made requests
		
		
};


#endif












