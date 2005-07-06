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

#ifdef _MSC_VER
#ifdef LIBMINISIP_EXPORTS
#define LIBMINISIP_API __declspec(dllexport)
#else
#define LIBMINISIP_API __declspec(dllimport)
#endif
#else
#define LIBMINISIP_API
#endif


#include<vector>
#include<list>
#include<libminisip/Codec.h>
#include<libmutil/XMLParser.h>
#include<libmutil/itoa.h>
#include<libmutil/MemObject.h>
#include<libmutil/cert.h>

#include<libmsip/SipDialogConfig.h>
#include<libminisip/SipDialogSecurityConfig.h>

#include<libminisip/Sip.h>
#include<libminisip/PhoneBook.h>
#include<libminisip/ContactDb.h>
#include<libminisip/MediaHandler.h>

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

		virtual std::string getMemObjectType(){return "SipSoftPhoneConfig";}

		MRef<SipCommonConfig *> inherited;	//inherited.sipIdentity is the default sip identity.
		
		SipDialogSecurityConfig securityConfig;

		
		//Configuration only the phone has and not every call
		MRef<Sip*> sip;
	
		MRef<SipIdentity *> pstnIdentity;

		list< MRef<SipIdentity*> > identities;
		
		bool useSTUN;
		string stunServerIpString;
		uint16_t stunServerPort;

		bool findStunServerFromSipUri;
		bool findStunServerFromDomain;
		string stunDomain;
		bool useUserDefinedStunServer;
		string userDefinedStunServer;
		
		
		Socket * proxyConnection;

		string soundDevice;
		string videoDevice;
		uint32_t frameWidth;
		uint32_t frameHeight;

		string manualProxy;
		bool autodetectProxy;
		bool dynamicSipPort;
		bool usePSTNProxy;
		
		string configFileName;

		bool tcp_server;
		bool tls_server;

		list<MRef<PhoneBook *> > phonebooks;

#ifdef MINISIP_AUTOCALL
		string autoCall;
#endif

		string ringtone;

		void save();
		std::string load( std::string filename );
		
		//P2T configurations:
		//-------------------
		
		/**
		 * the port of the Group Member List Server
		 * used for P2T Sessions.
		 */
		int32_t p2tGroupListServerPort;
		
		list<string> audioCodecs;
};

#endif
