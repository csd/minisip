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
*/

#ifndef SIPDIALOG_H
#define SIPDIALOG_H

#ifdef _MSC_VER
#ifdef LIBMSIP_EXPORTS
#define LIBMSIP_API __declspec(dllexport)
#else
#define LIBMSIP_API __declspec(dllimport)
#endif
#else
#define LIBMSIP_API
#endif

#include<libmutil/StateMachine.h>
#include<libmutil/MemObject.h>
#include<libmsip/SipSMCommand.h>

using namespace std;

class SipStack;
class SipTransaction;
class SipDialogConfig;
class SipDialogContainer;


class LIBMSIP_API SipDialogState{
	public:
		string callId;
		string localTag;
		string remoteTag;
		
		int seqNo;
		int remoteSeqNo;
//		string localUri;	// not used yet
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
				MRef<SipDialogConfig*> callconf,
				MRef<TimeoutProvider<string, MRef<StateMachine<SipSMCommand,string>*> > *> timeoutProvider );
		
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

#include<libmsip/SipTransaction.h>
#include<libmsip/SipDialogConfig.h>
#include<libmsip/SipDialogContainer.h>
#include<libmsip/SipStack.h>


#endif

