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

#include<libmcrypto/SipSim.h>
#include<libmikey/KeyAgreement.h>

#define DH_GROUP_OAKLEY5 0
#define DH_GROUP_OAKLEY1 1
#define DH_GROUP_OAKLEY2 2


class OakleyDH;
class CertificateChain;
class certificate;
class CertificateSet;

/*
ITgk is an interface implemented by KeyAgreement.
KeyAgreementDH inherits the methods in the interface
both via DHBase and KeyAgreement. MSVC warns that
the version inherited from DHBase is used.

 ITgk
 ! !  
 ! KeyAgreement
 DHBase   !
    !     !
  KeyAgreementDH

  We turn off the warning for this situation so that not every
  compile that includes this file gets those warnings.
*/
#ifdef _MSC_VER
#pragma warning (disable: 4250)
#endif

class LIBMIKEY_API PeerCertificates {
	public:
		PeerCertificates( MRef<CertificateChain*> aCert,
				  MRef<CertificateSet *> aCaDb );
		PeerCertificates( MRef<CertificateChain*> aCert,
				  MRef<CertificateChain*> aPeerCert );
		virtual ~PeerCertificates();
		virtual MRef<CertificateChain *> certificateChain();
		virtual MRef<CertificateChain *> peerCertificateChain();
		virtual void setPeerCertificateChain( MRef<CertificateChain *> chain );
		virtual int controlPeerCertificate( const std::string &peerUri );

	private:
		MRef<CertificateChain *> certChainPtr;
		MRef<CertificateChain *> peerCertChainPtr;
		MRef<CertificateSet *> certDbPtr;
};

class LIBMIKEY_API KeyAgreementDHBase : virtual public ITgk {
	public:
		KeyAgreementDHBase(MRef<SipSim* > sim);
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
		MRef<SipSim*> sim;
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
		KeyAgreementDH( MRef<CertificateChain *> cert, 
				MRef<CertificateSet *> CertificateSet );
		KeyAgreementDH( MRef<SipSim *> sim );
		~KeyAgreementDH();

		int32_t type();

		MikeyMessage* createMessage();

	private:
};

#endif
