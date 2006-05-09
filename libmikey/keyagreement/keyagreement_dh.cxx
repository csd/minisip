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


#include<config.h>
#include<libmikey/keyagreement_dh.h>
#include<libmikey/MikeyException.h>
#include<libmnetutil/IP4ServerSocket.h>
#include<libmnetutil/TCPSocket.h>

KeyAgreementDH::KeyAgreementDH( MRef<certificate_chain *> certChainPtr,
		MRef<ca_db *> certDbPtr, MRef<ca_db *> topCaDbPtr, MRef<ca_db *> crlDbPtr ):
	KeyAgreement(),
	peerKeyPtr( NULL ),
	peerKeyLengthValue( 0 ),
	certChainPtr( certChainPtr ),
	certDbPtr( certDbPtr ),
	topCaDbPtr( topCaDbPtr ),
	crlDbPtr( crlDbPtr ){
	//policy = list<Policy_type *>::list();
	typeValue = KEY_AGREEMENT_TYPE_DH;
	opensslDhPtr = DH_new();
	peerCertChainPtr = new certificate_chain();

}

KeyAgreementDH::~KeyAgreementDH(){
	DH_free( opensslDhPtr );
	if( peerKeyPtr == NULL )
		delete [] peerKeyPtr;

}

KeyAgreementDH::KeyAgreementDH( MRef<certificate_chain *> certChainPtr,
		MRef<ca_db *> certDbPtr, MRef<ca_db *> topCaDbPtr, MRef<ca_db *> crlDbPtr, int groupValue ):
	peerKeyPtr( NULL ),
	peerKeyLengthValue( 0 ),
	certChainPtr( certChainPtr ),
	peerCertChainPtr( NULL ),
	certDbPtr( certDbPtr ),
	topCaDbPtr( topCaDbPtr ),
	crlDbPtr( crlDbPtr ){
	//policy = list<Policy_type *>::list();
	typeValue = KEY_AGREEMENT_TYPE_DH;
	opensslDhPtr = DH_new();
	if( opensslDhPtr == NULL )
	{
		throw MikeyException( "Could not create openssl "
				          "DH parameters." );
	}

	if( setGroup( groupValue ) ){
		throw MikeyException( "Could not set the  "
				          "DH group." );
	}
	peerCertChainPtr = new certificate_chain();
}

int KeyAgreementDH::setGroup( int groupValue ){
	this->groupValue = groupValue;
	switch( groupValue ) {
		case DH_GROUP_OAKLEY5:
			BN_hex2bn( &opensslDhPtr->p, OAKLEY5_P );
			BN_hex2bn( &opensslDhPtr->g, OAKLEY5_G );
			tgkLengthValue = OAKLEY5_L;
			break;
		case DH_GROUP_OAKLEY1:
			BN_hex2bn( &opensslDhPtr->p, OAKLEY1_P );
			BN_hex2bn( &opensslDhPtr->g, OAKLEY1_G );
			tgkLengthValue = OAKLEY1_L;
			break;
		case DH_GROUP_OAKLEY2:
			BN_hex2bn( &opensslDhPtr->p, OAKLEY2_P );
			BN_hex2bn( &opensslDhPtr->g, OAKLEY2_G );
			tgkLengthValue = OAKLEY2_L;
			break;
		default:
			return 1;
	}
	if( !DH_generate_key( opensslDhPtr ) )
	{
		return 1;
	}
			
	tgkPtr = new unsigned char[ tgkLengthValue ];

	return 0;
}
	
void KeyAgreementDH::setPeerKey( unsigned char * peerKeyPtr,
			      int peerKeyLengthValue ){
	this->peerKeyPtr = new unsigned char[ peerKeyLengthValue ];
	this->peerKeyLengthValue = peerKeyLengthValue;
	memcpy( this->peerKeyPtr, peerKeyPtr, peerKeyLengthValue );

}

int KeyAgreementDH::publicKeyLength(){
	return BN_num_bytes( opensslDhPtr->pub_key );
}

unsigned char * KeyAgreementDH::publicKey(){
	unsigned char * publicKey;
	publicKey = new unsigned char[ publicKeyLength() ];
	BN_bn2bin( opensslDhPtr->pub_key, publicKey );
	return publicKey;

}

int KeyAgreementDH::computeTgk(){
	BIGNUM * bn_peerKeyPtr =  BN_new();;
	
	assert( peerKeyPtr );

	BN_bin2bn( peerKeyPtr, peerKeyLengthValue, bn_peerKeyPtr );

	if( DH_compute_key( tgkPtr, bn_peerKeyPtr, opensslDhPtr ) < 0 )
	{
		BN_clear_free( bn_peerKeyPtr );
		throw MikeyException( "Could not create the TGK." );
	}
	return 0;

}

int KeyAgreementDH::group(){
	return groupValue;

}

int KeyAgreementDH::peerKeyLength(){
	return peerKeyLengthValue;
}

unsigned char * KeyAgreementDH::peerKey(){
	return peerKeyPtr;
}

MRef<certificate *> KeyAgreementDH::cert(){
	return CertPtr;
}

MRef<certificate_chain *> KeyAgreementDH::certificateChain(){
	return certChainPtr;
}

MRef<certificate_chain *> KeyAgreementDH::peerCertificateChain(){
	return peerCertChainPtr;
}

ca_db_item * KeyAgreementDH::peerCertificateItem(){
	return topCaDbPtr->get_next();
}
		
MRef<crl *> downloadCrl(string url){
	string uri = url.substr(7);
	string host = uri.substr(0, uri.find("/"));
	string path = uri.substr(uri.find("/"));
	string FileName = url.substr(url.rfind("/"));
	
	TCPSocket sock(host, 80);      
	sock.write("GET "+path+"\r\n\r\n");
	char buf[4096];
	string ret;
	int n;
	
	while ((n=sock.read(buf,4095))>0){
		buf[n]=0;
		ret = ret + string(buf);
	}
	 
	FILE *fp, *fp2;
	char cstring[4096];
	char *home = getenv("HOME");
	string DownloadCrl = string(home)+string("/downloadCrls")+string(FileName);
	
	MRef<crl *> CRL;
	
	if ((fp = fopen ( DownloadCrl.c_str(), "w+"))==NULL){
		      cerr << "cannot open the file" << endl;
	}
	
	strcpy(cstring, ret.c_str());
	if (fwrite (&cstring, sizeof(cstring), 1, fp)!=1)
		cerr << "file write error" << endl;
	
	fclose (fp);
	CRL = new crl( DownloadCrl.c_str() );
	return CRL;
}

void KeyAgreementDH::addPeerCertificate( MRef<certificate *> peerCertPtr ){
	//int result;
	//MRef<crl *> CRL;
	//string crlUrl, cacheUrl, crlFile;
	//char *home = getenv("HOME");
	
	
	if( this->peerCertChainPtr.isNull() ){
		this->peerCertChainPtr = new certificate_chain();
	}
	
	/*if( peerCertPtr->namesubordination() == 0 ){
		this->peerCertNamesubordination = false;
		cerr << "namesubordination check in keyagreement_dh failed" << endl;
		exit();
	}else{
		this->peerCertNamesubordination = true;
		cerr << "namesubordination check in keyagreement_dh success" << endl;
	}*/
	
	//crlUrl = peerCertPtr->getCrlDistPoint();
	//cacheUrl = crlDbPtr->checkCrlCache( crlUrl );
	//crlFile = crlUrl.substr(crlUrl.rfind("/"));
		
	/*if ( cacheUrl.empty() ){
		cerr << "New CRL, inserting it into cache." << endl;
		CRL = downloadCrl( crlUrl );
		CRL->url = crlUrl;
		CRL->file =  string(home)+string("/downloadCrls")+string(crlFile);
		updateCrlCache( CRL );
	}else{
		cerr << "The CRL is already in cache, loading it." << endl;
		CRL = new crl( cacheUrl );
		CRL->url = crlUrl;
		CRL->file =  string(home)+string("/downloadCrls")+string(crlFile);
	}
	
	if ( result = peerCertPtr->revocation( CRL ) >= 0 ){
		cerr << "The peer certificate has been revoked." << endl;
		this->peerCertRevocation = false;
		exit();
		//return;
	}*/
		
 	this->peerCertChainPtr->lock();
	this->peerCertChainPtr->add_certificate( peerCertPtr );
	this->peerCertChainPtr->unlock();
}

/*int KeyAgreementDH::peerCertChainNamesubordination(){
	MRef<certificate *> peerCert;
	
	if( !peerCertChainPtr->get_first().isNull() ){
		do{
			peerCert = peerCertChainPtr->get_next();
			if ( peerCert->namesubordination() == 1 )
				return 1;
		}while( !peerCertChainPtr->is_end() );	
	}
	return 0;
}*/

string KeyAgreementDH::peerUri(){
	string peerUri;
	peerUri = peerCertChainPtr->get_first()->get_cn();
	cerr << peerUri;
	return peerUri;
}

int KeyAgreementDH::peerCertVerification(){
	return topCaDbPtr->checkVerifiedCert( peerCertChainPtr->get_first() );
}
	
int KeyAgreementDH::peerCertChainNamesubordination(){
	if( peerCertChainPtr.isNull() )
		return 0;
	return peerCertChainPtr->namesubordination();
}

int KeyAgreementDH::peerCertChainRevocation(){
	int result;
	MRef<crl *> CRL;
	MRef<certificate *> peerCert;
	string crlUrl, cacheUrl, crlFile;
	char *home = getenv("HOME");
	
	peerCertChainPtr->init_index();
	if( !peerCertChainPtr->get_first().isNull() ){
	
		do{
			peerCert = peerCertChainPtr->get_next();
			crlUrl = peerCert->getCrlDistPoint();
			cacheUrl = crlDbPtr->checkCrlCache( crlUrl );
			crlFile = crlUrl.substr(crlUrl.rfind("/"));

			if ( cacheUrl.empty() ){
				cerr << "New CRL, inserting is into cache." << endl;
				CRL = downloadCrl( crlUrl );
				CRL->url = crlUrl;
				CRL->file =  string(home)+string("/downloadCrls")+string(crlFile);
				updateCrlCache( CRL );
			}else{
				cerr << "The CRL is already in cache, loading it." << endl;
				CRL = new crl( cacheUrl );
				CRL->url = crlUrl;
				CRL->file =  string(home)+string("/downloadCrls")+string(crlFile);
			}

			if ( result = peerCert->revocation( CRL ) >= 0 ){
				cerr << "The peer certificate has been revoked." << endl;
				return 1;
			}
		
		}while( !peerCertChainPtr->is_end() ); 
	
	}
	
	return 0;
}			

int KeyAgreementDH::controlPeerCertificate(){
	if( peerCertChainPtr.isNull() || certDbPtr.isNull() )
		return 0;
	return peerCertChainPtr->control( certDbPtr );
}

void KeyAgreementDH::savePeerCertificate(){
	MRef< certificate *> peerCert;
	string peerCertFile;
	char *home = getenv("HOME");
	string peerCommonName;
	 
	if ( this->controlPeerCertificate() == 1 ){
		//this->peerCertChainPtr->save_peerCertificate_chain();
		//if ( !this->peerCertChainPtr->get_first().isNull() ){
		//	 do{
				 peerCert = this->peerCertChainPtr->get_first();
				 peerCommonName = peerCert->get_cn();
				 peerCertFile = string(home)+string("/verifiedPeerCerts/")+string(peerCommonName)+string(".pem");
				 peerCert->file = peerCertFile;
				 this->peerCertChainPtr->save_peerCertificate( peerCert );
			 	 updatePeerCertificateCache( peerCert );	
		//	 }while( !this->peerCertChainPtr->is_end() );
		
	}
}
		
void KeyAgreementDH::updatePeerCertificateCache( MRef<certificate *> peerCert ){
	
	topCaDbPtr->lock();
	topCaDbPtr->add_certificate( peerCert );
	topCaDbPtr->unlock();

}
	
void KeyAgreementDH::updateCrlCache( MRef<crl *> CRL ){

        crlDbPtr->lock();
        crlDbPtr->add_crl( CRL );
        crlDbPtr->unlock();

}


/*int KeyAgreementDH::peeCertNamesubordination(){
	return peerCertPtr->namesubordination();
}*/

MikeyMessage * KeyAgreementDH::parseResponse( MikeyMessage * response)
{
	return response->parseResponse( this );
}

void KeyAgreementDH::setOffer( MikeyMessage * offer )
{
	offer->setOffer( this );
	return;
}

MikeyMessage * KeyAgreementDH::buildResponse( MikeyMessage * offer)
{
	return offer->buildResponse( this );
}

bool KeyAgreementDH::authenticate( MikeyMessage * msg)
{
	return msg->authenticate( this );
}
