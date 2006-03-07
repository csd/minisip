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


#ifndef SIPSMCOMMAND_H
#define SIPSMCOMMAND_H

#include<libmsip/libmsip_config.h>

#include<libmsip/SipMessage.h>
#include<libmutil/CommandString.h>
#include<libmutil/MemObject.h>

/**
 * The object used to communicate in Minisip.
 * The communication between the differet components always happens
 * via a <code>SipSMCommand</code> object that can contain a SIP message
 * or a command string.<p>
 *
 * @author Erik Eliasson <eliasson@it.kth.se>
 */
class LIBMSIP_API SipSMCommand : public MObject{
	public:
		static const int COMMAND_PACKET;
		static const int COMMAND_STRING;
		static const int remote;
		static const int TU;
		static const int transaction;
		static const int ANY;
		static const int DIALOGCONTAINER;

		/**
		 * Constructor.
		 * @param cmd
		 * @param source
		 * @param destination
		 */
		SipSMCommand(const CommandString &cmd, int source, int destination);
		
		/**
		 * Constructor.
		 * @param cmd
		 * @param source
		 * @param destination
		 */
		SipSMCommand(MRef<SipMessage*> cmd, int source, int destination);

		virtual std::string getMemObjectType(){return "SipSMCommand";}

		int getType() const;

		int getSource() const;
		void setSource(int s);

		int getDestination() const;
		void setDestination(int s);

		MRef<SipMessage*> getCommandPacket() const;
		CommandString getCommandString() const;
#ifdef _WIN32_WCE
        friend LIBMSIP_API Dbg & operator<<(Dbg &, const SipSMCommand &);	
#else
        friend LIBMSIP_API ostream & operator<<(ostream &, const SipSMCommand &);
#endif

//		bool getDispatched() const{return dispatched;}
//		void setDispatched(bool d){dispatched = d;}
		void incDispatchCount(){dispatchCount++;}
		int getDispatchCount() const {return dispatchCount;}
		void setDispatchCount(char c){dispatchCount=c;}
	private:
		int type;
		CommandString cmdstr;
		MRef<SipMessage*> cmdpkt;
		int source;
		int destination;
		
//		bool dispatched;
		char dispatchCount;

};

/**
 * the receiver for SipSMCommand objects.
 */
class LIBMSIP_API SipSMCommandReceiver : public virtual MObject{
	public:
		/**
		 * handles the incoming commands.
		 * @param cmd a SipSMCommand
		 * @return true if the command was handled.
		 */
		virtual bool handleCommand(const SipSMCommand &cmd)=0;
};

#endif
