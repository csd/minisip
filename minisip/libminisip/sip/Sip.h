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
#include<libmsip/SipDialogContainer.h>
#include"../sdp/SdpPacket.h"

#ifdef IPSEC_SUPPORT
#include<../ipsec/MsipIpsecAPI.h>
#endif

#include<libmsip/SipDialog.h>
#include<libmsip/SipStack.h>

#include "../conf/ConfMember.h"

#include"../minisip/LogEntry.h"

#include"../mediahandler/MediaHandler.h"

class SipSoftPhoneConfiguration;
class MediaHandler;


using namespace std;

class Sip: public Runnable, public CommandReceiver{

	public:
		Sip(MRef<SipSoftPhoneConfiguration*> phoneconfig,
				MRef<MediaHandler*> mediaHandler,
				string localIpString,
				string externalContactIP,
				int32_t localUdpPort=5060,
				int32_t localTcpPort=5060,
				int32_t externalContactUdpPort=5060,
				string defaultTransportProtocol="UDP"
				,int32_t localTlsPort=5061,
				MRef<certificate_chain *> cert=NULL,    //The certificate chain is used by TLS
				//TODO: TLS should use the whole chain instead of only the f$
				MRef<ca_db *> cert_db = NULL
		    );

		virtual ~Sip();

		virtual std::string getMemObjectType(){return "Sip";}
		
		MRef<SipSoftPhoneConfiguration*> getPhoneConfig();
		
                
		/**
		Starts this Sip object as a Thread.
		You can still run the Sip object in non-thread by directly calling
		the run() function
		*/
		bool start();
		
		/**
		It sends the sip_stack_shutdown command, to stop the sip stack in a 
		controlled way.
		While you wait, you can use the join function.
		*/
		void stop();
		
		/**
		Join the sip thread (if this was started with start(), that is, as a 
		Thread. It will block and not return until the run function returns.
		*/
		void join();
		
		/**
		Main method. It can be run either directly with sip->run() or 
		to be run in a Thread (use sip->start())
		*/
		virtual void run();

		string confjoin(string &user, minilist<ConfMember> *list, string confId);
		string confconnect(string &user, string confId);
		MRef<SipStack*> getSipStack(){return sipstack;}

		void setMediaHandler( MRef<MediaHandler *> mediaHandler );

		void handleCommand(string subsystem, const CommandString &cmd);

		CommandString handleCommandResp(string subsystem, const CommandString &cmd);
                
	private:
		MRef<SipStack *> sipstack;
		MRef<SipSoftPhoneConfiguration*> phoneconfig;
		MRef<MediaHandler *> mediaHandler;
		
		MRef<Thread *> thread;
};


#endif
