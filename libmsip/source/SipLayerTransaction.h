/*
  Copyright (C) 2006, Erik Eliasson
  
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
*/


#ifndef SIPLAYERTRANSACTION_H
#define SIPLAYERTRANSACTION_H

#include<libmsip/libmsip_config.h>

#include<libmsip/SipSMCommand.h>

#include<list>
#include<libmutil/Mutex.h>
#include<libmutil/MemObject.h>
#include<map>

class SipCommandDispatcher;
class SipTransaction;
class SipLayerTransport;
class SipRequest;

class SipLayerTransaction: public SipSMCommandReceiver{
	public:
		SipLayerTransaction(MRef<SipCommandDispatcher*> dispatcher, 
				MRef<SipLayerTransport*> transp);

		~SipLayerTransaction();

		void doHandleAck(bool b);
		
		/**
		 * @param A transaction ID is composed of the branch
		 * 	parameter AND the CSeq method part concatenated
		 */
		void removeTransaction(std::string transactionId);

		/**
		 * @param A transaction ID is composed of the branch
		 * 	parameter AND the CSeq method part concatenated
		 */
		MRef<SipTransaction*> getTransaction(std::string transactionId);

		std::list<MRef<SipTransaction*> > getTransactions();
		
		std::list<MRef<SipTransaction*> > getTransactionsWithCallId(std::string callid);
		
		virtual std::string getMemObjectType() const {return "SipLayerTransaction";}
		
		virtual bool handleCommand(const SipSMCommand &cmd);

		/**
		 * Creates client transaction, and let's it start handle
		 * request.
		 *
		 * @return transaction ID (branch) of the newly created
		 * 	transaction.
		 */
		std::string createClientTransaction( MRef<SipRequest*> req );
		
	private:
		void addTransaction(MRef<SipTransaction*> t);

		bool defaultCommandHandler(const SipSMCommand &cmd);
		
		bool handleAck;
		
		std::map<std::string, MRef<SipTransaction*> > transactions;

		MRef<SipCommandDispatcher*> dispatcher;
		MRef<SipLayerTransport*> transportLayer;
};

#endif
