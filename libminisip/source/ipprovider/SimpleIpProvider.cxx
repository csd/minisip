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


/* Copyright (C) 2004, 2005
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#include<libminisip/SimpleIpProvider.h>

#include<config.h>
#include<libminisip/SipSoftPhoneConfiguration.h>
#include<libmnetutil/NetworkFunctions.h>
#include<libmnetutil/UDPSocket.h>

using namespace std;



SimpleIpProvider::SimpleIpProvider( MRef<SipSoftPhoneConfiguration *> config ){
        
	vector<string> ifaces = NetworkFunctions::getAllInterfaces();

	localIp = config->inherited->localIpString;
	
        if (localIp.length()>0){
                bool ok=false;
                for (unsigned i=0; i<ifaces.size(); i++){
                        if (localIp==NetworkFunctions::getInterfaceIPStr(ifaces[i]))
                                ok=true;
                }
                if (!ok){
                        merr << "Error: The IP address specified in the"
				"configuration file ("<<localIp<<
                                ") is not configured on any local interface."<< end;
			localIp = "";
                }
		else return;
	}
	
	for (unsigned i=0; i<ifaces.size(); i++){
		string ip = NetworkFunctions::getInterfaceIPStr(ifaces[i]);
		if (ip.length()>0){
			if (ifaces[i]==string("lo")){
				if (localIp.length()<=0)
					localIp = NetworkFunctions::getInterfaceIPStr(ifaces[i]);
			}else{
				string ipstr = NetworkFunctions::getInterfaceIPStr(ifaces[i]);
				if (ipstr.substr(0,3)=="10." || ipstr.substr(0,7)=="192.168"){
					if (localIp.length()<=0 || localIp=="127.0.0.1")
						localIp = ipstr;
				}else{
					localIp = ipstr;
				}
			}
		}
	}
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
