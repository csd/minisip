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


/* Name
 * 	SipDialogConfig.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/


#ifndef SIPDialogCONFIG_H
#define SIPDialogCONFIG_H

#include<libmsip/libmsip_config.h>

#include<libmutil/MemObject.h>
#include<libmutil/mtypes.h>
#include<libmutil/Mutex.h>
#include<libmsip/SipRequest.h>
#include<libmsip/SipStack.h>
#include<libmsip/SipSim.h>

#define DEFAULT_SIPPROXY_EXPIRES_VALUE_SECONDS 1000

#define KEY_MGMT_METHOD_NULL            0x00
#define KEY_MGMT_METHOD_MIKEY           0x10
#define KEY_MGMT_METHOD_MIKEY_DH        0x11
#define KEY_MGMT_METHOD_MIKEY_PSK       0x12
#define KEY_MGMT_METHOD_MIKEY_PK        0x13

#include<string>

class SipStackConfig;

class LIBMSIP_API SipProxy : public MObject{
	public:
		/**
		Initialize an empty proxy ... invalid
		*/
		SipProxy();

		/**
		Initialize a proxy with manual settings
		@param addr proxy string, it can be a name or an IP, with and without the :port
		@param port port the proxy addr is set to, it has precedence over the :port in the addr param
		*/
		SipProxy(const SipUri &addr, int port = -1);
		
		/**
		Initialize a proxy with automatic discovery of settings via DNS SRV
		@param userUri user's AOR, from where we extract the hostpart to check for SRV
		@param transport transport to check for (_sip._udp, ... ). If TCP and fails, we will retry 
		with UDP. If TLS, there is no fallback (they are all unsecured).
		*/
		SipProxy(const SipUri &userUri, std::string transport);
		
		std::string getDebugString();

		/**
		Find the proxy settings for the given uri
		@param uri user's AOR, from where we extract the hostpart to check for SRV
		@param port return parameter, where the port used by the service is obtained (the hostname is
		returned as the return param
		@param transport transport protocol to find the host:port settings for
		@return the proxy hostname (the port is returned by reference)
		*/
		static SipUri findProxy(const SipUri &uri, const std::string &transport="UDP");

		std::string sipProxyUsername;
		std::string sipProxyPassword;
		
		void setRegisterExpires( std::string _expires );
		void setRegisterExpires( int _expires );
		std::string getRegisterExpires( );
		int getRegisterExpires_int( ) {return registerExpires;}
		
		void setDefaultExpires( std::string _expires );
		void setDefaultExpires( int _expires );
		std::string getDefaultExpires( );
		int getDefaultExpires_int( ) {return defaultExpires;}

		const SipUri getUri() const{ return uri; }

		std::string getMemObjectType() const {return "SipProxy";}
		
		/**
		True to indicate that the proxy settings are to be looked up using DNS SRV
		It corresponds to <auto_detect_proxy> tag in the config file.
		True if the proxy settings, using SRV lookups from DNS
		False if the settings are specified in the config file
		 <proxy_addr>, <proxy_port>, <transport>
		
		If autodetectProxy is true, the preferred transport is checked first. 
		If that protocol is not supported, it will fall back to use UDP 
		(except for TLS, which will just not connect ... we want security).
		*/
		bool autodetectSettings;

	protected:		
		void setProxy(const SipUri &addr, int port=-1);

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

		SipUri uri;
};

class LIBMSIP_API SipIdentity : public MObject{
	public:
		SipIdentity();
		SipIdentity(const SipUri &sipuri);

		void setIdentityName(std::string n);//{identityIdentifier = n;}
		
		void setSipUri(const SipUri &addr) { sipUri = addr; }
		
		const SipUri &getSipUri() const { return sipUri; }

		/**
		@returns the sip proxy used by this identity
		*/
		MRef<SipProxy *> getSipProxy();
		
		/**
		@param proxy sip proxy to be used by this identity
		@returns true if it was ok, false otherwise
		*/
		bool setSipProxy( MRef<SipProxy *> proxy );
		
		/**
		Set the proxy to be used by this identity, given the following params
		@param autodetect use the userUri to detect the proxy name and ip (use SRV in DNS)
		@param userUri used when autodetect = true (we need the domain part to obtain the proxy name and port)
		@param transport transport to be used (if autodetect = true, it is used to fetch addequate SRV)
		@param proxyAddr used if autodetect = false ... resolve this name/ip into an ip
		@param port used if autodetect = false ... port number to use for the proxy
		@return string error message return string ... empty string if everything was ok.
		*/
		std::string setSipProxy( bool autodetect, std::string userUri, std::string transport, std::string proxyAddr, int port );
		
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
		
		std::string getDebugString();

		virtual std::string getMemObjectType() const {return "SipIdentity";}
		
		/**
		This identities index number. Useful to identify it across minisip.
		*/
		std::string getId() { 
			lock();
			std::string ret = identityIdx; 
			unlock();
			return ret;
		}
		
		std::string identityIdentifier;


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
		
		void setSim(MRef<SipSim*> s){sim=s;}

		MRef<SipSim *> getSim(){return sim;}

		std::string getPsk(){return psk;}

		void setPsk( std::string key );

		bool securityEnabled;
		int ka_type;
		bool dhEnabled;
		bool pskEnabled;
		bool checkCert;
		bool use_zrtp;
	private: 
		SipUri sipUri;

		MRef<SipSim *> sim;

		//bool use_srtp;
		//unsigned char *psk;
		//unsigned int pskLength;
		std::string psk;


		MRef<SipProxy *> sipProxy;

		/**
		We will use this index to be able to identify the identities
		*/
		static int globalIndex;
		std::string identityIdx;
		
		/**
		Indicates whether this identity is currently registered to a proxy.
		*/
		bool currentlyRegistered;

		/**
		Mutex for use in different threads
		*/
		Mutex mutex;
};

class LIBMSIP_API SipDialogConfig : public MObject{
	public:
		SipDialogConfig(MRef<SipStackConfig *> phone_config);

		virtual std::string getMemObjectType() const {return "SipDialogConfig";}
		
		MRef<SipStackConfig *> inherited;

		//Specific to calls
		std::string proxyNonce;
		std::string proxyRealm;

		uint32_t local_ssrc;

		MRef<SipRequest*> last_invite;

		MRef<SipIdentity*> sipIdentity;

		/**
		Set the identity to be used as default.
		@param identity identity to be used
		@param useSecurity 
		@param transport it is ignored ... transport is set according to the identity->sipProxy->getTransport() value
		*/
		void useIdentity( MRef<SipIdentity*> identity,
				std::string transport="UDP_X");

};

#endif
