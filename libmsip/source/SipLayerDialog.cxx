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



#include<config.h>

#include<libmsip/SipDialog.h>
#include"SipLayerDialog.h"
#include<libmsip/SipSMCommand.h>
#include<libmsip/SipCommandString.h>
#include"SipCommandDispatcher.h"
#include"SipStackInternal.h"

using namespace std;

SipLayerDialog::SipLayerDialog(MRef<SipCommandDispatcher*> d):dispatcher(d){
	
}

SipLayerDialog::~SipLayerDialog(){
	std::map<std::string, MRef<SipDialog*> >::iterator i;
	dialogListLock.lock();
	for (i=dialogs.begin(); i!= dialogs.end(); i++)
		(*i).second->freeStateMachine();
	dialogListLock.unlock();
}

list<MRef<SipDialog*> > SipLayerDialog::getDialogs() {
	list<MRef<SipDialog*> > l;
	std::map<std::string, MRef<SipDialog*> >::iterator i;
	dialogListLock.lock();
	for (i=dialogs.begin(); i!=dialogs.end(); i++){
		l.push_back( (*i).second );
		massert( (*i).second );
	}
	dialogListLock.unlock();
	return l;
}

bool SipLayerDialog::removeDialog(string callId){
	MRef<SipDialog*> d = dialogs[callId];
	if (d){
		d->free();
		d->freeStateMachine();
	}
	size_t n = dialogs.erase(callId);
#ifdef DEBUG_OUTPUT
	if (n!=1){
		merr << "WARNING: dialogs.erase should return 1, but returned "<< (short)n<<endl;
	}
#endif
	return n==1;
}

void SipLayerDialog::addDialog(MRef<SipDialog*> d){
	massert(d);
	massert(d->dialogState.callId!="");
	dialogListLock.lock();
	dialogs[d->dialogState.callId] = d;
	dialogListLock.unlock();
}

/**
 *
 * @return If the dialog was not found, then a null reference is returned.
 *
 */
MRef<SipDialog*> SipLayerDialog::getDialog(string cid){
	MRef<SipDialog*> ret;
	map<string, MRef<SipDialog*> >::iterator i = dialogs.find(cid);
	if (i!=dialogs.end())
		ret = (*i).second;	
	return ret;
}

void SipLayerDialog::setDefaultDialogCommandHandler(MRef<SipDefaultHandler*> cb){
	defaultHandler=cb;
}
MRef<SipDefaultHandler*>  SipLayerDialog::getDefaultDialogCommandHandler(){
	return defaultHandler;
}

//TODO: Optimize how dialogs are found based on callid parameter.
bool SipLayerDialog::handleCommand(const SipSMCommand &c){
	assert(c.getDestination()==SipSMCommand::dialog_layer);

#ifdef DEBUG_OUTPUT
	mdbg("signaling/sip") << "SipLayerDialog: got command: "<< c <<endl;
#endif

	string cid = c.getDestinationId();

	try{
		MRef<SipDialog *> dialog;
		if (cid.size()>0){
			dialogListLock.lock();
			map<string, MRef<SipDialog*> >::iterator d = dialogs.find(cid);
			if (d != dialogs.end()){
				dialog = (*d).second;
			}
			dialogListLock.unlock();
			if ( dialog && dialog->handleCommand(c) )
				return true;
		}else{

			std::map<std::string, MRef<SipDialog*> >::iterator i;
			dialogListLock.lock();
			for (i=dialogs.begin(); i!=dialogs.end(); i++){
				dialog = (*i).second;
				dialogListLock.unlock();

				if ( dialog->handleCommand(c) ){
					return true;
				}
				dialogListLock.lock();
			}
			dialogListLock.unlock();
		}

		if (defaultHandler){
			//cerr << "SipLayerDialog: No dialog handled the message - sending to default handler"<<endl;
			return defaultHandler->handleCommand(c);
		}else{
			cerr << "ERROR: libmsip: SipLayerDialog::handleCommand: No default handler for dialog commands set!"<<endl;
			return false;
		}

	}catch(exception &e){
		cerr << "SipLayerDialog: caught exception: "<< e.what() << endl;
		return false;
	}
}


