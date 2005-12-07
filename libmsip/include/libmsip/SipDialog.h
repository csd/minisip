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


/*
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
 *	    Cesc Santasusana, c e s c dot s a n t a A{T g m a i l dot co m; 2005
*/

#ifndef SIPDIALOG_H
#define SIPDIALOG_H

#include<libmsip/libmsip_config.h>

#include<libmutil/StateMachine.h>
#include<libmutil/MemObject.h>
#include<libmsip/SipSMCommand.h>
#include<libmsip/SipResponse.h>
#include<libmsip/SipRequest.h>

using namespace std;

class SipStack;
class SipTransaction;
class SipDialogConfig;
class SipDialogContainer;


/**
The dialog's state information, as specified by the RFC.
The values to store change depending on wheather we are acting
as a UAC or UAS when the dialog is being established.
Check RFC 3261, Section 12.1
*/
class LIBMSIP_API SipDialogState{
	public:
		
		/**
		Establish a dialog acting as a UAS (receive a request)
		- routeSet = Record-Route header list, direct order
		- remote target = contact header from request
		- remote uri = From uri
		- local uri = To uri
		- remote tag = From tag
		- local tag = To tag
		- remote seq = CSEQ from request
		- local seq = empty
		- call-id = call-id value from request
		*/
		bool updateState( MRef<SipRequest*> inv );
		
		/**
		Establish a dialog acting as a UAC (send a request, create
		state from the response)
		- routeSet = Record-Route header list, inverse order
		- remote target = contact header from response
		- remote uri = To uri
		- local uri = From uri
		- remote tag = To tag
		- local tag = From tag
		- remote seq = empty
		- local seq = CSEQ of req
		- call-id = call-id value from request
		*/
		bool updateState( MRef<SipResponse*> resp);
		
		string callId;
		string localTag;
		string remoteTag;
		
		int seqNo;
		int remoteSeqNo;
		string localUri;	// not used yet

		/**
		When constructing the ACK or follow up in-dialog requests,
		we place as request-uri the remoteTarget (that is, the contact
		uri). But, if for whatever reason this is empty, we return the 
		the remoteUri (either the To or From uri) and give an error
		messge.
		*/
		string getRemoteTarget();
		string remoteUri;
		string remoteTarget;
		bool secure;
		list<string> routeSet;
	
		bool isEarly;
};


/**
 * A dialog is a long term relationship between two or more clients and
 * servers. SipDialog is the base class for all dialogs in libmsip. It
 * provides support for states and transitions, and contains a set of 
 * transactions.
 * 
 * @author Erik Eliasson, eliasson@it.kth.se
 */

class LIBMSIP_API SipDialog : public SipSMCommandReceiver, public StateMachine<SipSMCommand,string>{

	public:
		/**
		 * Constructor.
		 * @param dContainer The Dialog Container
		 * @param callconf   The Dialog Configuration
		 */
		SipDialog(MRef<SipStack*> stack, 
				MRef<SipDialogConfig*> callconf);
		
		/**
		 * Deconstructor.
		 */
		virtual ~SipDialog();
		
		virtual string getMemObjectType(){return "SipDialog";}

		/**
		 * The SipSMCommand handler.
		 * @return true if the command was handled by this call.
		 */		
		virtual bool handleCommand(const SipSMCommand &command);

		virtual string getName()=0;

		virtual void handleTimeout(const string &c);
		
		/**
		 * get the dialog configuration
		 * @return reference to the <code>SipDialogConfig</code>
		 */
		MRef<SipDialogConfig*> getDialogConfig();

		/**
		 * get the Call-Id of the dialog
		 * @return string containing the Call-Id
		 */
		string getCallId(){return dialogState.callId;}
		
		/**
		 * get a reference to the dialog container.
		 * @return reference to <code>SipDialogContainer</code>
		 */
		MRef<SipDialogContainer*> getDialogContainer();

		MRef<SipStack*> getSipStack();

		void signalIfNoTransactions();

		/**
		 * register a transaction for this dialog
		 * @param trans the SipTransaction that should be registered
		 */
		void registerTransaction(MRef<SipTransaction*> trans);

		list<string> getRouteSet(){return dialogState.routeSet;}
		
                list<MRef<SipTransaction*> > getTransactions(){return transactions;}

		SipDialogState dialogState;
		
	protected:
		///a list containing all transactions
		list<MRef<SipTransaction*> > transactions;

		//the dialog container
		//MRef<SipDialogContainer*> dialogContainer;

		///
		MRef<SipStack*> sipStack;

	private:
		
		///the dialog configuration
		MRef<SipDialogConfig*> callConfig;
};

#include<libmsip/SipStack.h>
#include<libmsip/SipDialogConfig.h>

#endif

