#ifndef KEYAGREEMENTPKE_H
#define KEYAGREEMENTPKE_H

#include <libmikey/keyagreement.h>
#include <libmikey/keyagreement_psk.h>
#include <libmcrypto/cert.h>

/**
 * Instances of this class are used to store the necessary information about
 * the keys used in the security protocol SRTP
 * It contains the necessary methods to derive the keys used
 */
class KeyAgreementPKE : public KeyAgreementPSK{
	public:
	
		/**
		 * Constructor with the keys needed for the encryption
		 */
		KeyAgreementPKE( MRef<certificate*> pubKeyResponder,
				 int envKeyLength = 192);
	    
	    /**
	     * Destructor deletes some objects to prevent memory leaks
	     */
	    ~KeyAgreementPKE();
	
		int32_t type();

	    /**
	     * Returns the Public-Key of the responder
	     */
	    MRef<certificate*> getPublicKey(void);
	    
		/**
		 * Returns the envelope key
		 */
	    byte_t* getEnvelopeKey(void);
	    
		/**
		 * Returns the length of the envelope key
		 */
	    int getEnvelopeKeyLength(void);
	    
		/**
		 * Set the envelope key
		 */
		void setEnvelopeKey( const byte_t *aEnvKey, size_t aEnvKeyLength );

		MikeyMessage* createMessage();

	private:
	    
	    /**
	     * Public-Key of the responder
	     */
		MRef<certificate*> pubKeyResponder;
};
#endif //KEYAGREEMENTPKE_H
