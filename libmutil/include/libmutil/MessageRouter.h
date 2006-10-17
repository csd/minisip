/*
  Copyright (C) 2006 Erik Eliasson

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

#ifndef MESSAGE_ROUTER_H
#define MESSAGE_ROUTER_H

#include <libmutil/libmutil_config.h>

#include<libmutil/mtypes.h>
#include<libmutil/MemObject.h>
#include<libmutil/CommandString.h>
#include<libmutil/Exception.h>
#include<string>

/**
 * Thrown when handleCommand/handleCommandResp in SipMessageRouter
 * is called with a subsystem that is not registred (using the
 * addSubsystem in the MessageRouter class).
 */
class LIBMUTIL_API SubsystemNotFoundException : public Exception{
	public:
		/**
		 * @param what	Name of the subsystem not found. The name
		 * 		can be retrieved using the what() method.
		 */
		SubsystemNotFoundException(const char *what);
};

/**
 * Interface for a subsystem that receives CommandString messages.
 */
class LIBMUTIL_API CommandReceiver : public virtual MObject{
	public:
		/**
		 * Note that the second argument is sent as reference (&) and
		 * not as the more appropriate const reference (const&).
		 * This is only because of how CommandString is implemented
		 * (internally it uses a map<> and retrieving values from
		 * it is not declared as const and therefore inspecting
		 * constant CommandStrings is not possible) and classes 
		 * implementing CommandReceiver interface SHOULD NOT modify 
		 * the command parameter.
		 * 
		 * @param subsystem	Name of the subsystem that the message 
		 * 			is being sent to. This parameter can 
		 * 			be inspected if a class implements 
		 * 			several subsystems.
		 * @param command	Contains destination id, operation
		 * 			with parameters.
		 */
		virtual void handleCommand(std::string subsystem, const CommandString& command)=0;

		/**
		 * Same as handleCommand except that it has a return
		 * value.
		 */
		virtual CommandString handleCommandResp(std::string subsystem, const CommandString&)=0;
};

class MessageRouterInternal;

/**
 * Implements message passing to subsystems. Subsystems are registred with
 * a name and messages are passed with subsystem name as destination.
 * Subsystems must implement the CommandReceiver interface and the 
 * handleCommand and handleCommandResp is used to pass messages.
 *
 * If a command is sent to a unknown subsystem the MessageRouter will
 * throw a SubsystemNotFoundException exception.
 *
 */
class LIBMUTIL_API MessageRouter : public CommandReceiver{
	public:
		MessageRouter();
		MessageRouter(const MessageRouter &);
		~MessageRouter();

		/**
		 * Returns true if a subsystem with the name passed as
		 * parameter has been registred with this MessageRouter
		 * instance.
		 * @param name	Name of a subsystem.
		 * @return	true if a subsystem with that name has been
		 * 		registred and false if not.
		 *
		 */
		bool hasSubsystem(std::string name);

		/**
		 * Registers a subsystem.
		 *
		 * @param name	Name of the subsystem
		 * @param ss	Subsystem that will receive messagages.
		 * @return True if the subsystem was successfully added.
		 * 	   False if the subsystem already existed and could
		 * 	   not be added.
		 */
		bool addSubsystem(std::string name, MRef<CommandReceiver*> ss);

		/**
		 * @param subsystem	Destination subsystem name.
		 * @param cmd		Command that will be passed to the
		 * 			handleCommand method of the
		 * 			subsystem object registred using
		 * 			addSubsystem.
		 */
		void handleCommand(std::string subsystem, const CommandString& cmd);

		/**
		 *
		 * @param subsystem	Destination subsystem name.
		 * @param cmd		Command that will be passed to the
		 * 			handleCommand method of the
		 * 			subsystem object registred using
		 * 			addSubsystem.
		 * @return Return value of the handleCommandResp method of
		 * 	   the subsystem.
		 */
		CommandString handleCommandResp(std::string subsystem, const CommandString& cmd);

		/**
		 * Removes all subsystems.
		 */
		void clear();
		
	private:
		MessageRouterInternal *internal;
};

#endif
