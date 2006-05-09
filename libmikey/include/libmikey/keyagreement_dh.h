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
 *          Jie Chen <iw03_jch@it.kth.se>
*/


#ifndef KEYAGREEMENT_DH_H
#define KEYAGREEMENT_DH_H

#include<libmikey/libmikey_config.h>

#include<openssl/dh.h>
#include<libmikey/keyagreement.h>
#include<libmikey/oakley_groups.h>

#define DH_GROUP_OAKLEY5 0
#define DH_GROUP_OAKLEY1 1
#define DH_GROUP_OAKLEY2 2



class certificate_chain;
class certificate;
class ca_db;

class LIBMIKEY_API KeyAgreementDH : public KeyAgreement{
	public:
		KeyAgreementDH( MRef<certificate_chain *> cert, 
				MRef<ca_db *> ca_db, MRef<ca_db *> topCa_db, MRef<ca_db *> crl_db );
		KeyAgreementDH( MRef<certificate_chain *> cert, 
				MRef<ca_db *> ca_db, MRef<ca_db *> topCa_db, MRef<ca_db *> crl_db, int group );
		~KeyAgreementDH();

		int computeTgk();
		int setGroup( int group );
		int group();
		
		void setPeerKey( byte_t * peerKey, int peerKeyLength );
		int peerKeyLength();
		byte_t * peerKey();
		
		int publicKeyLength();
		byte_t * publicKey();
		
		MRef<certificate *> cert();
		MRef<certificate_chain *> certificateChain();
		MRef<certificate_chain *> peerCertificateChain();
		ca_db_item * peerCertificateItem();
		void addPeerCertificate( MRef<certificate *> cert );
		
		bool verifiedPeerCert;
		std::string peerUri();
		int peerCertVerification();
		int peerCertChainNamesubordination();
		int peerCertChainRevocation();
		int controlPeerCertificate();
		void savePeerCertificate();
		void updatePeerCertificateCache( MRef<certificate *> peerCert );
		void updateCrlCache( MRef<crl *> CRL );
			
		MikeyMessage * parseResponse( MikeyMessage * response);
		void setOffer( MikeyMessage * offer );
		MikeyMessage * buildResponse( MikeyMessage * offer);
		bool authenticate( MikeyMessage * msg);
		MRef<ca_db *> topCaDbPtr;
		MRef<ca_db *> crlDbPtr;
		
	private:
		int groupValue;
		DH * opensslDhPtr;
		byte_t * peerKeyPtr;
		int peerKeyLengthValue;
		MRef<certificate *> CertPtr;
		MRef<certificate_chain *> certChainPtr;
		MRef<certificate_chain *> peerCertChainPtr;
		MRef<ca_db *> certDbPtr;
		//MRef<ca_db *> topCaDbPtr;
};

#endif
