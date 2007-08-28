/*
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */

/* Copyright (C) 2004, 2005, 2006
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
 *          Mikael Magnusson <mikma@users.sourceforge.net>
*/

#include<config.h>
#include"SimpleIp6Provider.h"
#include<libminisip/signaling/sip/SipSoftPhoneConfiguration.h>
#include<libmnetutil/NetworkFunctions.h>
#include<libmnetutil/UDPSocket.h>
#include<algorithm>

using namespace std;

SimpleIp6Provider::SimpleIp6Provider( MRef<SipSoftPhoneConfiguration *> config ){
	vector<MRef<NetworkInterface*> > ifaces =
		NetworkFunctions::getInterfaces();
	bool use_ipv6 = true;
	Scope curScope = LINK_LOCAL;
	
	localIp = config->sipStackConfig->localIpString;

	mdbg << "SimpleIP6Provider: localIp = " << localIp << endl;

	
	if (localIp.length()>0){
		bool ok=false;
		for (unsigned i=0; i<ifaces.size(); i++){
			MRef<NetworkInterface *> iface = ifaces[i];
			vector<string> addrs = iface->getIPStrings( true );
			vector<string>::iterator iter;

			mdbg << "Simple6IP: checking interface = " << iface->getName() << endl;

			iter = find( addrs.begin(), addrs.end(), localIp );
			if ( iter != addrs.end() ){
				ok=true;
			}
		}
		if (!ok){
			merr << "Error: The IP address specified in the"
				"configuration file ("<<localIp<<
				") is not configured on any local interface."<< endl;
			localIp = "";
                }
		else return;
	}
	
	for (unsigned i=0; i<ifaces.size(); i++){
		void *ptr = &ifaces[i];

		mdbg << "SimpleIP6Provider: checking ptr = " << ptr << endl;

		MRef<NetworkInterface *> iface = ifaces[i];

		if( iface ){
			mdbg << "SimpleIP6Provider: checking interface = " << *iface << endl;

			vector<string> addrs = iface->getIPStrings( use_ipv6 );
			vector<string>::iterator iter;

			mdbg << "SimpleIP6Provider: checking interface = " << iface->getName() << endl;

			for( iter = addrs.begin(); iter != addrs.end(); iter++ ){
				string ipstr = *iter;
				
				if ( ipstr  == string("::1") ){
					if ( localIp.length() <= 0 )
						localIp = ipstr;
					continue;
				}

				Scope scope = ipScope( ipstr );

				mdbg << "SimpleIP6Provider: checking interface = " << ifaces[i] << " with IP=" << ipstr << " scope=" << scope << endl;
				//only update the local ip i	f it is the first interface with a private
				//ip different from localhost or a publi	c ip

				if( scope > curScope ){
					localIp = ipstr;
					curScope = scope;
					
				}
				else if( localIp.length() <= 0 ){
					localIp = ipstr;
					curScope = scope;
				}
			}
		}
	}
	mdbg << "SimpleIP6Provider: using localIP =  " << localIp << endl;
}

SimpleIp6Provider::Scope SimpleIp6Provider::ipScope( string ipstr ) {
	unsigned int prefix = strtol(ipstr.substr(0, 4).c_str(), NULL, 16);

	// Global IPv6 address:  |001|TLA|NLA|SLA|Interface

	// fec0::/10
	if( (prefix & 0xffc0) == 0xfec0 ){
		return SITE_LOCAL;
	}
	// fe80::/10
	else if( (prefix & 0xffc0) == 0xfe80 ){
		return LINK_LOCAL;
	}
	// fc00::/7
	else if( (prefix & 0xfe00) == 0xfc00 ){
		return UNIQUE_LOCAL;
	}
	// 2000::/3
	else if( (prefix & 0xe000) == 0x2000 ){
		return GLOBAL;
	}

	return INVALID;
}

string SimpleIp6Provider::getExternalIp(){
	return localIp;
}

string SimpleIp6Provider::getLocalIp(){
	return localIp;
}

uint16_t SimpleIp6Provider::getExternalPort( MRef<UDPSocket *> sock ){
	return sock->getPort();
}
