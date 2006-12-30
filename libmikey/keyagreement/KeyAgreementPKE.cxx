#include <config.h>

#include <libmikey/KeyAgreementPKE.h>
#include <libmikey/MikeyMessage.h>
#include <libmikey/MikeyException.h>
#include <libmcrypto/rand.h>


KeyAgreementPKE::KeyAgreementPKE( MRef<certificate*> pubKeyResponderT, int envKeyLength )
		:KeyAgreementPSK(){
										
	typeValue = KEY_AGREEMENT_TYPE_PK;
	
	//envelope key to encrypt KEMAC payload
	byte_t envKey[ envKeyLength ];
	Rand::randomize( envKey, envKeyLength );
	setPSK( envKey, envKeyLength );

	//public key to encrypt PKE payload
	pubKeyResponder = pubKeyResponderT;
	
	//verification set
	setV(1);
}

KeyAgreementPKE::~KeyAgreementPKE(){
}

MRef<certificate*> KeyAgreementPKE::getPublicKey(void){
	return pubKeyResponder;
}

byte_t* KeyAgreementPKE::getEnvelopeKey(void){
	return getPSK();
}

int KeyAgreementPKE::getEnvelopeKeyLength(){
	return getPSKLength();
}

void KeyAgreementPKE::setEnvelopeKey( const byte_t *aEnvKey,
				      size_t aEnvKeyLength ){
	setPSK( aEnvKey, aEnvKeyLength );
}
	

MikeyMessage* KeyAgreementPKE::createMessage(){
	return MikeyMessage::create( this );
}
