/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/* Copyright (C) 2004, 2005 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#include<config.h>
#include"StunIpProvider.h"
#include"../gui/Gui.h"
#include"../../sip/SipSoftPhoneConfiguration.h"
#include<libmnetutil/IPAddress.h>
#include<libmnetutil/UDPSocket.h>
#include<libmnetutil/NetworkFunctions.h>
#include<libmnetutil/NetworkException.h>
#include"../../stun/STUN.h"

using namespace std;


static vector<string> getLocalIPs(){
        vector<string> ret;
        vector<string> ifaces = NetworkFunctions::getAllInterfaces();
        for (unsigned i=0; i<ifaces.size(); i++){
		string ip = NetworkFunctions::getInterfaceIPStr(ifaces[i]);
		merr << "Adding local ip: "<< ip <<  end;
		ret.push_back(ip);
        }
        return ret;
}

static string findStunServer( MRef<SipSoftPhoneConfiguration *> phoneConf, uint16_t stunPort ){
	
#ifdef DEBUG_OUTPUT
        merr << "Try 1, autodetect"<< end;
#endif
        if (phoneConf->findStunServerFromSipUri){
                merr << "Using SIP uri: "<<phoneConf->inherited.sipIdentity->getSipUri()<< end;
                string useruri = phoneConf->inherited.sipIdentity->getSipUri();
                if (useruri.find("@")!=string::npos){
                        string uridomain = useruri.substr(useruri.find("@")+1);
                        merr << "domain=<"<<uridomain<<">"<< end;
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
	}

#ifdef DEBUG_OUTPUT
	mout << "Try 2, checkig if configured to use domain"<< end;
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
	mout << "Try 3, checking if user defined"<< end;
#endif
	if (phoneConf->useUserDefinedStunServer && 
			phoneConf->userDefinedStunServer.length()>0){
		uint16_t port=3478;
		string addr = phoneConf->userDefinedStunServer;
		if (addr.find(":")!=string::npos){
			merr << "Found port"<< end;
			string portstr = addr.substr(addr.find(":")+1);
			addr = addr.substr(0,addr.find(":")-1);
			merr << "Port parsed to <"<< portstr<<">"<< end;
			merr << "Addr is now <"<< addr<<">" <<end;;
			port = atoi(portstr.c_str());
		}
		phoneConf->stunServerIpString = addr;
		phoneConf->stunServerPort = port;
		return addr;
	}
	return "";
}

MRef<StunIpProvider *> StunIpProvider::create( MRef<SipSoftPhoneConfiguration *> phoneConf, Gui * gui ){

        vector<string> localips = getLocalIPs();

        IP4Address *stunIp = NULL;
        bool done=false;
                
	do{
		done=true;
		uint16_t port=0;
		string proxy = findStunServer(phoneConf, port);

		try{
			stunIp = new IP4Address(phoneConf->stunServerIpString);
		}
		catch(HostNotFound *hnf){
			/*
			merr <<  
				"Error: could not resolve STUN ip: "
				<< hnf->errorDescription() << end;
			if( ! gui->configDialog( phoneConf ) ){
				exit( 0 );
			}
			*/
			gui->displayErrorMessage( "Could not find your STUN server. "
					   "STUN will be disabled." );
			return NULL;
			done = false;
		}
		if( !phoneConf->useSTUN ){
			/* The user no longer wants to use STUN */
			return NULL;
		}
	} while( !done );

	uint16_t stunPort = phoneConf->stunServerPort;
	
	UDPSocket sock(false);
	
	uint16_t localPort = sock.getPort();
	char mappedip[16];
	uint16_t mappedport;
	int32_t natType = STUN::getNatType( *stunIp, stunPort, 
			sock, localips, localPort, mappedip, mappedport );

	if( natType == STUN::STUN_ERROR ){
		merr << "An error occured while minisip tried to"
			"discover the NAT type with STUN."
			"STUN support will be disabled." << end;
		return NULL;
	}
	
	string externalIp = mappedip;
#ifdef DEBUG_OUTPUT
	mout << "NAT type is: " << STUN::typeToString( natType ) <<
                        " and the external contact IP is set to "<<
                        externalIp << end;
#endif

	return new StunIpProvider( natType, externalIp, stunIp, stunPort  );
}

StunIpProvider::StunIpProvider( uint32_t natType, string externalIp, IPAddress * stunIp, uint16_t stunPort ):
		stunIp(stunIp),
		stunPort(stunPort),
		externalIp(externalIp),
		natType(natType)
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
		return socket->getPort();
	}
	
	STUN::getExternalMapping( *((IP4Address*)stunIp),
                                   stunPort,
                                  **socket,
                                  mappedIPBuffer,
                                  mappedPort );

	if( string( mappedIPBuffer ) != externalIp ){
		externalIp = string( mappedIPBuffer );
	}

	return mappedPort;
}

