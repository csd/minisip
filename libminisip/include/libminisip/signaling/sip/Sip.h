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


#ifndef MINISIP_SIP_H
#define MINISIP_SIP_H

#include<libminisip/libminisip_config.h>

#include<libmutil/minilist.h>

#include<libmsip/SipDialog.h>
#include<libmsip/SipStack.h>

#include<libminisip/signaling/sdp/SdpPacket.h>
#include<libminisip/signaling/conference/ConfMember.h>
#include<libminisip/gui/LogEntry.h>
#include<libminisip/media/SubsystemMedia.h>

class SipSoftPhoneConfiguration;
class SubsystemMedia;

class LIBMINISIP_API Sip: public Runnable{

	public:
		Sip(MRef<SipSoftPhoneConfiguration*> phoneconfig,
		    MRef<SubsystemMedia*> subsystemMedia);

		virtual ~Sip();

		virtual std::string getMemObjectType() const {return "Sip";}
		
		MRef<SipSoftPhoneConfiguration*> getPhoneConfig();
		
		/**
		Starts this Sip object as a Thread.
		You can still run the Sip object in non-thread by directly calling
		the run() function
		*/
		bool start();
		
		/**
		It sends the sip_stack_shutdown command, to stop the sip stack in a 
		controlled way.
		While you wait, you can use the join function.
		*/
		void stop();
		
		/**
		Join the sip thread (if this was started with start(), that is, as a 
		Thread. It will block and not return until the run function returns.
		*/
		void join();
		
		/**
		Main method. It can be run either directly with sip->run() or 
		to be run in a Thread (use sip->start())
		*/
		virtual void run();

		std::string confjoin(std::string &user, minilist<ConfMember> *conflist, std::string confId);
		std::string confconnect(std::string &user, std::string confId);
		MRef<SipStack*> getSipStack(){return sipstack;}

		void setMediaHandler( MRef<SubsystemMedia*> subsystemMedia);

	private:
		MRef<SipStack *> sipstack;
		MRef<SipSoftPhoneConfiguration*> phoneconfig;
		MRef<SubsystemMedia *> subsystemMedia;
		
		MRef<Thread *> thread;
};


#endif
