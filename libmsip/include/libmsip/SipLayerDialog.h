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


#ifndef _SipLayerDialog_H
#define _SipLayerDialog_H

#include<libmsip/libmsip_config.h>

#include<libmsip/SipSMCommand.h>

#include<list>
#include<libmutil/Mutex.h>
#include<libmutil/MemObject.h>
#include<libmutil/minilist.h>

class SipDialog;
class SipCommandDispatcher;


class LIBMSIP_API SipLayerDialog : public SipSMCommandReceiver{
	public:

		SipLayerDialog(MRef<SipCommandDispatcher*> dispatcher);

		~SipLayerDialog();
		
		void setDefaultDialogCommandHandler(MRef<SipDefaultHandler*> cb);
		MRef<SipDefaultHandler*> getDefaultDialogCommandHandler();
		
		void addDialog(MRef<SipDialog*> d);
		void removeTerminatedDialogs();

		virtual std::string getMemObjectType() const {return "SipLayerDialog";}
		
		virtual bool handleCommand(const SipSMCommand &cmd);
		
		std::list<MRef<SipDialog*> > getDialogs();

	private:
		MRef<SipDefaultHandler*> defaultHandler;

		minilist<MRef<SipDialog*> > dialogs;

                MRef<SipCommandDispatcher*> dispatcher;

		Mutex dialogListLock;
};

#endif
