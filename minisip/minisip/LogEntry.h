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

#ifndef LOG_ENTRY_H
#define LOG_ENTRY_H

#include<libmutil/MemObject.h>

class LogEntryHandler;

class LogEntry : public MObject{
	public:

		virtual std::string getMemObjectType(){return "LogEntry";}
		
		int type;
		string peerSipUri;
		time_t start;
		static LogEntryHandler * handler;
		void handle();
};

class LogEntrySuccess : public LogEntry {
	public:
		time_t duration;
		bool secured;
		int mos;
};

class LogEntryFailure : public LogEntry {
	public:
		string error;

};

class LogEntryIncoming{};
class LogEntryOutgoing{};

class LogEntryMissedCall:public LogEntryFailure, public LogEntryIncoming{

};

class LogEntryCallRejected:public LogEntryFailure, public LogEntryOutgoing{

};

class LogEntryIncomingCompletedCall : public LogEntryIncoming, public LogEntrySuccess{

};

class LogEntryOutgoingCompletedCall : public LogEntryOutgoing, public LogEntrySuccess{
};

class LogEntryHandler{
	public:
		virtual void handle( MRef<LogEntry *> )=0;
};


#endif
