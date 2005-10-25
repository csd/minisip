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
*/


#include<config.h>

#include<libmsip/SipDialogConfig.h>
#include<libmsip/SipTransaction.h>
#include<libmsip/SipDialog.h>
#include<libmsip/SipMessageTransport.h>
//#include<libmutil/itoa.h>
#include<libmutil/dbg.h>
#include<libmutil/MemObject.h>
#include<libmsip/SipDialogContainer.h>
#include<libmsip/SipInvite.h>


int SipIdentity::globalIndex = 1; //give an initial value

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

void SipIdentity::setIdentityName(string n){
	identityIdentifier = n;
}

void SipIdentity::setIsRegistered( bool registerOk ) {
	if( registerOk == true && sipProxy.getRegisterExpires_int() != 0 ) {
		currentlyRegistered = true;
	} else {
		currentlyRegistered = false;
	}
}

SipCommonConfig::SipCommonConfig():
	localUdpPort(0),
	localTcpPort(0),
	localTlsPort(0),
	transport(("")),
	autoAnswer(false){

}

int32_t SipCommonConfig::getLocalSipPort(bool usesStun) {
	int32_t localSipPort;

	if(transport=="TCP")
		localSipPort = localTcpPort;
	else if(transport=="TLS")
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
	inherited->transport = transport;
}


