/*
 Copyright (C) 2009 the Minisip Team
 
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

/* Copyright (C) 2009
 *
 * Authors: Erik Eliasson <ere@kth.se>
*/

#ifndef SNAKECLIENT_H
#define SNAKECLIENT_H

#include<libminisip/libminisip_config.h>

#include<libmutil/MemObject.h>
#include<libmutil/Thread.h>
#include<libmutil/MessageRouter.h>
#include<libminisip/signaling/sip/SipSoftPhoneConfiguration.h>


class CallbackService : public Runnable {
	public:
		CallbackService(MRef<CommandReceiver*> callback, std::string id, std::string url);

		void run();
		void start();
		void stop();
	private:
		bool doStop;
		MRef<CommandReceiver*> callback;
		std::string id;
		std::string url;
		int64_t last_eid;
		MRef<Thread*> thread;

};


class PresenceService : public MObject{
	public:
		PresenceService(MRef<CommandReceiver*> callback, MRef<SipSoftPhoneConfiguration*> conf, std::string id, std::string url);

		void registerContacts();

		void statusUpdated();

	private:
		MRef<CommandReceiver*> callback;
		MRef<SipSoftPhoneConfiguration*> pconf;
		std::string id;
		std::string url;
};


class LIBMINISIP_API SnakeClient : public virtual CommandReceiver {
	public:
		SnakeClient(MRef<SipSoftPhoneConfiguration*> conf); //Phonebook in config needed for presence

		virtual void handleCommand(std::string subsystem, const CommandString& command);
		virtual CommandString handleCommandResp(std::string subsystem, const CommandString&);
		void setCallback(MRef<CommandReceiver*> mr){callback=mr;}

		virtual std::string getMemObjectType() const {return "SnakeClient";}
		
	private:
		MRef<SipSoftPhoneConfiguration*> pconf;
		MRef<CommandReceiver*> callback;
		MRef<CallbackService*> callbackService;
		MRef<PresenceService*> presenceService;

};

#endif
