/*
 Copyright (C) 2004-2006 the Minisip Team
 
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
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */

/* Copyright (C) 2004, 2005 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#include<config.h>

#include<libminisip/ipprovider/StunIpProvider.h>

#include<libminisip/signaling/sip/SipSoftPhoneConfiguration.h>
#include<libmnetutil/IPAddress.h>
#include<libmnetutil/UDPSocket.h>
#include<libmnetutil/NetworkFunctions.h>
#include<libmnetutil/NetworkException.h>
#include<libmstun/STUN.h>

#include<stdlib.h>

#ifdef _WIN32_WCE
#	include"../include/minisip_wce_extra_includes.h"
#endif

using namespace std;

static vector<string> getLocalIPs(){
        vector<string> ret;
        vector<string> ifaces = NetworkFunctions::getAllInterfaces();
        for (unsigned i=0; i<ifaces.size(); i++){
		string ip = NetworkFunctions::getInterfaceIPStr(ifaces[i]);
		mdbg << "Adding local ip: "<< ip <<  endl;
		ret.push_back(ip);
        }
        return ret;
}

static string findStunServer( MRef<SipSoftPhoneConfiguration *> phoneConf, uint16_t stunPort ){
	
#ifdef DEBUG_OUTPUT
        mdbg << "Try 1, autodetect"<< endl;
#endif
        if (phoneConf->findStunServerFromSipUri){
		mdbg << "Using SIP uri: "<<phoneConf->defaultIdentity->getSipUri().getString()<< endl;
                const SipUri &useruri = phoneConf->defaultIdentity->getSipUri();
		const string &uridomain = useruri.getIp();
                        mdbg << "domain=<"<<uridomain<<">"<< endl;
                        if (uridomain.length()>0){
                                uint16_t port;
                                string proxy = NetworkFunctions::getHostHandlingService("_stun._udp",uridomain, port);
                                if (proxy.length()>0){
                                        phoneConf->stunServerIpString = proxy;
                                        phoneConf->stunServerPort = (uint16_t)port;
                                        return proxy;
                                }
			}
	}

#ifdef DEBUG_OUTPUT
	mout << "Try 2, checkig if configured to use domain"<< endl;
#endif
	if (phoneConf->findStunServerFromDomain && phoneConf->stunDomain.length()>0){
		 uint16_t port;
		 string proxy = NetworkFunctions::getHostHandlingService(
				 "_stun._udp",phoneConf->stunDomain, port);
		 if (proxy.length()>0){
                         phoneConf->stunServerIpString = proxy;
                         phoneConf->stunServerPort = (uint16_t)port;
                         return proxy;
		 }
	}
#ifdef DEBUG_OUTPUT
	mout << "Try 3, checking if user defined"<< endl;
#endif
	if (phoneConf->useUserDefinedStunServer && 
			phoneConf->userDefinedStunServer.length()>0){
		uint16_t port=3478;
		string addr = phoneConf->userDefinedStunServer;
		if (addr.find(":")!=string::npos){
			mdbg << "Found port"<< endl;
			string portstr = addr.substr(addr.find(":")+1);
			addr = addr.substr(0,addr.find(":")-1);
			mdbg << "Port parsed to <"<< portstr<<">"<< endl;
			mdbg << "Addr is now <"<< addr<<">" <<endl;
			port = atoi(portstr.c_str());
		}
		phoneConf->stunServerIpString = addr;
		phoneConf->stunServerPort = port;
		return addr;
	}
	return "";
}

MRef<IpProvider *> StunIpProvider::create( MRef<SipSoftPhoneConfiguration *> phoneConf ){

        vector<string> localips = getLocalIPs();

        MRef<IPAddress *> stunIp = NULL;
        bool done=false;
                
	do{
		done=true;
		uint16_t port=0;
		string proxy = findStunServer(phoneConf, port);

		try{
			stunIp = IPAddress::create(phoneConf->stunServerIpString, false);
		}
		catch(HostNotFound & ){
			merr << "Could not find your STUN server. "
			        "STUN will be disabled." << endl;
			return NULL;
			done = false;
		}
		if( !phoneConf->useSTUN ){
			/* The user no longer wants to use STUN */
			return NULL;
		}
	} while( !done );

	uint16_t stunPort = phoneConf->stunServerPort;
	
	UDPSocket sock;
	
	uint16_t localPort = (uint16_t)sock.getPort();
	char mappedip[16];
	uint16_t mappedport;
	int32_t natType = STUN::getNatType( **stunIp, stunPort, 
			sock, localips, localPort, mappedip, mappedport );

	if( natType == STUN::STUN_ERROR ){
		merr << "An error occured while minisip tried to "
			"discover the NAT type with STUN. "
			"STUN support will be disabled." << endl;
		return NULL;
	}

	if( natType == STUN::STUNTYPE_BLOCKED ){
		merr << "minisip could not contact your STUN server. "
			"STUN support will be disabled." << endl;
		return NULL;
	}
	
	string externalIp = mappedip;
#ifdef DEBUG_OUTPUT
	mout << "NAT type is: " << STUN::typeToString( natType ) <<
                        " and the external contact IP is set to "<<
                        externalIp << endl;
#endif

	return (IpProvider*) new StunIpProvider( 
			natType, externalIp, stunIp, stunPort  );
}

StunIpProvider::StunIpProvider( uint32_t natType_, string externalIp_, MRef<IPAddress *> stunIp_, uint16_t stunPort_ ):
		stunIp(stunIp_),
		stunPort(stunPort_),
		externalIp(externalIp_),
		natType(natType_)
{
}

StunIpProvider::~StunIpProvider()
{
}

string StunIpProvider::getExternalIp(){
	return externalIp;
}

uint16_t StunIpProvider::getExternalPort( MRef<UDPSocket *> socket ){
	char mappedIPBuffer[16];
	uint16_t mappedPort;

	if( natType == (unsigned)STUN::STUNTYPE_OPEN_INTERNET ){
		/* In that case, don't bother do the STUN query */
		return (uint16_t)socket->getPort();
	}
	
	STUN::getExternalMapping( **stunIp,
                                   stunPort,
                                  **socket,
                                  mappedIPBuffer,
                                  mappedPort );

	if( string( mappedIPBuffer ) != externalIp ){
		externalIp = string( mappedIPBuffer );
	}

	return mappedPort;
}

