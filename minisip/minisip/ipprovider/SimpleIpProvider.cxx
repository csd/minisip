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
#include"SimpleIpProvider.h"
#include"../../sip/SipSoftPhoneConfiguration.h"
#include<libmnetutil/NetworkFunctions.h>
#include<libmnetutil/UDPSocket.h>

using namespace std;



SimpleIpProvider::SimpleIpProvider( MRef<SipSoftPhoneConfiguration *> config ){
        
	vector<string> ifaces = NetworkFunctions::getAllInterfaces();
	
	localIp = config->inherited->localIpString;
	#ifdef DEBUG_OUTPUT
	cerr << "SimpleIPProvider: localIp = " << localIp << endl;
	#endif
	
	if (localIp.length()>0){
		bool ok=false;
		for (unsigned i=0; i<ifaces.size(); i++){
// 			cerr << "SimpleIP: checking interface = " << ifaces[i] << endl;
			if (localIp==NetworkFunctions::getInterfaceIPStr(ifaces[i]))
				ok=true;
		}
		if (!ok){
			cerr << "Error: The IP address specified in the"
				"configuration file ("<<localIp<<
				") is not configured on any local interface."<< endl;
			localIp = "";
                }
		else return;
	}
	
	for (unsigned i=0; i<ifaces.size(); i++){
		string ip = NetworkFunctions::getInterfaceIPStr(ifaces[i]);
		#ifdef DEBUG_OUTPUT
		cerr << "SimpleIPProvider: checking interface = " << ifaces[i] << " with IP=" << ip << endl;
		#endif
		if (ip.length()>0){
			if (ifaces[i]==string("lo")){
				if (localIp.length()<=0)
					localIp = NetworkFunctions::getInterfaceIPStr(ifaces[i]);
			}else{
				string ipstr = NetworkFunctions::getInterfaceIPStr(ifaces[i]);
				//only update the local ip if it is the first interface with a private
				//ip different from localhost or a public ip
				if ( isInPrivateIpRange( ipstr )){
					if (localIp.length()<=0 || localIp=="127.0.0.1")
						localIp = ipstr;
				}else{ 
					//use first public ip we find ... 
					if( localIp.length() <= 0 ) 
						localIp = ipstr;
				}
			}
		}
	}
	#ifdef DEBUG_OUTPUT
	cerr << "SimpleIPProvider: using localIP =  " << localIp << endl;
	#endif
}

bool SimpleIpProvider::isInPrivateIpRange( string ipstr ) {
	//check the easy ones first ... 10.x.x.x and 192.168.x.x
	if (ipstr.substr(0,3)=="10." || ipstr.substr(0,7)=="192.168" ) {
		return true;
	}
	//this range goes from 172.16.x.x to 172.31.x.x
	if( ipstr.substr(0,4)=="172." ) {
		if( ipstr[6] == '.' ) {
			if( ipstr[4] == '1' ) {
				if( ipstr[5] == '6' || ipstr[5] == '7' || ipstr[5] == '8' || ipstr[5] == '9' ) { 
					return true;
				} 
			}
			else if( ipstr[4] == '2' ) { return true; }
			else if( ipstr[4] == '3' && ipstr[5] == '1' ) { return true;}
		}
	}
	//finally, check for automatic ip private addresses (used by mocosoft)
	if( ipstr.substr(0,7)=="169.254" ) {
		return true;
	}
	
	return false;
}

string SimpleIpProvider::getExternalIp(){
	return localIp;
}

string SimpleIpProvider::getLocalIp(){
	return localIp;
}

uint16_t SimpleIpProvider::getExternalPort( MRef<UDPSocket *> sock ){
	return sock->getPort();
}
