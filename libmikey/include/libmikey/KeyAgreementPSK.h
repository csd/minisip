/*
  Copyright (C) 2005, 2004 Erik Eliasson, Johan Bilien
  
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
*/


#ifndef KEYAGREEMENT_PSK_H
#define KEYAGREEMENT_PSK_H

#include<libmikey/libmikey_config.h>

#include<libmikey/KeyAgreement.h>



class LIBMIKEY_API KeyAgreementPSK : public KeyAgreement{
	public:
		KeyAgreementPSK( const byte_t * psk, int pskLength );
		virtual ~KeyAgreementPSK();

		int32_t type();

		/**
		 * Generates a TGK of de given length with the random function from the
		 * OpenSSL library and stores it in this instance
		 */
		void generateTgk( uint32_t tgkLength = 192 );

		/**
		 * Generates and stores the transport encryption key of the given length.
		 * It is derived by the envelope key
		 */
		void genTranspEncrKey( byte_t * encrKey, int encrKeyLength );

		/**
		 * Generates and stores the salting key of the given length.
		 * It is also derived by the envelope key
		 */
		void genTranspSaltKey( byte_t * saltKey, int saltKeyLength );
		
		/**
		 * Creates and stores the authentication key to authenticate the MAC/signature
		 * of the MIKEY message.
		 */
		void genTranspAuthKey( byte_t * authKey, int authKeyLength );

		/**
		 * Returns the timestamp on which the message was sent
		 */
		uint64_t tSent();

		/**
		 * Sets the timestamp
		 */
		void setTSent( uint64_t tSent );

		/**
		 * Timestamp on which the message was received
		 */
		uint64_t t_received;

		/**
		 * Authentication key
		 */
		byte_t * authKey;

		/**
		 * Length of the authentication key
		 */
		unsigned int authKeyLength;

		/**
		 * If the V bit is set by the initiator, the responder has to send a
		 * verification message.
		 */
		void setV(int value) {v=value;}

		/**
		 * Used to test if the V bit is set.
		 */
		int getV() {return v;}

		/**
		 * MAC algorithmus (HMAC-SHA1)
		 */
		int macAlg;

		virtual MikeyMessage* createMessage();

	protected:
		KeyAgreementPSK();
		void setPSK( const byte_t* psk, int pskLength );
		byte_t* getPSK();
		int getPSKLength();

	private:
		byte_t * pskPtr;
		int pskLengthValue;

		/**
		 * The V bit
		 */
		int v;

		/**
		 * Timestamp from when the message was sent
		 */
		uint64_t tSentValue;
};

#endif
