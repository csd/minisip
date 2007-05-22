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

#include<libminisip/ipprovider/SimpleIpProvider.h>

#include<libminisip/signaling/sip/SipSoftPhoneConfiguration.h>
#include<libmnetutil/NetworkFunctions.h>
#include<libmnetutil/UDPSocket.h>

#ifdef _WIN32_WCE
#	include"../include/minisip_wce_extra_includes.h"
#endif

using namespace std;

SimpleIpProvider::SimpleIpProvider( MRef<SipSoftPhoneConfiguration *> config ){
	unsigned i; //index
	vector<string> ifaces = NetworkFunctions::getAllInterfaces();
	
	localIp = config->sipStackConfig->localIpString;
	#ifdef DEBUG_OUTPUT
	cerr << "SimpleIPProvider: localIp = " << localIp << endl;
	#endif
	
	if (localIp.length()>0){
		bool ok=false;
		for ( i=0; i<ifaces.size(); i++ ){
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
	
	bool ipFound = false;
		
	//if a preferred network interface is specified in the config file ... 
	if( config->networkInterfaceName != "") {
		for (i=0; i<ifaces.size(); i++){
			if ( config->networkInterfaceName == ifaces[i] ) {
				localIp = NetworkFunctions::getInterfaceIPStr(ifaces[i]);
				ipFound=true;
				break;
			}
		}


	#ifdef DEBUG_OUTPUT
		cerr << "SimpleIPProvider: preferred network interface = " << config->networkInterfaceName  << endl;
		if( ipFound ) cerr << "SimpleIPProvider: preferred interface found" << endl;
		else cerr << "SimpleIPProvider: preferred interface NOT found" << endl;
		if ( ipFound && localIp=="" ){
			cerr << "SimpleIPProvider: WARNING: prefered interface has no IP address configured"<<endl;
		}
	#endif
		//If the preferred interface is without IP, continue searching...
		if (localIp=="")
			ipFound=false;
	} 

	//if ip is not found (either not specified or the adapter is not good ... 
	//use one which we consider apropriate	
	if( ! ipFound ) {
		//print message telling the user about defining a preferred interface
		cout <<    "========================================================================" << endl
			<< "|No network interface defined as preferred in the configuration, or" << endl
			<< "|the one specified could not be found." << endl
			<< "|Minisip will try to find an appropriate one." << endl
			<< "|Minisip highly recommends you to add a preferred one. To do so, choose" << endl
			<< "|    from the list below and edit the configuration file, section <network_interface>" << endl
			<< "|    or use the GUI configuration;" << endl;
			for( i=0; i<ifaces.size(); i++ ){
				string ip = NetworkFunctions::getInterfaceIPStr(ifaces[i]);
		   		cout << "|       Network Interface: name = " << ifaces[i] << "; IP=" << ip << endl;
			}	
		cout <<    "========================================================================" << endl;
		for ( i=0; i<ifaces.size(); i++ ){
			string ip = NetworkFunctions::getInterfaceIPStr(ifaces[i]);
			#ifdef DEBUG_OUTPUT
			//cout << "SimpleIPProvider: interface = " << ifaces[i] << "; IP=" << ip << endl;
			#endif
			if (ip.length()>0){
				if (ifaces[i]==string("lo")){ //this interface only exhists in linux ...
					if (localIp.length()<=0)
						localIp = ip;
				}else{
					string ipstr = ip;
					
					//only update the local ip if it is the first interface with a private
					//ip different from localhost or a public ip
					if ( isInPrivateIpRange( ipstr )){
						if (localIp.length()<=0 || 
								localIp == "127.0.0.1" || //this is the lo interface
								localIp.substr(0,2)=="0."  //0.0.0.0 is used by windows ...
								)
							localIp = ipstr;
					}else{ 
						//use first public ip we find ... overwritting the private one
						if( localIp.length() <= 0 || 
								localIp=="127.0.0.1" ||
								localIp.substr(0,2)=="0." ||
								isInPrivateIpRange( localIp) )
							localIp = ipstr;
					}
				}
			}
		}
	}
	cout << "Minisip is using IP =  " << localIp << endl;
}

bool SimpleIpProvider::isInPrivateIpRange( string ipstr ) {
	//check the easy ones first ... 10.x.x.x, 127.x.x.x,
	//192.168.x.x and 0.x.x.x
	if (ipstr.substr(0,3)=="10." 
			|| ipstr.substr(0,4)=="127."
			|| ipstr.substr(0,7)=="192.168" 
			|| ipstr.substr(0,2)=="0.") {	//Found local interfaces in Windows XP used to communicate only
							//internally with a web camera that started with "0."
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
	return (uint16_t)sock->getPort();
}
