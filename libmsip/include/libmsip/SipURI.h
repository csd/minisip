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
 * 	SipURI.h
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/



#ifndef SIPURI_H
#define SIPURI_H

#define SIP_URI_USERNAME_DEFAULT "UNKNOWN"
#define SIP_URI_USER_TYPE_DEFAULT "phone"

#include<sys/types.h>

#include<libmutil/MemObject.h>

using namespace std;

class SipURI : public MObject{
	public:
		SipURI(string build_from);
		SipURI(string id, string ip, string type=/*"phone"*/ "",int32_t port=0);
                virtual std::string getMemObjectType(){return "SipURI";}

		void setUserId(string id);
		string getUserId();

		void setIp(string ip);
		string getIp();

		void setPort(int32_t port);
		int32_t getPort();

		void setUserType(string type);
		string getUserType();

		string getString();
		string getUserIpString();

		void setTransport(string transp);
		string getTransport();

		void setUsersName(string users_name);
		string getUsersName();
	private:
		string users_name;
		string user_id;
		string ip;
		int32_t port;
		string type;
		string transport;
		//string tag;
};

#endif
