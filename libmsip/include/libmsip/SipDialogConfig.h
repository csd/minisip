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

/* Copyright (C) 2004 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

/* Name
 * 	SipDialogConfig.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/


#ifndef SIPDialogCONFIG_H
#define SIPDialogCONFIG_H

#include<vector>
#include<libmsip/SipInvite.h>
#include<libmutil/TimeoutProvider.h>
#include<libmutil/StateMachine.h>
#include<libmutil/MemObject.h>
#include<libmutil/XMLParser.h>
#include<libmutil/cert.h>
#include<libmutil/itoa.h>
#include<libmsip/SipMessageTransport.h>
#include<libmsip/SipSMCommand.h>
#include<libmnetutil/IP4Address.h>
#include<libmnetutil/NetworkFunctions.h>

#ifdef _MSC_VER

#else
#include<stdint.h>
#endif

#define KEY_MGMT_METHOD_NULL            0x00
#define KEY_MGMT_METHOD_MIKEY           0x10
#define KEY_MGMT_METHOD_MIKEY_DH        0x11
#define KEY_MGMT_METHOD_MIKEY_PSK       0x12
#define KEY_MGMT_METHOD_MIKEY_PK        0x13


class SipProxy{
	public:
		SipProxy(){sipProxyIpAddr = NULL;sipProxyPort = 0;}

		SipProxy(string addr){
			assert(addr.find("@")==string::npos);
			if (addr.find(":")!=string::npos){
				string portstr = addr.substr(addr.find(":")+1);
				mdbg<< "parsed proxy port to <"<< portstr<<">"<<end;
				sipProxyPort = atoi(portstr.c_str());
				sipProxyAddressString =  addr.substr(0,addr.find(":"));
				
			}else{
				sipProxyAddressString = addr;
			}

			sipProxyIpAddr = new IP4Address(addr);
		}
		
		SipProxy(string addr, int port):sipProxyPort(port),sipProxyAddressString(addr){
			sipProxyIpAddr = new IP4Address(addr);
		}

		void setProxy(string proxy, int port){
			if (port != -1)
				sipProxyPort = port;
			sipProxyAddressString= proxy;
			sipProxyIpAddr = new IP4Address(proxy);
		}

		string getDebugString(){
			return "proxyString="+sipProxyAddressString
				+"; proxyIp="+ ((sipProxyIpAddr==NULL)?"NULL":sipProxyIpAddr->getString())
				+"; port="+itoa(sipProxyPort)
				+"; user="+sipProxyUsername
				+"; password="+sipProxyPassword;
				
		}

		static string findProxy(string uri, uint16_t &port){

			if (uri.find("@")==string::npos){                 
				return "unknown";
			}
			string domain = uri.substr(uri.find("@"));
			domain=domain.substr(1);

			string proxy = NetworkFunctions::getHostHandlingService("_sip._udp",domain, port);
			if (proxy.length()<=0){
				return "unknown"; //              merr << "Error: Can not find proxy (from configuration file or auto detect using users sip address)"<< end;
			}
			return proxy;
		}

                int sipProxyPort;
                string sipProxyAddressString;
		IPAddress * sipProxyIpAddr;
		
                string sipProxyUsername;
                string sipProxyPassword;

};

class SipIdentity : public MObject{
        public:
		SipIdentity(){/*sipProxyPort=0; sipProxyIpAddr=NULL;*/ registerToProxy=false; securitySupport=false;}
                SipIdentity(string sipuri);

		void setIdentityName(string n){identityIdentifier = n;}
		
		void setSipUri(string uri);
		
		string getSipUri(){return sipUsername +"@" + sipDomain;}

		void setDoRegister(bool f){registerToProxy=f;}
		bool getDoRegister(){return registerToProxy;}
		
		string getDebugString(){
			return "username="+sipUsername+ "; domain="+sipDomain + " proxy=["+sipProxy.getDebugString()+"]";
		}

		virtual std::string getMemObjectType(){return "SipIdentity";}
		
		
                string sipUsername;
                string sipDomain;       //SipAddress is <sipUsername>@<sipDomain>

		SipProxy sipProxy;

		string identityIdentifier;

		bool securitySupport;

		bool registerToProxy;
};


class SipCommonConfig : public MObject{
	public:
		SipCommonConfig();

		virtual std::string getMemObjectType(){return "SipCommonConfig";}
		
		//shared with Dialog config
//		string userUri; 	//General->Users SIP address
		string localIpString; //GEneral->Network Interface
		string externalContactIP;
                int32_t externalContactUdpPort;
                
		int32_t localUdpPort;	// Advanced -> ...Sip port...
		int32_t localTcpPort;
#ifndef NO_SECURITY
		int32_t localTlsPort;
#endif

		
		MRef<SipIdentity*> sipIdentity;
		
/*		
		int32_t proxyPort;
		string proxyUsername;	//General->Users proxy username
		string proxyPassword;	//General->Users proxy password
		string sipDomain;
*/
		
		string transport;
	
		MRef<SipMessageTransport*> sipTransport;	// ? General-> Network Interface ?
		
//		TimeoutProvider<string, MRef<StateMachine<SipSMCommand,string>*> > *timeoutProvider;
//
		bool autoAnswer;

		void save( XMLFileParser * parser );
		void load( XMLFileParser * parser );

};


class SipDialogConfig : public MObject{
	public:
		//SipDialogConfig(MRef<SipCommonConfig*> phone_config);
		SipDialogConfig(SipCommonConfig &phone_config);

		virtual std::string getMemObjectType(){return "SipDialogConfig";}
		
		SipCommonConfig inherited;

		Socket *proxyConnection; //TODO: verify that this is working ok - it has been moved here from SipSoftPhoneConfiguration

		//Specific to calls
		string proxyNonce;
		string proxyRealm;

/////		int32_t seqNo;
		
//		string callId;

/////		string tag_local;
/////		string tag_foreign;
		
/////		string uri_foreign;

		uint32_t local_ssrc;

		bool local_called;
		
		MRef<SipInvite*> last_invite;

		void useIdentity( MRef<SipIdentity*> identity,
				bool useSecurity,
				string transport="UDP");


};

#endif
