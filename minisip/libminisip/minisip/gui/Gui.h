/*
 Copyright (C) 2004-2006 the Minisip Team
 
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
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */

/* Copyright (C) 2004 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#ifndef GUI_H
#define GUI_H

#include<config.h>
#include<libmutil/CommandString.h>
#include<libmutil/Thread.h>
#include<libmutil/MessageRouter.h>
#include"../LogEntry.h"

class SipSoftPhoneConfiguration;
class ContactDb;
class ConfMessageRouter;

class Gui : public Runnable, public CommandReceiver {
	public:
		virtual ~Gui();

		/**
		* Purpose: Displays an error message to the user. This
		* method must be able to display messages before the GUI
		* is "run" (because of errors when initializing the
		* application). (Error handling method)
		* 
		* @param s     Message that will be displayed to the
		* user.
		*/
		//virtual void displayErrorMessage(string s)=0;

		/**
		* Purpose: The user interface will probably need a
		* reference to the SipSoftPhone. After the GUI has
		* been created and before it is "run" a pointer to
		* the SipSoftPhone will be set. (Initialization method)
		* @param sipphone      Pointer to the SipSoftPhone of the
		*                      application.
		*/
		virtual void setSipSoftPhoneConfiguration(MRef<SipSoftPhoneConfiguration *> sipphoneconfig)=0;

		/**
                 * Purpose: The user interface may use the application
		 * contact database to look up names given SIP URIs.
		 * After the GUI has been created and before it is "run",
		 * a pointer to the ContactDb will be set. 
		 * (Initialization method)
		* @param contactDb     Pointer to the Contact database 
		*                      of the application
		*/
		virtual void setContactDb(MRef<ContactDb *> contactDb)=0;


		/**
		* Purpose: The GUI and the rest of the application
		* communicates by passing messages to each other. The
		* handleCommand passes messages TO the gui.
		* @param command       Message that is passed to the
		*                      user interface.
		*/
		virtual void handleCommand(const CommandString &command)=0;


		void handleCommand(string subsystem, const CommandString &cmd){
			assert(subsystem=="gui");
			handleCommand(cmd);
		}

		CommandString handleCommandResp(string , const CommandString& ){
			cerr << "Warning: Gui::handleCommandResp called (BUG)"<<endl;
			CommandString ret("","");
			return ret;
		}

		/**
		* Purpose: The GUI and the rest of the application
		* communicates by passing messages to each other. The
		* set_callback method sets the object capable of receiving
		* messages FROM the gui.
		* @param callback      Pointer to the object that will 
		*                      receive messages FROM the GUI.
		*/
		virtual void setCallback(MRef<CommandReceiver*> cb);
		
		MRef<CommandReceiver*> getCallback();

		virtual void setConfCallback(MRef<ConfMessageRouter*> cb);
		
		MRef<ConfMessageRouter*> getConfCallback();

		void sendCommand(string toSubsystem, const CommandString &cmd){
			callback->handleCommand(toSubsystem, cmd);
		}


		// Lesson learned: Doing logging in the GUI is not good
		// since when ever something interesting happend it is
		// likely that the GUI has been shut down. 
//		virtual void log(int type, string msg){}; 
		
//		virtual void gotPacket(int32_t i)=0;

		virtual bool configDialog( MRef<SipSoftPhoneConfiguration *> conf )=0;

		virtual void run()=0;

		
	protected:
		MRef<CommandReceiver*> callback;
		MRef<ConfMessageRouter*> confCallback;
		//ConfCallback *confCallback;
		//GuiCallback *callback;
};


#endif
