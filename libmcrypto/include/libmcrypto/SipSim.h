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
 *  Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Pan Xuan <xuan@kth.se>
*/


#ifndef SIPSIM_H
#define SIPSIM_H 

#include <libmcrypto/config.h>

#include <libmutil/MemObject.h>
#include <libmcrypto/cert.h>

#define HASH_SHA1 1

class LIBMCRYPTO_API SipSim : public virtual MObject {

public:

	SipSim();
	virtual ~SipSim();

	// Signs a message using RSA as defined in PKCS #1
	//
	// @param doHash 	Specifies if a hash of the data should be
	// 			generated before signing.
	virtual bool getSignature(unsigned char * data, 
				int dataLength, 
				unsigned char * signaturePtr, 
				int & signatureLength, 
				bool doHash, 
				int hash_alg=HASH_SHA1) = 0;
	
	/**
	 * Returns cryptographically strong random data 
	 * @return true of success, false otherwise
	 */
	virtual bool getRandomValue(unsigned char * randomPtr, unsigned long randomLength) = 0;

	//virtual bool getDHPublicValue(unsigned long & dhPublicValueLength, unsigned char * dhPublickValuePtra)=0;

	virtual void setCertificateChain(MRef<CertificateChain *> c){certChain = c;}
	virtual MRef<CertificateChain *> getCertificateChain(){return certChain;}

	virtual void setCAs(MRef<CertificateSet*> c){ca_set=c;}
	virtual MRef<CertificateSet *> getCAs(){return ca_set;}
	

protected:
	MRef<CertificateChain *> certChain;
	MRef<CertificateSet *> ca_set;

};

#endif

