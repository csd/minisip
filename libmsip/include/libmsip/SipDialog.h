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

#include<libmsip/SipCommandDispatcher.h>
#include<libmsip/SipRequest.h>

class SipStack;
class SipTransaction;
class SipDialogConfig;
class SipCommandDispatcher;
//class SipDialogContainer;


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
		
		std::string callId;
		std::string localTag;
		std::string remoteTag;
		
		int seqNo;
		int remoteSeqNo;
		std::string localUri;	// not used yet

		/**
		When constructing the ACK or follow up in-dialog requests,
		we place as request-uri the remoteTarget (that is, the contact
		uri). But, if for whatever reason this is empty, we return the 
		the remoteUri (either the To or From uri) and give an error
		messge.
		*/
		std::string getRemoteTarget();
		std::string remoteUri;
		std::string remoteTarget;
		bool secure;
		std::list<std::string> routeSet;
	
		bool isEarly;
		bool isEstablished;
};


/**
 * A dialog is a long term relationship between two or more clients and
 * servers. SipDialog is the base class for all dialogs in libmsip. It
 * provides support for states and transitions, and contains a set of 
 * transactions.
 * 
 * @author Erik Eliasson, eliasson@it.kth.se
 */

class LIBMSIP_API SipDialog : public SipSMCommandReceiver, public StateMachine<SipSMCommand,std::string>{

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
		
		virtual std::string getMemObjectType(){return "SipDialog";}

		/**
		 * The SipSMCommand handler.
		 * @return true if the command was handled by this call.
		 */		
		virtual bool handleCommand(const SipSMCommand &command);

		virtual std::string getName()=0;

		virtual void handleTimeout(const std::string &c);
		
		/**
		 * get the dialog configuration
		 * @return reference to the <code>SipDialogConfig</code>
		 */
		MRef<SipDialogConfig*> getDialogConfig();

		/**
		 * get the Call-Id of the dialog
		 * @return string containing the Call-Id
		 */
		std::string getCallId(){return dialogState.callId;}
		
		//MRef<SipCommandDispatcher*> getDispatcher();

		MRef<SipStack*> getSipStack();

		void signalIfNoTransactions();

		/**
		 * Register a transaction to this dialog. This does
		 * not mean that it is added to the transaction layer.
		 * 
		 * @param trans the SipTransaction that should be associated
		 *        with this dialog.
		 */
//		void registerTransactionToDialog(MRef<SipTransaction*> trans);

		std::list<std::string> getRouteSet(){return dialogState.routeSet;}
		
		/**
		 * Add the dialog route set if the dialog is established, 
		 * or proxy route to a request outside of the dialog
		 */
		void addRoute( MRef<SipRequest *> req );
  
		/**
		 * Returns all dialogs within this transaction (all
		 * transactions that have the same call id as this 
		 * dialog).
		 */
                std::list<MRef<SipTransaction*> > getTransactions();//{return transactions;}

		SipDialogState dialogState;

		/** Create a request within the dialog */
		MRef<SipRequest*> createSipMessage( const std::string &method );

		/** Create an ACK request within the dialog */
		MRef<SipRequest*> createSipMessageAck( MRef<SipRequest *> origReq );

		/** Create a BYE request within the dialog */
		MRef<SipRequest*> createSipMessageBye();

		/** Create a REFER request within the dialog */
		MRef<SipRequest*> createSipMessageRefer( const std::string &referredUri );

		MRef<SipResponse*> createSipResponse( MRef<SipRequest*> req, int32_t status, const std::string &reason );

		/** Send a Sip message to the transaction layer */
		void sendSipMessage( MRef<SipMessage*> msg, int queue=HIGH_PRIO_QUEUE );

	protected:
//		///a list containing all transactions
//		std::list<MRef<SipTransaction*> > transactions;

		///
		MRef<SipStack*> sipStack;
		
		MRef<SipCommandDispatcher*> dispatcher;

	private:
		
		///the dialog configuration
		MRef<SipDialogConfig*> callConfig;

		MRef<SipRequest*> createSipMessageSeq( const std::string &method, int seqNo );
};

#include<libmsip/SipStack.h>
#include<libmsip/SipDialogConfig.h>
#include<libmsip/SipCommandDispatcher.h>

#endif

