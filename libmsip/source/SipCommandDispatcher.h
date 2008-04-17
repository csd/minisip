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


#ifndef _SipCommandDispatcher_H
#define _SipCommandDispatcher_H

#include<libmsip/libmsip_config.h>

#include<libmsip/SipSMCommand.h>

#include<list>
#include<libmutil/Mutex.h>
#include<libmutil/MessageRouter.h>
#include<libmutil/MemObject.h>
#include<libmutil/minilist.h>
#include"transactions/SipTransaction.h"
#include"SipLayerDialog.h"
#include"SipLayerTransaction.h"
#include"SipLayerTransport.h"
#include<libmsip/SipStack.h>

class SipStackInternal;
class SipDialog;

/**
 * queue_type: For internal use only!
 * An item in the priority queue of commands to dispatch.
 */
typedef struct queue_type{
        int type;
        MRef<SipSMCommand*> command;
        MRef<SipTransaction*> transaction_receiver;
        MRef<SipDialog*> call_receiver;
} queue_type;

class SipLayerDialog;
class SipLayerTransport;

#define TYPE_COMMAND 2
#define TYPE_TIMEOUT 3


class SipCommandDispatcher : public MObject{
	public:
		SipCommandDispatcher(MRef<SipStackInternal*> stack, MRef<SipLayerTransport*> transport);

		void free();

		void setCallback(MRef<CommandReceiver*> cb);
		MRef<CommandReceiver*> getCallback();
		
		void addDialog(MRef<SipDialog*> d);
		std::list<MRef<SipDialog *> > getDialogs();

		void setDialogManagement(MRef<SipDialog*> mgmt);

		virtual void run();
		void stopRunning();

		MRef<SipStackInternal*> getSipStackInternal();
		
//#ifdef DEBUG_OUTPUT
		virtual std::string getMemObjectType() const {return "SipCommandDispatcher";}
//#endif
		
		virtual bool handleCommand(const SipSMCommand &cmd);
		bool dispatch(const SipSMCommand &cmd);

 		bool maintainenceHandleCommand(const SipSMCommand &cmd);
		
		void enqueueCommand(const SipSMCommand &cmd, int queue=LOW_PRIO_QUEUE);

		void enqueueTimeout(MRef<SipTransaction*> receiver, const SipSMCommand &);
		void enqueueTimeout(MRef<SipDialog*> receiver, const SipSMCommand &);


		MRef<SipLayerTransport*> getLayerTransport();
		MRef<SipLayerTransaction*> getLayerTransaction();
		MRef<SipLayerDialog*> getLayerDialog();


		/**
		//CESC::
		Needs to be moved to private and use set/get functions
		*/
		MRef<SipDialog*> managementHandler;

		/**
		 * If set to true, the SipStack will send the following SipSMCommand from 
		 * the transaction_layer to the dialog_layer:
		 * 	op=transaction_terminated
		 *      param=transaction id
		 * This makes it possible for applications to know when a
		 * transaction has terminated, and is/will be removed from
		 * memory.
		 */
		void setInformTransactionTerminate(bool doInform){informTuOnTransactionTerminate=doInform;}
		                
	private:
		MRef<CommandReceiver*> callback;
		MRef<SipStackInternal *> sipStackInternal;

		Semaphore semaphore;
		Mutex mlock;
                minilist<queue_type> high_prio_command_q;
                minilist<queue_type> low_prio_command_q;

		
                //
                MRef<SipLayerDialog*> dialogLayer;

                //
                MRef<SipLayerTransaction*> transactionLayer;

                //
                MRef<SipLayerTransport *> transportLayer;
		
		Mutex dialogListLock;

                /**
                We will use this to stop the dialog container :: run()
                on stack shutdown.
                */
                bool keepRunning;

		/**
		 * If true, the SipStack will send the following SipSMCommand from 
		 * the transaction_layer to the dialog_layer:
		 * 	op=transaction_terminated
		 *      param=transaction id
		 */
		bool informTuOnTransactionTerminate;

};

#include<libmsip/SipDialog.h>
#include"SipStackInternal.h"

#endif
