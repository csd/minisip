#ifndef _SIPSIMSOFT_H
#define _SIPSIMSOFT_H

#include<libmsip/SipSim.h>

/**
 *
 * "Soft" refers to that crypto-algorithms are
 * implemented in software (using OpenSSL or 
 * GnuTLS), but could also be read as "soft" security
 * compared to SmartCard hardware security devices.
 *
 */
class LIBMSIP_API SipSimSoft : public SipSim{
	public:
		SipSimSoft(MRef<certificate_chain*> chain, MRef<ca_db*> cas);
//		SipSimSoft(string certfilepath, string pkeyfilepath);

		virtual MRef<certificate_chain*> getCertificateChain();

		virtual MRef<ca_db*> getCAs();

		virtual void sign(byte_t *data, uint32_t len, byte_t *out_sign, int method=SHA1_RSAPKCS1 );

	private:
		MRef<certificate_chain *> certChain;
		MRef<ca_db *> ca_set;

};


#endif
