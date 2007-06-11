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

#include<libminisip/libminisip_config.h>

#include<libmutil/CommandString.h>
#include<libmutil/Thread.h>
#include<libmutil/MessageRouter.h>
#include<libminisip/signaling/conference/ConfMessageRouter.h>

#include<libminisip/gui/LogEntry.h>

class SipSoftPhoneConfiguration;
class ContactDb;
class ConfMessageRouter;

class LIBMINISIP_API Gui : public Runnable, public CommandReceiver {
	public:
		virtual ~Gui();

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


		void handleCommand(std::string subsystem, const CommandString &cmd){
			assert(subsystem=="gui");
			handleCommand(cmd);
		}

		CommandString handleCommandResp(std::string , const CommandString& ){
			std::cerr << "Warning: Gui::handleCommandResp called (BUG)"<<std::endl;
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

		void sendCommand(std::string toSubsystem, const CommandString &cmd){
			callback->handleCommand(toSubsystem, cmd);
		}

		CommandString sendCommandResp(std::string toSubsystem, const CommandString &cmd){
			return callback->handleCommandResp(toSubsystem, cmd);
		}

		virtual bool configDialog( MRef<SipSoftPhoneConfiguration *> conf )=0;

		virtual void run()=0;

		
	protected:
		MRef<CommandReceiver*> callback;
		MRef<ConfMessageRouter*> confCallback;
};


#endif
