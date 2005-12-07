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


/* Name
 * 	SipTransaction.h
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/


#ifndef SIPTRANSACTION_H
#define SIPTRANSACTION_H

#include<libmsip/libmsip_config.h>

#include<libmutil/StateMachine.h>
#include<libmutil/MemObject.h>
#include<libmsip/SipSMCommand.h>
#include<libmsip/SipDialogConfig.h>

class SipDialog;
class SipMessage;
class SipMessageDispatcher;
class SipStack;
class Socket;

/**
 * SipTransaction
 */
class LIBMSIP_API SipTransaction : public StateMachine<SipSMCommand,string>{
	public:
		
		SipTransaction(MRef<SipStack*> stack, MRef<SipDialog*> d, int cseq, const string &cseqMethod, const string &branch, string callid);
                
		virtual ~SipTransaction();
		
		virtual string getName()=0;

		virtual bool handleCommand(const SipSMCommand &command);

		virtual void handleTimeout(const string &c);
		
		string getBranch();
		void setBranch(string branch);
				
		void setDialog(MRef<SipDialog*> );
		void send(MRef<SipMessage*>  pack, bool addVia, string branch=""); // if not specified branch, use the attribute one - ok in most cases.
		void setSocket(Socket * sock){socket=sock;};
		Socket * getSocket(){return socket;};

		virtual string getMemObjectType(){return "SipTransaction";}
		void setDebugTransType(string t){debugTransType = t;}
		string getDebugTransType(){return debugTransType;}

		int getCSeqNo(){return cSeqNo;}

		string getCSeqMethod(){return cSeqMethod;}
                

		//The transition to cancel a transaction is common to all
		//transactions and is defined in this class.
		bool a1000_cancel_transaction(const SipSMCommand &command);

		//FIXME: get the reliability correctly
		bool isUnreliable();

		
	protected:
		/**
		 * The transaction can be either within or not within
		 * a dialog. This function returns the configuration
		 * that is the one of the dialog (if within it) or
		 * the one of the sip stack (if not within a dialog).
		 */
		MRef<SipCommonConfig *> getConfig();
		
		void setCSeqNo(int n){cSeqNo=n;}
		MRef<SipStack*> sipStack;
		MRef<SipDialog*> dialog; 
		Socket * socket;
		//IPAddress * toaddr;             //FIXME: This is leaked?
		int32_t port;
		string transport;
		string callId;

		
	private:
		MRef<SipMessageDispatcher*> dispatcher;
		string command;
		int cSeqNo;
		string cSeqMethod;
		string branch;

		string debugTransType;
};


class LIBMSIP_API SipTransactionClient: public SipTransaction{
        public:
                SipTransactionClient(MRef<SipStack*> stack, MRef<SipDialog*> d, int seq_no, const string &cSeqMethod, const string &branch, string callid);
                ~SipTransactionClient();
};

class LIBMSIP_API SipTransactionServer: public SipTransaction{
        public:
                SipTransactionServer(MRef<SipStack*> stack, MRef<SipDialog*> d, int seq_no, const string &cSeqMethod, const string &branch, string callid);
                ~SipTransactionServer();
};

#endif
