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

/* Name
 * 	SipNotify.h
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/



#ifndef SIPIMMESSAGE_H
#define SIPIMMESSAGE_H

#include<libmsip/SipMessage.h>
#include<libmsip/SipDialogConfig.h>
#include<libmutil/MemObject.h>

class SipIMMessage : public SipMessage{

	public:
		static const int type;
                
		SipIMMessage(string &build_from);

		SipIMMessage(string branch,
				string call_id, 
				MRef<SipIdentity*> toIdentity,
				MRef<SipIdentity*> fromIdentity,
                                int local_port,
				int32_t seq_no,
				string msg
				);
		
		virtual ~SipIMMessage();

		virtual std::string getMemObjectType(){return "SipIMMessage";}

		virtual string getString();
		
	private:
		MRef<SipIdentity *> fromIdentity;
		string toUser; //telephone number for example
		string toDomain;

};

#endif

