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

/* Name
 * 	SipTransaction.h
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/


#ifndef SIPTRANSACTION_H
#define SIPTRANSACTION_H

#include<libmsip/SipMessage.h>
#include<libmsip/SipSMCommand.h>
#include<libmsip/SipMessageDispatcher.h>
#include<libmutil/StateMachine.h>
#include<libmutil/MemObject.h>

class SipDialog;

/**
 * SipTransaction
 */
class SipTransaction : public StateMachine<SipSMCommand,string>{
	public:
		
		SipTransaction(const string &memType, MRef<SipDialog*> d, const string &branch, string callid);
                
		virtual ~SipTransaction(){};
		
		virtual string getName()=0;

	

		virtual bool handleCommand(const SipSMCommand &command)=0;

		virtual void handleTimeout(const string &c);
		
		void setBranch(const string &b);
		string getBranch();
		
		void setDialog(MRef<SipDialog*> );
		void send(MRef<SipMessage*>  pack, string branch=""); // if not specified branch, use the attribute one - ok in most cases.
		void setSocket(Socket * sock){socket=sock;};
		Socket * getSocket(){return socket;};

                virtual std::string getMemObjectType(){return "SipTransaction";}
		void setDebugTransType(string t){debugTransType = t;}
		string getDebugTransType(){return debugTransType;}
                
	protected:
		MRef<SipDialog*> dialog; 
		Socket * socket;
		IPAddress * toaddr;             //FIXME: This is leaked?
		int32_t port;
		string callId;
	private:
		MRef<SipMessageDispatcher*> dispatcher;
		string command;
		string branch;

		string debugTransType;
};

#endif
