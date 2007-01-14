
#include <config.h>
#include <libmcrypto/SipSimSoft.h>
#include <libmcrypto/rand.h>

using namespace std;

SipSimSoft::SipSimSoft(MRef<certificate_chain*> chain, MRef<ca_db*> cas){
	certChain = chain;
	ca_set = cas;
}

/*
SipSimSoft::SipSimSoft(string certfilepath, string pkeyfilepath){

}
*/

bool SipSimSoft::getSignature(unsigned char * data,
		int dataLength,
		unsigned char * signaturePtr,
		int & signatureLength,
		bool doHash,
		int hash_alg)
{
	MRef<certificate*> myCert = certChain->get_first();
	assert(doHash /*we don't support not hashing in SipSimSoft yet...*/);
	myCert->sign_data(data, dataLength, signaturePtr, &signatureLength);
	return true;
}

bool SipSimSoft::getRandomValue(unsigned char * randomPtr, unsigned long randomLength)
{
	Rand::randomize(randomPtr, randomLength);
}

