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


/* Name
 * 	SipDialogConfig.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/


#ifndef SIPDialogCONFIG_H
#define SIPDialogCONFIG_H

#ifdef _MSC_VER
#ifdef LIBMSIP_EXPORTS
#define LIBMSIP_API __declspec(dllexport)
#else
#define LIBMSIP_API __declspec(dllimport)
#endif
#else
#define LIBMSIP_API
#endif

#include<vector>
#include<libmsip/SipInvite.h>
#include<libmutil/TimeoutProvider.h>
#include<libmutil/StateMachine.h>
#include<libmutil/MemObject.h>
#include<libmutil/XMLParser.h>
#include<libmutil/cert.h>
#include<libmutil/itoa.h>
//#include<libmsip/SipMessageTransport.h>
#include<libmsip/SipSMCommand.h>
#include<libmnetutil/IP4Address.h>
#include<libmnetutil/NetworkFunctions.h>

#include<libmutil/dbg.h>

#ifdef _MSC_VER

#else
#include<stdint.h>
#endif

#define KEY_MGMT_METHOD_NULL            0x00
#define KEY_MGMT_METHOD_MIKEY           0x10
#define KEY_MGMT_METHOD_MIKEY_DH        0x11
#define KEY_MGMT_METHOD_MIKEY_PSK       0x12
#define KEY_MGMT_METHOD_MIKEY_PK        0x13

#define DEFAULT_SIPPROXY_EXPIRES_VALUE_SECONDS 1000

#include<string>

using namespace std;


class LIBMSIP_API SipProxy{
	public:
		SipProxy(){
			sipProxyIpAddr = NULL;
			sipProxyPort = 0; 
			registerExpires=DEFAULT_SIPPROXY_EXPIRES_VALUE_SECONDS;
			defaultExpires=DEFAULT_SIPPROXY_EXPIRES_VALUE_SECONDS;
		}

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
			registerExpires=DEFAULT_SIPPROXY_EXPIRES_VALUE_SECONDS;
			defaultExpires=DEFAULT_SIPPROXY_EXPIRES_VALUE_SECONDS;
		}
		
		SipProxy(string addr, int port):sipProxyPort(port),sipProxyAddressString(addr){
			sipProxyIpAddr = new IP4Address(addr);
			registerExpires=DEFAULT_SIPPROXY_EXPIRES_VALUE_SECONDS;
			defaultExpires=DEFAULT_SIPPROXY_EXPIRES_VALUE_SECONDS;
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
				+"; password="+sipProxyPassword
				+"; expires="+itoa(registerExpires);
		}

		static string findProxy(string uri, uint16_t &port){

			if (uri.find("@")==string::npos){                 
				return "unknown";
			}
			string domain = uri.substr(uri.find("@"));
			domain=domain.substr(1);

			string proxy = NetworkFunctions::getHostHandlingService("_sip._udp",domain, port);
			if (proxy.length()<=0){
				return "unknown";
			}
			return proxy;
		}

		
		int sipProxyPort;
		string sipProxyAddressString;
		IPAddress * sipProxyIpAddr;
		string sipProxyUsername;
		string sipProxyPassword;
		
		void setRegisterExpires( string _expires );
		void setRegisterExpires( int _expires );
		string getRegisterExpires( );
		int getRegisterExpires_int( ) {return registerExpires;}
		
		void setDefaultExpires( string _expires );
		void setDefaultExpires( int _expires );
		string getDefaultExpires( );
		int getDefaultExpires_int( ) {return defaultExpires;}
		
	private:
		/**
		Default expires value. 
		This is the one read from the config file ... do not change once set
		*/
		int defaultExpires; //in seconds
		
		/**
		 Value of the expires tag to be added for the contact in this proxy.
		 Use setRegisterExpires to set it to zero (de-register).
		 Note that this value may change during time ... basically, it changes
		 to zero if we are unregistered. 
		 The register_expires value read from the config file is stored in 
			defaultExpies member.
		 */
		int registerExpires; //in seconds
		
};

class LIBMSIP_API SipIdentity : public MObject{
	public:
		SipIdentity();
		SipIdentity(string sipuri);

		void setIdentityName(string n);//{identityIdentifier = n;}
		
		void setSipUri(string addr);
		
		string getSipUri(){
			string ret;
			lock();
			ret = sipUsername + "@" + sipDomain;
			unlock();
			return ret;
		}

		void setDoRegister(bool f){
			lock();
			registerToProxy=f;
			unlock();
		}

		bool getDoRegister(){
			lock();
			bool ret = registerToProxy;
			unlock();
			return ret;
		}

		void lock(){mutex.lock();};
		void unlock(){mutex.unlock();};
		
		string getDebugString(){
			lock();
			string ret = "identity="+identityIdx+"; username="+
				sipUsername+ "; domain="+sipDomain + 
				" proxy=["+sipProxy.getDebugString()+
				"]; isRegistered="+itoa(currentlyRegistered);
			unlock();
			return ret;
		}

		virtual std::string getMemObjectType(){return "SipIdentity";}
		
		/**
		This identities index number. Useful to identify it across minisip.
		*/
		string getId() { 
			lock();
			string ret = identityIdx; 
			unlock();
			return ret;
		}
		
		string sipUsername;
		string sipDomain;       //SipAddress is <sipUsername>@<sipDomain>

		SipProxy sipProxy;

		string identityIdentifier;

		bool securitySupport;

		/**
		Indicates whether this identity requires to be registered to a proxy.
		*/
		bool registerToProxy;
		
		/**
		It sets the correct value for this::isRegistered.
		Param registerOk indicates whether a register_ok command has been received.
		If false, there was some error, thus we are not registered.
		If true, we registered ok ... but if proxy::expires = 0, then we are 
		unregistering (isRegistered = false, then).
		*/
		void setIsRegistered( bool registerOk );
		
		/**
		True if this identity is currently registered, false otherwise.
		*/
		bool isRegistered() {
			lock();
			bool ret = currentlyRegistered;
			unlock();
			return ret;}
		
	private: 
		/**
		We will use this index to be able to identify the identities
		*/
		static int globalIndex;
		string identityIdx;
		
		/**
		Indicates whether this identity is currently registered to a proxy.
		*/
		bool currentlyRegistered;

		/**
		Mutex for use in different threads
		*/
		Mutex mutex;
};


class LIBMSIP_API SipCommonConfig : public MObject{
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
		int32_t localTlsPort;
		
		string transport;
		
		/**
			Return the port in use, depending on the transport.
			Parameter: usesStun (default false), found in 
					SipSoftPhoneConfiguration::useSTUN
		*/
		int32_t getLocalSipPort(bool usesStun=false);
		
		MRef<SipIdentity*> sipIdentity;
	
//		MRef<SipMessageTransport*> sipTransport;	// ? General-> Network Interface ?
		
		bool autoAnswer;

		void save( XMLFileParser * parser );
		void load( XMLFileParser * parser );

};


class LIBMSIP_API SipDialogConfig : public MObject{
	public:
		SipDialogConfig(MRef<SipCommonConfig *> phone_config);

		virtual std::string getMemObjectType(){return "SipDialogConfig";}
		
		MRef<SipCommonConfig *> inherited;

		Socket *proxyConnection; //TODO: verify that this is working ok - it has been moved here from SipSoftPhoneConfiguration

		//Specific to calls
		string proxyNonce;
		string proxyRealm;

		uint32_t local_ssrc;

		MRef<SipInvite*> last_invite;

		void useIdentity( MRef<SipIdentity*> identity,
				bool useSecurity,
				string transport="UDP");

};

#endif
