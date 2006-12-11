
#include<config.h>
#include<libmsip/SipSimSoft.h>

using namespace std;

SipSimSoft::SipSimSoft(MRef<certificate_chain*> chain, MRef<ca_db*> cas){
	certChain = chain;
	ca_set = cas;
}

/*
SipSimSoft::SipSimSoft(string certfilepath, string pkeyfilepath){

}
*/

MRef<certificate_chain*> SipSimSoft::getCertificateChain(){

	return certChain;
}

MRef<ca_db*> SipSimSoft::getCAs(){
	return ca_set;
}


void SipSimSoft::sign(byte_t *data, uint32_t len, byte_t *out_sign, int method){
	cerr << "EEEE: ERROR: unimplemented method: SipSimSoft::sign(...)"<<endl;
}

