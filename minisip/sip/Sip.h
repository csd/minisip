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


/*

 +-Sip------------------------------------------+
 |                                              |  invite(string)
 | +-SipDialogContainer---+                     |<----------------------------------
 | |                      | addCall(SipCall)    |
 | |                      |<----------------    | handleCommand(SipSMCommand)
 | |                      |                     |<----------------------------------
 | |                      | handleCommand(CS)   |
 | |                      |-------------------->| sipcb_handleCommand(CommandString)
 | |                      |                     |---------------------------------->
 | |                      | sipcb_handleCommand |
 | |                      |-------------------->|
 | |                      |                     |
 | |                      | enqueuePacket()     |
 | |                      |<----------------    |
 | |                      |                     |
 | |                      | enqueueCommand()    |
 | |                      |<----------------    |
 | |                      |                     |
 | | [call_list]          | handleSipMessage()  |
 | | [defaultHandler]     |<---+                |
 | +----------------------+    |                |
 |                             |                |
 | [SipMessageTransport]-------+                |
 | [SipSoftPhoneConfiguration]                  |
 |                                              |
 |                                              |
 +----------------------------------------------+

*/

#ifndef MINISIP_SIP_H
#define MINISIP_SIP_H

#include<config.h>

#include<libmutil/minilist.h>
#include<libmsip/SipCallback.h>
#include<libmsip/SipDialogContainer.h>
#include"../sdp/SdpPacket.h"

#ifdef IPSEC_SUPPORT
#include<../ipsec/MsipIpsecAPI.h>
#endif

#include<libmsip/SipDialog.h>
#include<libmsip/SipStack.h>


class SipSoftPhoneConfiguration;
class LogEntry;
class MediaHandler;


using namespace std;

class Sip: public MObject, public Runnable{

	public:
		Sip(MRef<SipSoftPhoneConfiguration*> phoneconfig,
				MRef<MediaHandler*> mediaHandler,
				string localIpString,
				string externalContactIP,
				int32_t localUdpPort=5060,
				int32_t localTcpPort=5060,
				int32_t externalContactUdpPort=5060,
				string defaultTransportProtocol="UDP"
#ifndef NO_SECURITY
				,int32_t localTlsPort=5061,
				MRef<certificate_chain *> cert=NULL,    //The certificate chain is used by TLS
				//TODO: TLS should use the whole chain instead of only the f$
				MRef<ca_db *> cert_db = NULL
#endif
		    );

		virtual std::string getMemObjectType(){return "Sip";}
		
		MRef<SipSoftPhoneConfiguration*> getPhoneConfig();
		
                virtual void run();

		//void registerMediaStream(MRef<SdpPacket*> sdppack);

		string invite(string &user);
		string confjoin(string &user, string list[10], int num);
		string confconnect(string &user);
		MRef<SipStack*>	getSipStack(){return sipstack;}
//		MRef<SipDialogContainer*> getDialogContainer();//{return dialogContainer;}

		void setMediaHandler( MRef<MediaHandler *> mediaHandler );

//		bool handleCommand(const SipSMCommand &command);
		
//		void setCallback(SipCallback *callback);
//
//		SipCallback *getCallback();
                
	private:
		MRef<SipStack *> sipstack;
		MRef<SipSoftPhoneConfiguration*> phoneconfig;
		MRef<MediaHandler *> mediaHandler;
		
//		SipCallback *callback;

//		MRef<SipDialogContainer*> dialogContainer;
                
};


#endif
