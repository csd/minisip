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

 +-SipStack-----------------------------------------+
 |                                              |  invite(string)
 | +-SipDialogContainer---+                     |<----------------------------------
 | |                      | addDialog(d)        |
 | |                      |<----------------    | handleCommand(SipSMCommand)
 | |                      |                     |<----------------------------------
 | |                      | handleCommand(CS)   |
 | |                      |-------------------->| sipcb_handleCommand(CommandString)
 | |                      |                     |---------------------------------->
 | |                      | sipcb_handleCommand |
 | |                      |-------------------->|  setDefaultDialog()
 | |                      |                     |<----------------------------------
 | |                      | enqueuePacket()     |
 | |                      |<----------------    |  addDialog(d)
 | |                      |                     |<----------------------------------
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

#ifndef LIBMSIP_SipStack_H
#define LIBMSIP_SipStack_H


#ifdef _MSC_VER
#ifdef LIBMSIP_EXPORTS
#define LIBMSIP_API __declspec(dllexport)
#else
#define LIBMSIP_API __declspec(dllimport)
#endif
#else
#define LIBMSIP_API
#endif


#include<libmutil/minilist.h>
#include<libmsip/SipCallback.h>
#include<libmsip/SipTransaction.h>
#include<libmsip/SipDialogContainer.h>

#include<libmsip/SipMessageTransport.h>
#include<libmutil/cert.h>

class SipDialog;
class SipTransaction;

using namespace std;

class LIBMSIP_API SipStack: public SipSMCommandReceiver, public Runnable{

	public:
		SipStack(MRef<SipDialog*> defaultDialog,
				string localIpString,
				string externalContactIP,
				int32_t localUdpPort=5060,
				int32_t localTcpPort=5060,
				int32_t externalContactUdpPort=5060,
				string defaultTransportProtocol="UDP"
				,int32_t localTlsPort=5061,
				MRef<certificate_chain *> cert=NULL,	//The certificate chain is used by TLS 
								//TODO: TLS should use the whole chain instead of only the first certificate --EE
				MRef<ca_db *> cert_db = NULL
			  );


		virtual std::string getMemObjectType(){return "SipStack";}
		
		void init();
                virtual void run();

		string invite(string &user);

		MRef<SipDialogContainer*> getDialogContainer();

		bool handleCommand(const SipSMCommand &command);
		
		void setCallback(SipCallback *callback);
		SipCallback *getCallback();

		void setDefaultHandler(MRef<SipDialog*> d);

		void addDialog(MRef<SipDialog*> d);

		MRef<SipMessageTransport *> getSipTransportLayer(){return transportLayer;}

		//void setSipMessageTransport(...);
	
                
	private:
		SipCallback *callback;
		MRef<SipDialogContainer*> dialogContainer;

		MRef<SipMessageTransport *> transportLayer;
};


//#include<libmsip/SipDialog.h>

#endif
