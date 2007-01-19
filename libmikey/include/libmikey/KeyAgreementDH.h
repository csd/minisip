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


#ifndef KEYAGREEMENT_DH_H
#define KEYAGREEMENT_DH_H

#include<libmikey/libmikey_config.h>

#include<libmikey/KeyAgreement.h>

#define DH_GROUP_OAKLEY5 0
#define DH_GROUP_OAKLEY1 1
#define DH_GROUP_OAKLEY2 2


class OakleyDH;
class certificate_chain;
class certificate;
class ca_db;

class LIBMIKEY_API PeerCertificates {
	public:
		PeerCertificates( MRef<certificate_chain*> aCert,
				  MRef<ca_db *> aCaDb );
		PeerCertificates( MRef<certificate_chain*> aCert,
				  MRef<certificate_chain*> aPeerCert );
		virtual ~PeerCertificates();
		virtual MRef<certificate_chain *> certificateChain();
		virtual MRef<certificate_chain *> peerCertificateChain();
		virtual void setPeerCertificateChain( MRef<certificate_chain *> chain );
		virtual int controlPeerCertificate();

	private:
		MRef<certificate_chain *> certChainPtr;
		MRef<certificate_chain *> peerCertChainPtr;
		MRef<ca_db *> certDbPtr;
};

class LIBMIKEY_API KeyAgreementDHBase: virtual public ITgk{
	public:
		KeyAgreementDHBase();
		~KeyAgreementDHBase();

		int computeTgk();
		int setGroup( int group );
		int group();
		
		void setPeerKey( byte_t * peerKey, int peerKeyLength );
		int peerKeyLength();
		byte_t * peerKey();
		
		int publicKeyLength();
		byte_t * publicKey();

	private:
		OakleyDH * dh;
		byte_t * peerKeyPtr;
		int peerKeyLengthValue;
		byte_t * publicKeyPtr;
		int publicKeyLengthValue;
};

class LIBMIKEY_API KeyAgreementDH : public KeyAgreement,
				    public KeyAgreementDHBase,
				    public PeerCertificates{
	public:
		KeyAgreementDH( MRef<certificate_chain *> cert, 
				MRef<ca_db *> ca_db );
		KeyAgreementDH( MRef<SipSim *> sim );
		~KeyAgreementDH();

		int32_t type();

		MikeyMessage* createMessage();

	private:
};

#endif
