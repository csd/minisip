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

#ifndef SIPDIALOG_H
#define SIPDIALOG_H

#include<libmsip/SdpPacket.h>

#include<libmutil/StateMachine.h>
#include<libmutil/MemObject.h>
#include<libmsip/SipSMCommand.h>

using namespace std;

class SipDialogContainer;
class SipTransaction;
class SipDialogConfig;

/**
 * A dialog is a long term relationship between two or more clients and
 * servers. SipDialog is the base class for all dialogs in libmsip. It
 * provides support for states and transitions, and contains a set of 
 * transactions.
 * 
 * @author Erik Eliasson, eliasson@it.kth.se
 */

class SipDialog : public SipSMCommandReceiver, public StateMachine<SipSMCommand,string>{

	public:
		/**
		 * Constructor.
		 * @param dContainer The Dialog Container
		 * @param callconf   The Dialog Configuration
		 */
		SipDialog(MRef<SipDialogContainer*> dContainer, 
//				const SipDialogConfig &callconf, 
				MRef<SipDialogConfig*> callconf,
				MRef<TimeoutProvider<string, MRef<StateMachine<SipSMCommand,string>*> > *> timeoutProvider );
		
		/**
		 * Deconstructor.
		 */
		virtual ~SipDialog();

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

		int32_t requestSeqNo();
		void setSeqNo(int32_t seq);
		
		/**
		 * get the Call-Id of the dialog
		 * @return string containing the Call-Id
		 */
		string getCallId(){return callId;}
		
		/**
		 * get a reference to the dialog container.
		 * @return reference to <code>SipDialogContainer</code>
		 */
		MRef<SipDialogContainer*> getDialogContainer();

		void signalIfNoTransactions();

		/**
		 * register a transaction for this dialog
		 * @param trans the SipTransaction that should be registered
		 */
		void registerTransaction(MRef<SipTransaction*> trans);

		list<string> getRouteSet(){return routeSet;}
		
                list<MRef<SipTransaction*> > getTransactions(){return transactions;}

	protected:
		///a list containing all transactions
		list<MRef<SipTransaction*> > transactions;

		list<string> routeSet;
		
		///the dialog container
		MRef<SipDialogContainer*> dialogContainer;
		string callId;

	private:
		
		///the dialog configuration
		MRef<SipDialogConfig*> callConfig;
};

#include<libmsip/SipTransaction.h>
#include<libmsip/SipDialogContainer.h>
#include<libmsip/SipDialogConfig.h>


#endif

