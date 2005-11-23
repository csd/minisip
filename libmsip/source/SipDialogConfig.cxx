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


int SipIdentity::globalIndex = 1; //give an initial value

SipProxy::SipProxy(){
	sipProxyAddressString = "";
	sipProxyIpAddr = NULL;
	sipProxyPort = 0; 
	autodetectSettings = false; //dont autodetect ... the values are invalid
	registerExpires=DEFAULT_SIPPROXY_EXPIRES_VALUE_SECONDS;
	defaultExpires=DEFAULT_SIPPROXY_EXPIRES_VALUE_SECONDS;
}

SipProxy::SipProxy(std::string addr, int port){
	autodetectSettings = false;
	try {
		registerExpires=DEFAULT_SIPPROXY_EXPIRES_VALUE_SECONDS;
		defaultExpires=DEFAULT_SIPPROXY_EXPIRES_VALUE_SECONDS;
		setProxy( addr, port );
	} catch (NetworkException & exc ) {
		#ifdef DEBUG_OUTPUT
		cerr << "SipProxy(str, int) throwing ... " << endl;
		#endif
		throw HostNotFound( "[SipProxy " + addr + "]" );
	}
}

SipProxy::SipProxy(std::string userUri, string transportParam) { //note: this->transport is an class member, do not rename transportParam
	string addr;
	uint16_t port;
	autodetectSettings = true;
	
	registerExpires=DEFAULT_SIPPROXY_EXPIRES_VALUE_SECONDS;
	defaultExpires=DEFAULT_SIPPROXY_EXPIRES_VALUE_SECONDS;
	
	if( getTransport() == "" ) {
		setTransport( transportParam );
	}
	
	try {
		addr = SipProxy::findProxy( userUri, port, transportParam );
	}catch( NetworkException & exc ) {
		if( transportParam == "TCP" ) { 
			//if tcp doesn't work, try find UDP
			addr = "unknown";
		}
	}
	
	//if tcp failed, retry with udp ...
	if( addr == "unknown" && transportParam == "TCP" ) {
		try {
			cerr << "Autodetect Sip proxy for [" << userUri << "] for transport TCP failed. Retrying with transport UDP." << endl;
			transportParam = "UDP";
			addr = SipProxy::findProxy( userUri, port, transportParam );
		}catch( NetworkException & exc2 ) {
				addr = "unknown";
		}
	}	
	
	if( addr == "unknown" ) {
		#ifdef DEBUG_OUTPUT
		cerr << "SipProxy(str, str) throwing (1) ... " << endl;
		#endif
		throw HostNotFound( "[SipProxy for <" + userUri + ">]" );
	}
	
// 	addr = SipProxy::findProxy( userUri, (uint16_t)port, transportParam );
	
	try {
		setProxy( addr, port );
		setTransport( transportParam );
	} catch (NetworkException & exc ) {
		#ifdef DEBUG_OUTPUT
		cerr << "SipProxy(str, str) throwing (2)... " << endl;
		#endif
		throw HostNotFound( "[SipProxy <" + addr + "]" );
	}
}

//addr could be "IP:port" ... but the port param passed to the function has precedence ...
void SipProxy::setProxy(std::string addr, int port){
	massert(addr.find("@")==std::string::npos);
	if( port > 65535 || port < 0 ) port = -1; //check the port
	
	#ifdef DEBUG_OUTPUT
	cerr << "SipProxy:setProxy(str) : addr = " << addr << endl;
	#endif
	if (addr.find(":")!=std::string::npos){
		if( port == -1 ) {
			std::string portstr = addr.substr(addr.find(":")+1);
			portstr = portstr.substr( 0, portstr.find(":") ); //make sure only one port is there ...
			mdbg<< "parsed proxy port to <"<< portstr<<">"<<end;
			sipProxyPort = atoi(portstr.c_str());
		} else {
			sipProxyPort = port;
		}
		sipProxyAddressString =  addr.substr(0,addr.find(":"));
		
	}else{
		sipProxyAddressString = addr;
		if( port == -1 ) {
			#ifdef DEBUG_OUTPUT
			cerr << "SipProxy: invalid port ... setting 5060" << endl;
			#endif
			sipProxyPort = 5060;
		} else {
			sipProxyPort = port;
		}
	}
	
	sipProxyIpAddr = new IP4Address(sipProxyAddressString);
}

std::string SipProxy::getDebugString(){
	return "proxyString="+sipProxyAddressString
		+"; proxyIp="+ ((sipProxyIpAddr==NULL)?"NULL":sipProxyIpAddr->getString())
		+"; port="+itoa(sipProxyPort)
		+"; transport="+getTransport()
		+"; autodetect="+ (autodetectSettings?"yes":"no")
		+"; user="+sipProxyUsername
		+"; password="+sipProxyPassword
		+"; expires="+itoa(defaultExpires);
}

std::string SipProxy::findProxy(std::string uri, uint16_t &port, string transport){

	if (uri.find("@")==std::string::npos){                 
		return "unknown";
	}
	std::string domain = uri.substr(uri.find("@"));
	domain=domain.substr(1);
	
	//check if we find a colon ... and ignore that part
	if (uri.find(":")!=std::string::npos){
		uri = uri.substr( 0, uri.find(":") );
		#ifdef DEBUG_OUTPUT
		cerr << "SipProxy::findProxy : sanitizing malformed proxy uri ..." << endl;
		#endif
	}
	
	//Do a SRV lookup according to the transport ...
	string srv;
	if( transport == "TLS" || transport == "tls") { srv = "_sips._tcp"; }
	else if( transport == "TCP" || transport == "tls") { srv = "_sip._tcp"; }
	else { //if( trans == "UDP" || trans == "udp") { 
		srv = "_sip._udp"; 
	}

	std::string proxy = NetworkFunctions::getHostHandlingService(srv, domain, port);
	if (proxy.length()<=0){
		return "unknown";
	}
	return proxy;
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
	securitySupport=false;
	identityIdx = itoa( globalIndex );
	globalIndex ++;
#ifdef DEBUG_OUTPUT	
	cerr << "SipIdentity::SipIdentity : cretated identity id=" << identityIdx << endl;
#endif
	setIsRegistered (false);
}
SipIdentity::SipIdentity(string addr) : securitySupport(false),registerToProxy(false){
	setSipUri(addr);
	securitySupport = false;
	identityIdx = itoa( globalIndex );
	globalIndex ++;
#ifdef DEBUG_OUTPUT	
	cerr << "SipIdentity::SipIdentity(str) : cretated identity id=" << identityIdx << endl;
#endif	
	setIsRegistered (false);
}

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
	
	if( !autodetect ) {
		try {
			this->sipProxy = new SipProxy(proxyAddr, proxyPort);
			#ifdef DEBUG_OUTPUT
			cerr << "SipIdentity::setProxy: manual sipproxy success ... " << endl;
			#endif
			proxySetSuccess = true;
		} catch ( NetworkException & exc ){
			#ifdef DEBUG_OUTPUT
			cerr << "SipIdentity::setProxy: manual settings for SIP proxy are wrong ... trying autodetect" << endl;
			cerr << "SipIdentity::setProxy: error = " << exc.errorDescription() << endl;
			#endif
		}
	}
	
	if( !proxySetSuccess || autodetect) {
		try{
			this->sipProxy = new SipProxy( userUri, transport );
			//the transport may have fallen back to UDP ...
			transport = getSipProxy()->getTransport(); 
			proxyAddr = getSipProxy()->sipProxyAddressString;
			proxyPort = getSipProxy()->sipProxyPort;
			proxySetSuccess = true;
		} catch( NetworkException & exc ){
			#ifdef DEBUG_OUTPUT
			cerr << "SipIdentity::setProxy: autodetect failed to fetch proxy settings ..." << endl;
			cerr << "SipIdentity::setProxy: error = " << exc.errorDescription() << endl;
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
	
	MRef<SipProxy *> prox = getSipProxy();
	if( ! prox ) {	
		#ifdef DEBUG_OUTPUT
		cerr << "SipIdentity::setProxy: creating fake proxy ..." << endl;
		#endif
		//if creation of proxy failed ... make a fake one, to save some of the data
		setSipProxy( new SipProxy() );
		prox = getSipProxy();
		prox->autodetectSettings = autodetect;
		prox->sipProxyAddressString = proxyAddr;
	}
	prox->sipProxyPort = proxyPort;
	prox->setTransport( transport );
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
	string ret = "identity="+identityIdx+"; username="+
		sipUsername+ "; domain="+sipDomain + 
		" proxy=["+ getSipProxy()->getDebugString()+ 
		"]; isRegistered="+itoa(currentlyRegistered);
	unlock();
	return ret;
}

SipCommonConfig::SipCommonConfig():
	localUdpPort(0),
	localTcpPort(0),
	localTlsPort(0),
	autoAnswer(false){

}

string SipCommonConfig::getTransport() {
	string ret = sipIdentity->getSipProxy()->getTransport();
	if( ret == "" ) {
		#ifdef DEBUG_OUTPUT
		cerr << "SipCommonConfig::getTransport(): empty proxy transport ... returning default UDP" << endl;
		#endif
		ret = "UDP";
	}
	return ret;
}

int32_t SipCommonConfig::getLocalSipPort(bool usesStun) {
	int32_t localSipPort;
	string transport = getTransport();
	
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


SipDialogConfig::SipDialogConfig(MRef<SipCommonConfig *> commonconf) : proxyConnection(NULL) {
	inherited = new SipCommonConfig; /// We want do do a "deep copy" here. This is so that
					 /// we have a local copy that we can modify and that 
					 /// no one else modifies.
	**inherited = **commonconf;


	last_invite=NULL;


	local_ssrc = rand();
}

void SipDialogConfig::useIdentity(
			MRef<SipIdentity*> identity,
			bool useSecurity,
			string transport)
{
	inherited->sipIdentity = identity;
// 	inherited->transport = transport;
// 	inherited->transport = inherited->sipIdentity->sipProxy.getTransport();
}


