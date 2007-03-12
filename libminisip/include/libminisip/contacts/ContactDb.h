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

#ifndef CONTACT_DB_H
#define CONTACT_DB_H

#include<libminisip/libminisip_config.h>

#include<libmutil/MemObject.h>
#include<string>

class ContactDb;
class PhoneBookPerson;

#define CONTACT_STATUS_ONLINE 1
#define CONTACT_STATUS_OFFLINE 2
#define CONTACT_STATUS_UNKNOWN 3

class LIBMINISIP_API ContactEntry : public MObject{

	public:
		ContactEntry();
		ContactEntry( std::string uri, std::string desc, 
						MRef< PhoneBookPerson * > person = NULL );

		~ContactEntry();

		static void setDb( MRef<ContactDb *> db );

		std::string getName();
		std::string getUri();
		std::string getDesc();

		void setDesc( std::string desc );
		void setUri( std::string uri );

		uint32_t getId();

		bool isOnline(){return onlineStatus==CONTACT_STATUS_ONLINE;}
		bool isOffline(){return onlineStatus==CONTACT_STATUS_OFFLINE;}
		void setOnlineStatus(int s){onlineStatus=s;}
		void setOnlineStatusDesc( std::string s){onlineStatusDesc=s;}

		uint32_t getPersonIndex(){return personIndex;}

		virtual std::string getMemObjectType() const {return "ContactEntry";}
	private:
		static MRef<ContactDb *> db;

		uint32_t id;
		std::string uri;
		std::string desc;
		uint32_t type;
		MRef< PhoneBookPerson * > person;
		uint32_t personIndex;

		std::string location;
		int onlineStatus;
		std::string onlineStatusDesc;
                friend class PhoneBookPerson;
                friend class PhoneBook;
};

class LIBMINISIP_API ContactDb : public MObject{
	public:
		ContactDb();

		ContactEntry * lookUp(  std::string uri );
		ContactEntry * lookUp( uint32_t id );

		void addEntry( ContactEntry * entry );
		void delEntry( ContactEntry * entry );

		virtual std::string getMemObjectType() const {return "ContactDb";}

	private:

		std::list< ContactEntry * > entries;
};

#endif
