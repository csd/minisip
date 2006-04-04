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


#ifndef _SIPMESSAGEDISPATCHER_H
#define _SIPMESSAGEDISPATCHER_H

#include<libmsip/libmsip_config.h>

#include<libmsip/SipSMCommand.h>

#include<list>
#include<libmutil/Mutex.h>
#include<libmutil/MemObject.h>
#include<libmutil/minilist.h>
#include<libmsip/SipTransaction.h>
#include<libmsip/SipDialog.h>

//class SipTransaction;

class LIBMSIP_API SipMessageDispatcher : public SipSMCommandReceiver{
	public:
		void addTransaction(MRef<SipTransaction*> t);
		void removeTransaction(MRef<SipTransaction*> t);
		void addDialog(MRef<SipDialog*> d);
		
//#ifdef DEBUG_OUTPUT
		virtual std::string getMemObjectType() {return "SipMessageDispatcher";}
//#endif
		
		virtual bool handleCommand(const SipSMCommand &cmd);
		
		std::list<MRef<SipDialog*> > getDialogs() {//return &dialogs;
			std::list<MRef<SipDialog*> > l;
			dialogListLock.lock();
			for (int i=0; i< dialogs.size(); i++)
				l.push_back(dialogs[i]);
			dialogListLock.unlock();
			return l;
		}

		/**
		//CESC::
		Needs to be moved to private and use set/get functions
		*/
		MRef<SipDialog*> managementHandler;
		                
	private:
		minilist<MRef<SipTransaction*> > transactions;
		minilist<MRef<SipDialog*> > dialogs;
		Mutex dialogListLock;
};

#endif
