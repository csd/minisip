#include <config.h>

#include <libmikey/KeyAgreementPKE.h>
#include <libmikey/MikeyMessage.h>
#include <libmikey/MikeyException.h>
#include <libmcrypto/rand.h>

using namespace std;

KeyAgreementPKE::KeyAgreementPKE( MRef<CertificateChain*> cert,
				  MRef<CertificateChain*> peerCert )
		:KeyAgreementPSK(),
		 PeerCertificates(cert, peerCert){
	// TODO autodetect length from RSA size
	int envKeyLength = 112;

	//envelope key to encrypt KEMAC payload
	//byte_t envKey[ envKeyLength ];
	byte_t *envKey = new byte_t[ envKeyLength ];
	Rand::randomize( envKey, envKeyLength );
	setPSK( envKey, envKeyLength );
	
	delete []envKey;

	//verification set
	setV(1);
}

KeyAgreementPKE::KeyAgreementPKE( MRef<CertificateChain *> cert, 
				  MRef<CertificateSet *> ca_db)
		:KeyAgreementPSK(),
		 PeerCertificates(cert, ca_db){

	int envKeyLength = 112;

	//envelope key to encrypt KEMAC payload
	//byte_t envKey[ envKeyLength ];
	byte_t *envKey = new byte_t[ envKeyLength ];

	Rand::randomize( envKey, envKeyLength );
	setPSK( envKey, envKeyLength );
	delete []envKey;
}

KeyAgreementPKE::~KeyAgreementPKE(){
}

int32_t KeyAgreementPKE::type(){
	return KEY_AGREEMENT_TYPE_PK;
}

byte_t* KeyAgreementPKE::getEnvelopeKey(void){
	return getPSK();
}

int KeyAgreementPKE::getEnvelopeKeyLength(){
	return getPSKLength();
}

void KeyAgreementPKE::setEnvelopeKey( const byte_t *aEnvKey,
				      size_t aEnvKeyLength ){
	setPSK( aEnvKey, (int)aEnvKeyLength );
}
	
MikeyMessage* KeyAgreementPKE::createMessage(){
	return MikeyMessage::create( this );
}
