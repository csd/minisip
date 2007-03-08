/*
  Copyright (C) 2005, 2004 Erik Eliasson, Pan Xuan
  
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
 *          Pan Xuan <xuan@kth.se>
*/


#ifndef SIPSIMSMARTCARDGD_H
#define SIPSIMSMARTCARDGD_H 

#include <vector>
#include <libmcrypto/SmartCard.h>
#include <libmcrypto/SipSim.h>
#include <libmcrypto/SipSimDh.h>
#include <libmcrypto/SipSimPk.h>


class SipSimSmartCardGD : public SmartCard, public SipSimDh, public SipSimPk {
public:

	SipSimSmartCardGD();
	
	~SipSimSmartCardGD();

	/* select MIKEY applet APDU */
	bool selectMikeyApp();

	/* SIM card Pin related functions */
	bool verifyPin(int verifyMode);
	bool changePin(const char * newPinCode);

	/*
	 * @return 0: unverified   1: user mode   2: admin mode
	*/
	int isVerified(){return verifiedCard;}


/* 
   General SIM functions needed for MIKEYs. Before executing those functions, the host has been verified and
   a smart card connection has been established.
*/

	/** 
	 * This method returns pointer which points to an expected length of random value 
	 * It is the user's responsibility to free the allocated memory by calling delete.
	 * the randomLength is in bits
	 */
	bool getRandomValue(unsigned char * randomPtr, unsigned long randomLength);    

	bool getSignature(unsigned char * dataPtr, int dataLength, unsigned char *signaturePtr, int& signatureLength, 
			bool doHash, int hash_alg=HASH_SHA1);

	/** 
	 * This method returns the tek which is calculated in the pseudo random function 
	 * implemented on smart card
	 */
	bool getKey(unsigned char csId, unsigned long csbIdValue,
		    unsigned char * randPtr, unsigned long randLength,
		    unsigned char * tekPtr, unsigned long tekLength, int keyType);

	bool genTgk( unsigned char * dhpubPtr, unsigned long dhpubLength ); 

	/*virtual bool getTekPk(unsigned char csId, unsigned long csbIdValue,
						  unsigned long tgkLength, unsigned char * tgkPtr,
	                      unsigned long & tekLength, unsigned char * tekPtr);*/
	           

	/** 
	 * Diffie-Hellman specific SIM functions 
	 */
	virtual bool getDHPublicValue(unsigned long & dhPublicValueLength, unsigned char * dhPublickValuePtr);
	

/* Public key based SIM functions */
//	bool getPke();
//	bool getKemac();

//	/*to get traffic generation key from random number generator*/ 
//	bool getTgk(unsigned short tgkLength, unsigned char * tgkPtr);

	/*to get transport keys from the pseudo random function*/
//	bool getTransportEncryKey();
//	bool getTransportAuthKey();
//	bool getTransportSaltKey();


      /* public key relevant functions*/
	bool generateKeyPair();

	/*keyPairType: 0 get modulus; 1 get exponent*/
	//bool getPublicKey(unsigned long publicKeyLength, unsigned char * publicKeyPtr, int keyPairType);
   	bool getPublicKey(unsigned char * publicKeyPtr, int keyPairType);


private:
	/* deallocate the memory assigend for sendBuffer and recvBuffer */
	void clearBuffer();

	/* This is used each time right before you read from or write on smart card. 
	To check out whether user has been verified with a right PIN code 
	0: unverified   1: user mode   2: admin mode*/
	int verifiedCard;
	
	/* three times for both userPinCode attempt and adminPinCode attempt */
	int userAttemptTimer;
	int adminAttemptTimer;

	/* the card is blocked if three times failure on the pinCode attempts   0: unblocked   1: blocked   2: unusable 
	access level 1 can degrade to 0 inputing a right adminPinCode */
	int blockedCard;
	
	/* Command APDU length */
	unsigned long sendBufferLength;
	
	/* Response APDU length */
	unsigned long recvBufferLength;

	/* Command APDU content */
	unsigned char * sendBuffer;
	
	/* Response APDU content */
	unsigned char * recvBuffer;
	
	/* sw1 and sw2 value__return state of R-APDU */
	unsigned short sw_1_2;

};

#endif

