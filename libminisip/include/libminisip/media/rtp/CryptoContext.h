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
 * Authors: Israel Abad <i_abad@terra.es>
 *          Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
 *	    Joachim Orrblad <joachim@orrblad.com>
*/


#ifndef CRYPTOCONTEXT_H
#define CRYPTOCONTEXT_H

#include<libminisip/libminisip_config.h>

#include<libmcrypto/aes.h>
#include<libmcrypto/hmac.h>
#include<libmutil/MemObject.h>

#include<libminisip/media/rtp/RtpPacket.h>

#define REPLAY_WINDOW_SIZE 64

class LIBMINISIP_API CryptoContext : public MObject{
	public:
		CryptoContext( uint32_t ssrc );

		CryptoContext( uint32_t ssrc, int roc, uint16_t seq_no,
			        int64_t key_deriv_rate,
				//enum encr_method encryption,
				uint8_t ealg,
				//enum auth_method authentication,
				uint8_t aalg,
				unsigned char * master_key,
				unsigned int master_key_length,
				unsigned char * master_salt,
				unsigned int master_salt_length,
				uint8_t ekeyl,
				uint8_t akeyl,
				uint8_t skeyl,
				uint8_t encr,
				uint8_t auth,
				uint8_t tag_length );
		~CryptoContext();

		void set_roc( unsigned int roc );
		unsigned int get_roc();

		void rtp_encrypt( RtpPacket * rtp, uint64_t index );
		void rtp_authenticate( RtpPacket * rtp, uint32_t roc,
				unsigned char * tag );
		void derive_srtp_keys( uint64_t index );
		uint64_t guess_index( unsigned short new_seq_nb );
		bool check_replay( unsigned short new_seq_nb );
		void update( unsigned short new_seq_nb );

		int get_tag_length();
		int get_mki_length();

		virtual std::string getMemObjectType() const  {return "CryptoContext";}

                uint32_t getSsrc() { return ssrc; }

                uint8_t getEalg()  { return ealg; }
                /**
                 * Derive a new Crypto Context for use with a new SSRC
                 *
                 * This method returns a new Crypto Context initialized with the data
                 * of this crypto context. Replacing the SSRC, Roll-over-Counter, and
                 * the key derivation rate the application cab use this Crypto Context
                 * to encrypt / decrypt a new stream (Synchronization source) inside
                 * one RTP session.
                 *
                 * Before the application can use this crypto context it must call
                 * the <code>deriveSrtpKeys</code> method.
                 *
                 * @param ssrc
                 *     The SSRC for this context
                 * @param roc
                 *     The Roll-Over-Counter for this context
                 * @param keyDerivRate
                 *     The key derivation rate for this context
                 * @return
                 *     a new CryptoContext with all relevant data set.
                 */
                CryptoContext* newCryptoContextForSSRC(uint32_t ssrc, int roc, uint16_t seq,
                                                       int64_t keyDerivRate);



	private:

		uint32_t ssrc;
		bool using_mki;
		unsigned int mki_length;
		unsigned char * mki;

		unsigned int roc;
		unsigned int guessed_roc;
		unsigned short s_l;
		int64_t key_deriv_rate;

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

		//enum encr_method encryption;
		//enum auth_method authentication;
		uint8_t ealg;
		uint8_t aalg;
		uint8_t ekeyl;
		uint8_t akeyl;
		uint8_t skeyl;
		uint8_t encr;
		uint8_t auth;
		uint8_t tag_length;
};

#endif


