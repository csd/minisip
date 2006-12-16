#ifndef KEYAGREEMENTPKE_H
#define KEYAGREEMENTPKE_H

#include <libmikey/keyagreement.h>
#ifdef HAVE_OPENSSL_EVP_H
# include <openssl/evp.h>
#else
typedef struct evp_pkey_st EVP_PKEY;
#endif

/**
 * Instances of this class are used to store the necessary information about
 * the keys used in the security protocol SRTP
 * It contains the necessary methods to derive the keys used
 */
class KeyAgreementPKE : public KeyAgreement{
	public:
	
		/**
		 * Constructor with the keys needed for the encryption
		 */
	    KeyAgreementPKE(byte_t* envKey, const int envKeyLength, EVP_PKEY* pubKeyResponder);
	    
	    /**
	     * Destructor deletes some objects to prevent memory leaks
	     */
	    ~KeyAgreementPKE();
	
	    /**
	     * Generates a TGK of de given length with the random function from the
	     * OpenSSL library and stores it in this instance
	     */
	    void generateTgk(uint32_t tgkLength);
	
	    /**
	     * Generates and stores the transport encryption key of the given length.
	     * It is derived by the envelope key
	     */
	    void genTranspEncrKey(byte_t* encrKey, int encrKeyLength);
	
	    /**
	     * Generates and stores the salting key of the given length.
	     * It is also derived by the envelope key
	     */
	    void genTranspSaltKey(byte_t* saltKey, int saltKeyLength);
	
	    /**
	     * Creates and stores the authentication key to authenticate the MAC/signature
	     * of the MIKEY message.
	     */
	    void genTranspAuthKey(byte_t* authKey, int authKeyLength);
	
	    /**
	     * If the V bit is set by the initiator, the responder has to send a
	     * verification message.
	     */
	    void setV(int value){v=value;}
	    
		/**
		 * Used to test if the V bit is set.
		 */
		int getV(){return v;}
	
	    /**
	     * Returns the Public-Key of the responder
	     */
	    EVP_PKEY* getPublicKey(void);
	    
		/**
		 * Returns the envelope key
		 */
	    byte_t* getEnvelopeKey(void);
	    
		/**
		 * Returns the length of the envelope key
		 */
	    int getEnvelopeKeyLength(void);
	    
		/**
		 * Returns the timestamp on which the message was sent
		 */
	    uint64_t tSent();
	    
	    /**
	     * Sets the timestamp
	     */
	    void setTSent(uint64_t tSent);
	    
	    /**
	     * Timestamp on which the message was received
	     */
	    uint64_t t_received;
	    
	    /**
	     * Authentication key
	     */
	    byte_t* authKey;
	    
	    /**
	     * Length of the authentication key
	     */
	    unsigned int authKeyLength;
	    
	    /**
	     * MAC algorithmus (HMAC-SHA1)
	     */
		int macAlg;
	    
	private:
	
		/**
		 * Envelope key
		 */
	    byte_t* envKey;
	    
	    /**
	     * Length of the envelope key
	     */
	    int envKeyLengthValue;
	    
	    /**
	     * Public-Key of the responder
	     */
	    EVP_PKEY* pubKeyResponder;
	    
	    /**
	     * Timestamp from when the message was sent
	     */
	    uint64_t tSentValue;
	    
	    /**
	     * The V bit
	     */
	    int v;

};
#endif //KEYAGREEMENTPKE_H
