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

#include <config.h>

#include<iostream>

#include<libmutil/massert.h>
#include<libminisip/rtp/CryptoContext.h>
#include<libmutil/print_hex.h>
#include<libmikey/MikeyPayloadSP.h>

CryptoContext::CryptoContext( uint32_t ssrc ):
ssrc(ssrc),
using_mki(false),mki_length(0),mki(NULL),
roc(0),guessed_roc(0),s_l(0),key_deriv_rate(0),
replay_window(0),
master_key(NULL), master_key_length(0),
master_key_srtp_use_nb(0), master_key_srtcp_use_nb(0),
master_salt(NULL), master_salt_length(0),
n_e(0),k_e(NULL),n_a(0),k_a(NULL),n_s(0),k_s(NULL),
ealg(MIKEY_SRTP_EALG_NULL), aalg(MIKEY_SRTP_AALG_NULL),
ekeyl(0), akeyl(0), skeyl(0), 
encr(1), auth(1){}/*These should be set to 0, but for backward compatability they are set to 1  *///encryption(no_encr),authentication(no_auth)

CryptoContext::CryptoContext( uint32_t ssrc, int roc, uint16_t seq_no,
			        int key_deriv_rate,
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
				uint8_t tag_length):

ssrc(ssrc),using_mki(false),mki_length(0),mki(NULL),
roc(roc),guessed_roc(0),s_l(seq_no),key_deriv_rate(key_deriv_rate),
replay_window(0),
master_key_srtp_use_nb(0), master_key_srtcp_use_nb(0)
//encryption(encryption),authentication(authentication)
{
	this->ealg = ealg; 
	this->aalg = aalg;
	this->encr = encr;
	this->auth = auth;
	this->ekeyl = ekeyl;
	this->akeyl = akeyl; 
	this->skeyl = skeyl;
	this->master_key_length = master_key_length;
	this->master_key = new unsigned char[ master_key_length ];
	memcpy( this->master_key, master_key, master_key_length );
	
	this->master_salt_length = master_salt_length;
	this->master_salt = new unsigned char[ master_salt_length ];
	memcpy( this->master_salt, master_salt, master_salt_length );
	
	switch( ealg ){
		case MIKEY_SRTP_EALG_NULL:
			n_e = 0;
			k_e = NULL;
			n_s = 0;
			k_s = NULL;
			break;
		case MIKEY_SRTP_EALG_AESCM:
		case MIKEY_SRTP_EALG_AESF8:
			n_e = ekeyl;
			k_e = new unsigned char[ n_e ];
			n_s = skeyl;
			k_s = new unsigned char[ n_s ];
			break;
	}

	switch( aalg ){
		case MIKEY_SRTP_AALG_NULL:
			n_a = 0;
			k_a = NULL;
			this->tag_length = 0;
			break;
		case MIKEY_SRTP_AALG_SHA1HMAC:
			n_a = akeyl;
			k_a = new unsigned char[ n_a ];
			this->tag_length = tag_length;
			break;
	}
}
	
CryptoContext::~CryptoContext(){
	if( mki )
		delete [] mki;

	if( master_key )
		delete [] master_key;

	if( master_salt )
		delete [] master_salt;

	if( k_e )
		delete [] k_e;

	if( k_a )
		delete [] k_a;

	if( k_s )
		delete [] k_s;
}	

void CryptoContext::rtp_encrypt( RtpPacket * rtp, uint64_t index ){
	
	// FIXME: handle f8 mode
	if( ealg == MIKEY_SRTP_EALG_AESCM )
	{
		/* Compute the IV:
		  k_s   XX XX XX XX XX XX XX XX XX XX XX XX XX XX
		  SSRC              XX XX XX XX
		  index                         XX XX XX XX XX XX
		  ------------------------------------------------------XOR
		  IV    XX XX XX XX XX XX XX XX XX XX XX XX XX XX 00 00
		*/
		  
		unsigned char iv[16];
		uint32_t ssrc = rtp->getHeader().getSSRC();
		memcpy( iv, k_s, 4 );
		
		int i;
		for(i = 4; i < 8; i++ ){
			iv[i] = ( 0xFF & ( ssrc >> ((7-i)*8) ) ) ^ k_s[i];
		}

		for(i = 8; i < 14; i++ ){
			iv[i] = ( 0xFF & (unsigned char)( index >> ((13-i)*8) ) ) ^ k_s[i];
		}

		iv[14] = iv[15] = 0;
		
		AES * aes = new AES( k_e, n_e );
		aes->ctr_encrypt( rtp->getContent(),
				  rtp->getContentLength(),
				  iv );
		delete aes;
	}
}

/* Warning: tag must have been initialized */
void CryptoContext::rtp_authenticate( RtpPacket * rtp, uint32_t roc, unsigned char * tag ){
	unsigned int tag_length;
	
	if( aalg == MIKEY_SRTP_AALG_SHA1HMAC )
	{
		unsigned char temp[20];
		unsigned char * chunks[4];
		unsigned int chunkLength[4];
		uint32_t beRoc = hton32( roc );
		
		char * bytes = rtp->getHeader().getBytes();
		unsigned char * content = rtp->getContent();
		unsigned int header_size = rtp->getHeader().size() ;
		unsigned int content_size = rtp->getContentLength() ;
		
		chunks[0] = (unsigned char *)bytes;
		chunkLength[0] = header_size;
		chunks[1] = (unsigned char *)content;
		chunkLength[1] = content_size;
		chunks[2] = (unsigned char *)&beRoc;
		chunkLength[2] = 4;
		chunks[3] = NULL;

//		cerr << "packet no: " << rtp->getHeader().getSeqNo() << endl;
//		cerr << "HMAC input size: " << header_size + content_size << endl;
//		cerr << "HMAC input: " << print_hex( (unsigned char *)bytes, header_size + content_size ) << endl;
		hmac_sha1( k_a, n_a,
		    /* data */ chunks, 
		    /*authenticated part length */	
		    	chunkLength,
		    /* tag */  temp, &tag_length );
		massert( tag_length == 20 );
		/* truncate the result */
		memcpy( tag, temp, get_tag_length() );
		delete [] bytes;
		
	}
}

/* used by the key derivation method */
static void compute_iv( unsigned char * iv, uint64_t label, uint64_t index, int key_deriv_rate, unsigned char * master_salt ){

        uint64_t key_id;

        if( key_deriv_rate == 0 )
                key_id = label << 48;
        else
                key_id = ((label << 48) || index / key_deriv_rate);

        //printf( "Key_ID: %llx\n", key_id );

        /* compute the IV
        key_id:                           XX XX XX XX XX XX XX
        master_salt: XX XX XX XX XX XX XX XX XX XX XX XX XX XX
        ------------------------------------------------------------ XOR
        IV:          XX XX XX XX XX XX XX XX XX XX XX XX XX XX 00 00
        */

	int i;
        for(i = 0; i < 7 ; i++ ){
                iv[i] = master_salt[i];
        }

        for(i = 7; i < 14 ; i++ ){
                iv[i] = (unsigned char)(0xFF & (key_id >> (8*(13-i)))) ^
                        master_salt[i];
        }

        iv[14] = iv[15] = 0;
        //cerr << "AES-CM input: " << print_hex( iv, 16 ) << endl;
}

/* Derives the srtp session keys from the master key */
void CryptoContext::derive_srtp_keys( uint64_t index ){
	AES * aes;
	unsigned char iv[16];
	uint64_t label = 0;

	compute_iv( iv, label, index, key_deriv_rate, master_salt );


        aes = new AES( master_key, 16/*master_key_length*/ );
        aes->get_ctr_cipher_stream( k_e, n_e, iv );

   //     cerr << "Session Encryption Key: " << print_hex( k_e, n_e ) << endl;

        label = 0x01;

        compute_iv( iv, label, index, key_deriv_rate, master_salt );

        aes = new AES( master_key, 16/*master_key_length*/ );
        aes->get_ctr_cipher_stream( k_a, n_a, iv );

//        cerr << "Session Authentication Key: " << print_hex( k_a, n_a ) << endl;

        label = 0x02;

        compute_iv( iv, label, index, key_deriv_rate, master_salt );

        aes = new AES( master_key, 16/*master_key_length*/ );
        aes->get_ctr_cipher_stream( k_s, n_s, iv );

//        cerr << "Session Salt: " << print_hex( k_s, n_s ) << endl;
	delete aes;
}

/* Based on the algorithm provided in Appendix A - draft-ietf-srtp-05.txt */
uint64_t CryptoContext::guess_index( unsigned short new_seq_nb ){
	if( s_l < 32768 ){
		if( new_seq_nb - s_l > 32768 )
			guessed_roc = roc - 1;
		else
			guessed_roc = roc;
	}
	else{
		if( s_l - 32768 > new_seq_nb )
			guessed_roc = roc + 1;
		else
			guessed_roc = roc;
	}

	return ((uint64_t)guessed_roc) << 16 | new_seq_nb;
}

bool CryptoContext::check_replay( unsigned short new_seq_nb ){
	if( aalg == MIKEY_SRTP_AALG_NULL && ealg == MIKEY_SRTP_EALG_NULL ){
		/* No security policy, don't use the replay protection */
		return true;
	}

	uint64_t guessed_index = guess_index( new_seq_nb );
	uint64_t local_index = 
		(((uint64_t)roc) << 16 & 0xFFFF) | s_l;
	
	int64_t delta = guessed_index - local_index;
	//fprintf( stderr, "roc: %u\n", roc );
	//fprintf( stderr, "guessed_index: %llu\n", guessed_index );
	//fprintf( stderr, "local_index: %llu\n", local_index );
	//fprintf( stderr, "s_l: %lli\n", s_l );


	//fprintf( stderr, "delta: %lli\n", delta );
	if( delta > 0 )
		/* Packet not yet received*/
		return true;
	else{
		if( -delta > REPLAY_WINDOW_SIZE )
			/* Packet too old */
			return false;
		else{
			if( (replay_window >> (-delta)) & 0x1 )
				/* Packet already received ! */
				return false;
			else
				/* Packet not yet received */
				return true;
		}
	}
}

void CryptoContext::update( unsigned short new_seq_nb ){
	
	int64_t delta = guess_index( new_seq_nb ) - 
		( ((uint64_t)roc) << 16 | s_l );
	
	/* update the replay bitmask */
	if( delta > 0 ){
		replay_window = replay_window << delta;
		replay_window |= 1;
	}
	else{
		replay_window |= ( 1 << delta );
	}

	/* update the locally stored ROC and highest sequence number */
	if( new_seq_nb > s_l )
		s_l = new_seq_nb;
	if( guessed_roc > roc ){
		roc = guessed_roc;
		s_l = new_seq_nb;
	}
}

void CryptoContext::set_roc( unsigned int roc ){
	this->roc = roc;
}

unsigned int CryptoContext::get_roc(){
	return roc;
}

int CryptoContext::get_tag_length(){
	switch( aalg ){
		case MIKEY_SRTP_AALG_NULL:
			return 0;
		case MIKEY_SRTP_AALG_SHA1HMAC:
			return tag_length;
	}
	return -1;
}

int CryptoContext::get_mki_length(){
	return mki_length;
}

uint32_t CryptoContext::getSsrc(){
	return ssrc;
}

