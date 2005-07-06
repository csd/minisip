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


/* Copyright (C) 2004 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#ifndef PHONEBOOK_H
#define PHONEBOOK_H

#include<config.h>

#include<libmutil/MemObject.h>
#include<list>



class PhoneBookPerson;
class PhoneBookIo;
class ContactEntry;


class PhoneBook : public MObject{
	public:
		static MRef<PhoneBook *> create( MRef< PhoneBookIo * > io );
		
		void save();

		void setIo( MRef<PhoneBookIo *> );
		void setName( std::string name );
		std::string getName();

		void addPerson( MRef< PhoneBookPerson * > person );
		void delPerson( MRef< PhoneBookPerson * > person );

		std::list< MRef< PhoneBookPerson * > > getPersons();

		std::string getPhoneBookId();
		
		virtual std::string getMemObjectType(){return "PhoneBook";};

	private:
		MRef<PhoneBookIo *> io;
		
		std::list< MRef< PhoneBookPerson * > > persons;
		std::string name;

};

class PhoneBookIo : public MObject{
	public:
		virtual void save( MRef< PhoneBook * > book )=0;
		virtual MRef< PhoneBook * > load()=0;
		virtual std::string getPhoneBookId()=0;
};

class PhoneBookPerson : public MObject{
	public:
		PhoneBookPerson( std::string name );
		~PhoneBookPerson();
		std::string getName();

		void setName( std::string name );
		void setPhoneBook( MRef<PhoneBook *> phoneBook );

		std::list< MRef<ContactEntry *> > getEntries();

		void addEntry( MRef<ContactEntry *> );
		void delEntry( MRef<ContactEntry *> );
		
		virtual std::string getMemObjectType(){return "PhoneBookPerson";}
	private:
		std::string name;
		MRef< PhoneBook * > phoneBook;
		MRef< ContactEntry * > defaultEntry;
		std::list< MRef<ContactEntry *> > entries;

};
#endif
