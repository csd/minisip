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
 *	    Cesc Santasusana < cesc Dot santa at@ gmail dOT com>
*/


#include<config.h>

#include<libmsip/SipDialogConfig.h>
#include<libmnetutil/IP4Address.h>
#include<libmnetutil/NetworkException.h>
#include<libmnetutil/NetworkFunctions.h>
#include<libmutil/massert.h>

using namespace std;

int SipIdentity::globalIndex = 1; //give an initial value

SipProxy::SipProxy(): uri(){
	autodetectSettings = false; //dont autodetect ... the values are invalid
	registerExpires=DEFAULT_SIPPROXY_EXPIRES_VALUE_SECONDS;
	defaultExpires=DEFAULT_SIPPROXY_EXPIRES_VALUE_SECONDS;
}

SipProxy::SipProxy(const SipUri &addr, int port){
	autodetectSettings = false;
	try {
		registerExpires=DEFAULT_SIPPROXY_EXPIRES_VALUE_SECONDS;
		defaultExpires=DEFAULT_SIPPROXY_EXPIRES_VALUE_SECONDS;
		setProxy( addr, port );
	} catch (NetworkException & ) {
		#ifdef DEBUG_OUTPUT
		cerr << "SipProxy(str, int) throwing ... " << endl;
		#endif
		throw HostNotFound( "[SipProxy " + addr.getString() + "]" );
	}
}

SipProxy::SipProxy(const SipUri &userUri, string transportParam) {
	SipUri addr;
	bool unknown = true;
	autodetectSettings = true;
	
	registerExpires=DEFAULT_SIPPROXY_EXPIRES_VALUE_SECONDS;
	defaultExpires=DEFAULT_SIPPROXY_EXPIRES_VALUE_SECONDS;

	addr = userUri;
	if( transportParam != "" )
		addr.setTransport( transportParam );

	setProxy( addr );
}

//addr could be "IP:port" ... but the port param passed to the function has precedence ...
void SipProxy::setProxy(const SipUri &addr, int port){
	massert(addr.getUserName()=="");
	if( port > 65535 || port < 0 ) port = -1; //check the port
	
	#ifdef DEBUG_OUTPUT
	cerr << "SipProxy:setProxy(str) : addr = " << addr << endl;
	#endif

	uri = addr;

	if( port != -1 ){
		uri.setPort(port);
	}

	// Lose router
	uri.setParameter( "lr", "true" );
}

std::string SipProxy::getDebugString(){
	return "uri="+uri.getString()
		+"; autodetect="+ (autodetectSettings?"yes":"no")
		+"; user="+sipProxyUsername
		+"; password="+sipProxyPassword
		+"; expires="+itoa(defaultExpires);
}

int SipProxy::getRegisterExpires_int( ) {
	return registerExpires;
}

int SipProxy::getDefaultExpires_int( ) {
	return defaultExpires;
}

void SipProxy::setRegisterExpires( string _expires ) {
	int r;
	r = atoi( _expires.c_str() );
	setRegisterExpires( r );
}

void SipProxy::setRegisterExpires( int _expires ) {
	if( _expires >= 0 && _expires < 100000 ) //sanity check ...
		registerExpires = _expires;
	else registerExpires = DEFAULT_SIPPROXY_EXPIRES_VALUE_SECONDS;
}

string SipProxy::getRegisterExpires( ) {
	return itoa(registerExpires); 
}

void SipProxy::setDefaultExpires( string _expires ) {
	int r;
	r = atoi( _expires.c_str() );
	setDefaultExpires( r );
}
void SipProxy::setDefaultExpires( int _expires ) {
	if( _expires >= 0 && _expires < 100000 ) //sanity check ...
		defaultExpires = _expires;
	else defaultExpires = DEFAULT_SIPPROXY_EXPIRES_VALUE_SECONDS;
}
string SipProxy::getDefaultExpires( ) {
	return itoa(defaultExpires); 
}


SipIdentity::SipIdentity(){
	/*sipProxyPort=0; sipProxyIpAddr=NULL;*/ 
	registerToProxy=false; 
	securityEnabled=false;
	ka_type=0;
	//use_srtp=false;
	use_zrtp=false;
	pskEnabled=false;
	dhEnabled=false;
	checkCert=false;

	identityIdx = itoa( globalIndex );
	globalIndex ++;
#ifdef DEBUG_OUTPUT	
	cerr << "SipIdentity::SipIdentity : created identity id=" << identityIdx << endl;
#endif
	setIsRegistered (false);
}

SipIdentity::SipIdentity(const SipUri &addr) : sipUri(addr),securityEnabled(false),registerToProxy(false){
	securityEnabled = false;
	ka_type=0;
	//use_srtp=false;
	use_zrtp=false;
	pskEnabled=false;
	dhEnabled=false;
	checkCert=false;

	identityIdx = itoa( globalIndex );
	globalIndex ++;
#ifdef DEBUG_OUTPUT	
	cerr << "SipIdentity::SipIdentity(str) : created identity id=" << identityIdx << endl;
#endif	
	setIsRegistered (false);
}

void SipIdentity::setDoRegister(bool f){
	lock();
	registerToProxy=f;
	unlock();
}

bool SipIdentity::getDoRegister(){
	lock();
	bool ret = registerToProxy;
	unlock();
	return ret;
}

void SipIdentity::lock(){
	mutex.lock();
}

void SipIdentity::unlock(){
	mutex.unlock();
}

std::string SipIdentity::getId() {
	lock();
	std::string ret = identityIdx;
	unlock();
	return ret;
}

bool SipIdentity::isRegistered(){
	lock();
	bool ret = currentlyRegistered;
	unlock();
	return ret;
}



#if 0
void SipIdentity::setSipUri(string addr){
	if (addr.substr(0,4)=="sip:")
		addr = addr.substr(4);
	if (addr.find("@")==string::npos){
			#ifdef DEBUG_OUTPUT	
			cerr << "WARNING: Incomplete sip address: "<< addr<<endl;
			#endif
		sipUsername = addr.substr(0, addr.find("@"));
		sipDomain = "";
	} else {
		sipUsername = addr.substr(0, addr.find("@"));
		sipDomain = addr.substr(addr.find("@")+1);
	}

#ifdef DEBUG_OUTPUT	
	cerr << "SipIdentity::setSipUri: sipUsername=<"<< sipUsername << "> sipDomain=<" << sipDomain << ">"<< endl;
#endif
}

string SipIdentity::getSipUri() {
	string ret;
	lock();
	if( sipUsername != "" && sipDomain !="" ) {
		ret = sipUsername + "@" + sipDomain;
	} else {
		//one of the two is empty, so do not add the @ ... 
		ret = sipUsername + sipDomain;
	}
	unlock();
	return ret;
}
#endif

MRef<SipProxy *> SipIdentity::getSipProxy() {
	return sipProxy;
}

bool SipIdentity::setSipProxy( MRef<SipProxy *> proxy ){
	sipProxy = proxy;
	return true;
}

string SipIdentity::setSipProxy( bool autodetect, string userUri, string transport, string proxyAddr, int proxyPort ) {
	bool proxySetSuccess = false;
	string ret = "";
	
	#ifdef DEBUG_OUTPUT
	if( autodetect ) cerr << "SipIdentity::setSipProxy: autodetect is true";
	else 		cerr << "SipIdentity::setSipProxy: autodetect is false";
	cerr << "; userUri=" << userUri << "; transport = "<< transport << "; proxyAddr=" << proxyAddr << "; proxyPort=" << proxyPort << endl;
	#endif	
	
	setSipProxy( NULL );
	
	if( autodetect ) {
		try{
			this->sipProxy = new SipProxy( userUri, transport );
			proxySetSuccess = true;
		} catch( NetworkException & exc ){
			#ifdef DEBUG_OUTPUT
			cerr << "SipIdentity::setProxy: autodetect failed to fetch proxy settings ..." << endl;
			cerr << "SipIdentity::setProxy: error = " << exc.what() << endl;
			#endif
			ret +="The SIP proxy for the account ["
				+ this->identityIdentifier + "] could no be resolved.";
			this->setDoRegister( false );
		}
	} else { 
		#ifdef DEBUG_OUTPUT
		cerr << "SipIdentity::setProxy: else ..." << endl;
		#endif
	}
	
	if( !proxySetSuccess ) {
		try {
			SipUri proxyUri( proxyAddr );
			proxyUri.setPort( proxyPort );
			proxyUri.setTransport( transport );

			this->sipProxy = new SipProxy( proxyUri );
			#ifdef DEBUG_OUTPUT
			if( autodetect )
				cerr << "SipIdentity::setProxy: creating fake proxy ..." << endl;
			else
				cerr << "SipIdentity::setProxy: manual sipproxy success ... " << endl;
			#endif
			proxySetSuccess = true;
		} catch ( NetworkException & exc ){
			#ifdef DEBUG_OUTPUT
			cerr << "SipIdentity::setProxy: manual settings for SIP proxy are wrong ... trying autodetect" << endl;
			cerr << "SipIdentity::setProxy: error = " << exc.what() << endl;
			#endif
		}
	}
	
	MRef<SipProxy *> prox = getSipProxy();
	prox->autodetectSettings = autodetect;
	return ret;
}

void SipIdentity::setIdentityName(string n){
	identityIdentifier = n;
}

void SipIdentity::setIsRegistered( bool registerOk ) {
	if( registerOk == true && getSipProxy()->getRegisterExpires_int() != 0 ) {
		currentlyRegistered = true;
	} else {
		currentlyRegistered = false;
	}
}

string SipIdentity::getDebugString(){
	lock();
	string ret = "identity="+identityIdx+
			"; uri="+sipUri.getString()+ 
			" proxy=["+(getSipProxy()?getSipProxy()->getDebugString():"")+
			"]; isRegistered="+itoa(currentlyRegistered);
	unlock();
	return ret;
}

void SipIdentity::setPsk( string key ){
	psk=key;
}

SipStackConfig::SipStackConfig():
	localUdpPort(0),
	localTcpPort(0),
	localTlsPort(0),
	autoAnswer(false),
	use100Rel(false){

}

int32_t SipStackConfig::getLocalSipPort(bool usesStun, const string &transport ) {
	int32_t localSipPort;
	
	if(transport=="TCP" || transport=="tcp")
		localSipPort = localTcpPort;
	else if(transport=="TLS" || transport=="tls")
		localSipPort = localTlsPort;
	else{ /* UDP, may use STUN */
		if( usesStun ){
			localSipPort = externalContactUdpPort;
		} else {
			localSipPort = localUdpPort;
		}
	}
	return localSipPort;
}


SipDialogConfig::SipDialogConfig(MRef<SipStackConfig *> commonconf) {
	inherited = new SipStackConfig; /// We want do do a "deep copy" here. This is so that
					 /// we have a local copy that we can modify and that 
					 /// no one else modifies.
	**inherited = **commonconf;


	last_invite=NULL;


	local_ssrc = rand();
}

void SipDialogConfig::useIdentity(
			MRef<SipIdentity*> id,
			string transport)
{
	this->sipIdentity=id;
//	inherited->sipIdentity = identity;
// 	inherited->transport = transport;
// 	inherited->transport = inherited->sipIdentity->sipProxy.getTransport();
}


SipUri SipDialogConfig::getContactUri(bool useStun) const{
	SipUri contactUri;
	const SipUri &fromUri = sipIdentity->getSipUri();
	string transport;

	if( sipIdentity->getSipProxy() )
		transport = sipIdentity->getSipProxy()->getUri().getTransport();
	else
		transport = fromUri.getTransport();

	contactUri.setParams( fromUri.getUserName(),
			      inherited->externalContactIP,
			      "",
			      inherited->getLocalSipPort(useStun, transport));
	if( transport != "" )
		contactUri.setTransport( transport );

	return contactUri;
}
