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
 * 	SipSoftPhoneConfiguration.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/


#ifndef SIPSOFTPHONECONFIGURATION_H
#define SIPSOFTPHONECONFIGURATION_H

#include<config.h>

#include<vector>
#include<list>
#include<libmutil/XMLParser.h>
#include<libmutil/itoa.h>
#include<libmutil/MemObject.h>
#include<libmutil/cert.h>

//#include<libmsip/SipMessageTransport.h>

#include<libmsip/SipDialogConfig.h>

#include"SipDialogSecurityConfig.h"

#include"Sip.h"
#include"../minisip/contactdb/PhoneBook.h"
//#include"../minisip/contactdb/ContactDb.h"
#include"../codecs/Codec.h"

//#include"../mediahandler/MediaHandler.h"

class IPAddress;
class Socket;
class SoundIO;
class PhoneBook;
//FIXME: XXX Deep copy needed for for example psk


/**
 * The phone configuration.
 */
class SipSoftPhoneConfiguration : public MObject{
	public:
		SipSoftPhoneConfiguration();

		virtual std::string getMemObjectType(){return "SipSoftPhoneConfig";}

		void save();
		std::string load( std::string filename );
		
		string configFileName;
		static string getDefaultConfigFilename();
		static string getDefaultConfigFileString();
		
		static string getDefaultPhoneBookFilename();
		static string getDefaultPhoneBookString();
		
		static void installConfigFile(string config, string address="", bool overwrite=false);
		
		bool checkVersion( uint32_t fileVersion, string fileVersion_str );
		
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
		list< MRef<SipIdentity*> > identities;
		
		/**
		Return the identity with getId()==id (from the identities list
		*/
		MRef<SipIdentity*> getIdentity( string id );
		
		/*
		IPAddress * pstnProxy;
		string pstnProxyString;
		string pstnSipDomain;
		int32_t pstnProxyPort;
		string pstnNumber;
		string pstnProxyUsername;	// Advanced->PSTN proxy username
		string pstnProxyPassword;  // Advanced->PSTN proxy password
		*/
		

		bool useSTUN;
		string stunServerIpString;
		uint16_t stunServerPort;

		bool findStunServerFromSipUri;
		bool findStunServerFromDomain;
		string stunDomain;
		bool useUserDefinedStunServer;
		string userDefinedStunServer;
		
		
		Socket * proxyConnection;

//		bool doRegister; 	//General->Register to proxy
//		bool doRegisterPSTN; 	//Advanced...
//		
		string soundDevice;
		string videoDevice;
		uint32_t frameWidth;
		uint32_t frameHeight;

		string manualProxy;
		bool autodetectProxy;
		bool dynamicSipPort;
		bool usePSTNProxy;
		

		bool tcp_server;

//		MRef<TimeoutProvider<string, MRef<StateMachine<SipSMCommand,string>*> > *> timeoutProvider;
		

		bool tls_server;

		list<MRef<PhoneBook *> > phonebooks;

#ifdef MINISIP_AUTOCALL
		string autoCall;
#endif

		string ringtone;
		
		list<string> audioCodecs;
		
		bool muteAllButOne;
		
		string soundIOmixerType;
		
		//P2T configurations:
		//-------------------
		
		/**
		 * the port of the Group Member List Server
		 * used for P2T Sessions.
		 */
		int32_t p2tGroupListServerPort;
};

#endif
