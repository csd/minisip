#ifndef _SIPSIM_H
#define _SIPSIM_H

#include<libmsip/libmsip_config.h>
#include<libmutil/mtypes.h>
#include<libmutil/MemObject.h>
#include<libmcrypto/cert.h>

/**
 * Subscriber identity module used to authenticate the user.
 * This can be used to sign MIKEY requests, use in TLS/SSL
 * handshake.
 *
 * Implementations of SipSim can be a interface to a 
 * SmartCard or do it in software.
 */
class LIBMSIP_API SipSim : public MObject{
	public:
		static const int SHA1_RSAPKCS1;

		//string getSipUri();

		/**
		 * Returns the certificate for the subscriber.
		 */
		//virtual MRef<certificate*> getCertificate() = 0;

		virtual MRef<certificate_chain *> getCertificateChain() = 0;
		virtual MRef<ca_db *> getCAs() = 0;

		/**
		 * Puts len bytes from data into SHA1 and 
		 * signs the resulting hash.
		 *
		 * @return Length of generated signature in bytes.
		 */
		virtual void sign(byte_t *data, uint32_t len, byte_t *out_sign, int method=SHA1_RSAPKCS1 )= 0;
};


#endif
