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
 * Authors: Israel Abad <i_abad@terra.es>
 *          Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/


#ifndef CRYPTOCONTEXT_H
#define CRYPTOCONTEXT_H

#include<config.h>


#ifndef NO_SECURITY

#include"RtpPacket.h"
#include<libmutil/aes.h>
#include<libmutil/hmac.h>
#include<libmutil/MemObject.h>
#include<stdint.h>

//#include"../keyagreement/keyagreement.h"
#define REPLAY_WINDOW_SIZE 64

enum encr_method{ 
	no_encr,
	aes_cm,
	aes_f8
};

enum auth_method{
	no_auth,
	hmacsha1
};

class CryptoContext : public MObject{
	public:
		CryptoContext( uint32_t ssrc );
#if 0
		CryptoContext( int roc, int key_deriv_rate,
				enum encr_method encryption, 
				enum auth_method authentication,
				KeyAgreement * key_agreement );
#endif
		CryptoContext( uint32_t ssrc, int roc, int key_deriv_rate,
				enum encr_method encryption, 
				enum auth_method authentication,
				unsigned char * master_key,
				unsigned int master_key_length,
				unsigned char * master_salt,
				unsigned int master_salt_length );
		~CryptoContext();
		
		void set_roc( unsigned int roc );
		unsigned int get_roc();

		void rtp_encrypt( RtpPacket * rtp, uint64_t index );
		void rtp_authenticate( RtpPacket * rtp, unsigned char * tag );
		void derive_srtp_keys( uint64_t index );
		uint64_t guess_index( unsigned short new_seq_nb );
		bool check_replay( unsigned short new_seq_nb );
		void update( unsigned short new_seq_nb );

		int get_tag_length();
		int get_mki_length();

		virtual std::string getMemObjectType(){return "CryptoContext";};

		uint32_t getSsrc();

	private:

		uint32_t ssrc;
		bool using_mki;
		unsigned int mki_length;
		unsigned char * mki;

		unsigned int roc;
		unsigned int guessed_roc;
		unsigned short s_l;
		unsigned int key_deriv_rate;

		/* bitmask for replay check */
		uint64_t replay_window;

		unsigned char * master_key;
		unsigned int master_key_length;
		unsigned int master_key_srtp_use_nb;
		unsigned int master_key_srtcp_use_nb;
		unsigned char * master_salt;
		unsigned int master_salt_length;
		
		/* Session Encryption, Authentication keys, Salt */
		int n_e;
		unsigned char * k_e;
		int n_a;
		unsigned char * k_a;
		int n_s;
		unsigned char * k_s;
		
		enum encr_method encryption;
		enum auth_method authentication;
};

#endif //NO_SECURITY

#endif


