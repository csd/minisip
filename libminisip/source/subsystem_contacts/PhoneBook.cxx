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

#include<config.h>

#include<libminisip/contacts/PhoneBook.h>

#include<libminisip/contacts/ContactDb.h>
#include"MXmlPhoneBookIo.h"

using namespace std;

void PhoneBook::save(){
	if( !io.isNull() ){
		io->save( this );
	}
}

MRef<PhoneBook * > PhoneBook::create( MRef<PhoneBookIo *> io ){
	if( !io.isNull() ){
		MRef<PhoneBook * > phonebook = io->load();
		if( !phonebook.isNull() ){
			phonebook->setIo( io );
		}
		return phonebook;
	}

	return NULL;
}

void PhoneBook::setIo( MRef<PhoneBookIo *> io_ ){
	this->io = io_;
}

void PhoneBook::setName( string n ){
	this->name = n;
}

string PhoneBook::getName(){
	return name;
}

std::list< MRef< PhoneBookPerson * > > PhoneBook::getPersons(){
	return persons;
}

void PhoneBook::addPerson( MRef< PhoneBookPerson * > person ){
	person->setPhoneBook( this );
	persons.push_back( person );
}

void PhoneBook::delPerson( MRef< PhoneBookPerson * > person ){
	list< MRef< PhoneBookPerson * > >::iterator i;
	
	for( i = persons.begin(); i != persons.end(); i++ ){
		if( *(*i) == *person ){
			i = persons.erase( i );
		}
	}
}

string PhoneBook::getPhoneBookId(){
	return io->getPhoneBookId();
}

PhoneBookPerson::PhoneBookPerson( std::string n ):name( n ){
}

PhoneBookPerson::~PhoneBookPerson(){
	if( entries.size() != 0 ){
		list< MRef<ContactEntry *> >::iterator i;
		
		for( i = entries.begin(); i != entries.end(); i++ ){
			delEntry( *i );
		}
	}

/*	if( !phoneBook.isNull() ){
		phoneBook->delPerson( this );
	}*/
}

void PhoneBookPerson::setName( string n){
	this->name = n;
}

string PhoneBookPerson::getName(){
	return name;
}

void PhoneBookPerson::addEntry( MRef<ContactEntry *> entry ){
        entry->personIndex = (uint32_t)entries.size();
	entries.push_back( entry );
}

void PhoneBookPerson::delEntry( MRef<ContactEntry *> entry ){
	list< MRef< ContactEntry * > >::iterator i;
        uint32_t index = 0;
	
	for( i = entries.begin(); i != entries.end(); i++ ){
		if( *(*i) == *entry ){
			i = entries.erase( i );
			if( i == entries.end() ){
				break;
			}
		}
		(*i)->personIndex = index;
		index++;
	}

	if( entries.size() == 0 ){
		phoneBook->delPerson( this );
	}
}

std::list< MRef<ContactEntry *> > PhoneBookPerson::getEntries(){
	return entries;
}

void PhoneBookPerson::setPhoneBook( MRef<PhoneBook *> phonebook ){
	this->phoneBook = phonebook;
}


PhoneBookIoDriver::PhoneBookIoDriver( MRef<Library *> lib ): MPlugin( lib ) {}
PhoneBookIoDriver::PhoneBookIoDriver(): MPlugin() {}

PhoneBookIoRegistry::PhoneBookIoRegistry(){
	registerPlugin( new MXmlPhoneBookIoDriver( NULL ) );
}

MRef<PhoneBook*> PhoneBookIoRegistry::createPhoneBook( const string &name )
{
	string driverId;
	string deviceId;

#ifdef DEBUG_OUTPUT
	mdbg << "PhoneBookIoRegistry: name =  " << name << endl;
#endif
	size_t pos = name.find( ':', 0 );
	if( pos == string::npos ){
		return NULL;
	}

	driverId = name.substr( 0, pos );

#ifdef DEBUG_OUTPUT
	mdbg << "PhoneBookIoRegistry: driverId =  " << driverId << endl;
#endif

	list< MRef<MPlugin*> >::iterator iter;
	list< MRef<MPlugin*> >::iterator stop = plugins.end();

	for( iter = plugins.begin(); iter != stop; iter++ ){
		MRef<MPlugin*> plugin = *iter;

		MRef<PhoneBookIoDriver*> driver = dynamic_cast<PhoneBookIoDriver*>(*plugin);

		if( !driver ){
			merr << "Not a PhoneBookIoDriver? " << plugin->getName() << endl;
		}

		if( driver && driver->getPrefix() == driverId ){
			MRef<PhoneBookIo*> io =
				driver->createPhoneBookIo( name );

			if( io ){
				return io->load();
			}
			else{
				mdbg << "PhoneBookIoRegistry: no io" << name << endl;
			}
		}
	}

	mdbg << "PhoneBookIoRegistry: device not found " << name << endl;
	return NULL;
}
