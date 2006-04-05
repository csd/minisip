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

/* Copyright (C) 2004 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

/* Name
 * 	SipSoftPhoneConfiguration.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#ifndef SIPSOFTPHONECONFIGURATION_H
#define SIPSOFTPHONECONFIGURATION_H

#include<libminisip/libminisip_config.h>

#include<vector>
#include<list>

#include<libmutil/XMLParser.h>
#include<libmutil/itoa.h>
#include<libmutil/MemObject.h>

#include<libmsip/SipDialogConfig.h>

#include<libminisip/sip/SipDialogSecurityConfig.h>
#include<libminisip/sip/Sip.h>
#include<libminisip/contactdb/PhoneBook.h>
#include<libminisip/configbackend/ConfBackend.h>

class IPAddress;
class Socket;
class SoundIO;
class PhoneBook;
//FIXME: XXX Deep copy needed for for example psk


/**
 * The phone configuration.
 */
class LIBMINISIP_API SipSoftPhoneConfiguration : public MObject{
	public:
		SipSoftPhoneConfiguration();
		virtual ~SipSoftPhoneConfiguration();

		virtual std::string getMemObjectType(){return "SipSoftPhoneConfig";}

		void save();
		std::string load( MRef<ConfBackend *> be );
		
		static std::string getDefaultPhoneBookFilename();
		
		static void installConfigFile(std::string config, std::string address="", bool overwrite=false);
		
		bool checkVersion( uint32_t fileVersion );


		/**
		 * Saves the default options to the ConfBackend.
		 * @param backend a reference to the ConfBackend object
		 * to use for saving the default values
		 */
		void saveDefault( MRef<ConfBackend *> backend );
		
		MRef<SipCommonConfig *> inherited;	//inherited.sipIdentity is the default sip identity.
		
		SipDialogSecurityConfig securityConfig;

		
		//Configuration only the phone has and not every call
		MRef<Sip *> sip;
	
//		SipIdentity pstnIdentity;
		MRef<SipIdentity *> pstnIdentity;
//		MRef<SipIdentity *> defaultIdentity;

		/**
		List of identities we have extracted from the config file.
		The default identity is in SipSoftPhoneConfiguration::inherited.sipIdentity
		*/
		std::list< MRef<SipIdentity*> > identities;
		
		/**
		Return the identity with getId()==id (from the identities list
		*/
		MRef<SipIdentity*> getIdentity( std::string id );

		/**
		 * Returns the identity when getUri() == uri
		 */
		MRef<SipIdentity *> getIdentity( SipUri &uri );
		
		/*
		IPAddress * pstnProxy;
		std::string pstnProxyString;
		std::string pstnSipDomain;
		int32_t pstnProxyPort;
		std::string pstnNumber;
		std::string pstnProxyUsername;	// Advanced->PSTN proxy username
		std::string pstnProxyPassword;  // Advanced->PSTN proxy password
		*/
		

		bool useSTUN;
		std::string stunServerIpString;
		uint16_t stunServerPort;

		bool findStunServerFromSipUri;
		bool findStunServerFromDomain;
		std::string stunDomain;
		bool useUserDefinedStunServer;
		std::string userDefinedStunServer;
		
		Socket * proxyConnection;

//		bool doRegister; 	//General->Register to proxy
//		bool doRegisterPSTN; 	//Advanced...
//		
		std::string soundDevice;
		std::string videoDevice;
		uint32_t frameWidth;
		uint32_t frameHeight;

// 		bool autodetectProxy; //it is in SipProxy::
		
		bool usePSTNProxy;
// 		std::string manualProxy; //was used in qt interface ... change the qt to be like gtk
// 		bool dynamicSipPort; //was used in qt interface ... change the qt to be like gtk

		bool tcp_server;

//		MRef<TimeoutProvider<std::string, MRef<StateMachine<SipSMCommand,std::string>*> > *> timeoutProvider;

		bool tls_server;

		std::list<MRef<PhoneBook *> > phonebooks;

#ifdef MINISIP_AUTOCALL
		std::string autoCall;
#endif

		std::string ringtone;
		
		std::list<std::string> audioCodecs;
		
		//not used anymore ... it was used in mediahandler ... 
// 		bool muteAllButOne;
		
		std::string soundIOmixerType;
		
		/**
		Start up commands, specified in the config file.
		Example:
		<startup_cmd>
			<command>
				call
			</command>
			<params>
				user@domain
			</params>
		</startup_cmd>
		<startup_cmd>
			<command>
				im
			</command>
			<params>
				user@domain this is the message to be sent
			</params>
		</startup_cmd>
		*/
		std::list<std::string> startupActions;
		
		//P2T configurations:
		//-------------------
		
		/**
		 * the port of the Group Member List Server
		 * used for P2T Sessions.
		 */
		int32_t p2tGroupListServerPort;

		/**
		In the <network_interface> part of the config file, we can specify
		the name of the adapter we want to use. We use the name, as the IP 
		address may vary (DHCP, etc).
		In linux: lo, eth0, eth1, ... 
		In Windows XP: <1234ASD1-234F-988A-9102BDE1>
		If empty, there is no preferred network interface, thus 
			list all available, tell the user and try to choose
			a correct one.
		*/
		std::string networkInterfaceName;

	private:
		MRef<ConfBackend *> backend;
};

#endif
