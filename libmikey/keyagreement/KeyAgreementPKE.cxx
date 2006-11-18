#include <config.h>

#include <libmikey/KeyAgreementPKE.h>
#include <libmcrypto/rand.h>


KeyAgreementPKE::KeyAgreementPKE(byte_t* envKeyT, int envKeyLength, EVP_PKEY* pubKeyResponderT)
																:KeyAgreement(), tSentValue(0){
										
	typeValue = KEY_AGREEMENT_TYPE_PK;
	
	//envelope key to encrypt KEMAC payload
	envKeyLengthValue = envKeyLength;
	envKey = new unsigned char[envKeyLengthValue];
	memcpy(envKey, envKeyT, envKeyLengthValue);
	
	//public key to encrypt PKE payload
	pubKeyResponder = pubKeyResponderT;
	
	//verification set
	v = 1;
}

KeyAgreementPKE::~KeyAgreementPKE(){
	if(envKey)
		delete [] envKey;
}

void KeyAgreementPKE::generateTgk(uint32_t tgkLength){
	typeValue = KEY_AGREEMENT_TYPE_PK;
	this->tgkLengthValue = tgkLength;
	if( tgkPtr ){
		delete [] tgkPtr;
	}
	
	tgkPtr = new unsigned char[tgkLength];
	Rand::randomize(tgkPtr, tgkLength);
}

void KeyAgreementPKE::genTranspEncrKey(byte_t* encrKey, int encrKeyLength){
	keyDeriv(0xFF, csbIdValue, envKey, envKeyLengthValue, 
			encrKey, encrKeyLength, KEY_DERIV_TRANS_ENCR);
}

void KeyAgreementPKE::genTranspSaltKey(byte_t* encrKey, int encrKeyLength){
	keyDeriv(0xFF, csbIdValue, envKey, envKeyLengthValue, 
			encrKey, encrKeyLength, KEY_DERIV_TRANS_SALT);
}

void KeyAgreementPKE::genTranspAuthKey(byte_t* encrKey, int encrKeyLength){
	keyDeriv(0xFF, csbIdValue, envKey, envKeyLengthValue, 
			encrKey, encrKeyLength, KEY_DERIV_TRANS_AUTH);
}

void KeyAgreementPKE::setTSent(uint64_t tSent){
	tSentValue = tSent;
}

uint64_t KeyAgreementPKE::tSent(){
	return tSentValue;
}

EVP_PKEY* KeyAgreementPKE::getPublicKey(void){
	return pubKeyResponder;
}

byte_t* KeyAgreementPKE::getEnvelopeKey(void){
	return envKey;
}

int KeyAgreementPKE::getEnvelopeKeyLength(){
	return envKeyLengthValue;	
}
