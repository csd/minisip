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

#include<libminisip/contacts/ContactDb.h>
#include<libminisip/contacts/PhoneBook.h>

#include<stdlib.h>

using namespace std;

MRef<ContactDb *> ContactEntry::db = NULL;

ContactEntry::ContactEntry():person(NULL), onlineStatus(CONTACT_STATUS_UNKNOWN){
	if( ! db.isNull() ){
		db->addEntry( this );
	}
        personIndex = 0;
	id = rand();

}

ContactEntry::ContactEntry( string uri_, string desc_, 
		MRef<PhoneBookPerson *> p):
	uri(uri_),
	desc(desc_),
	person(p),
	onlineStatus(CONTACT_STATUS_UNKNOWN){
	
	
	if( ! db.isNull() ){
                db->addEntry( this );
        }
        
	id = rand();
}

ContactEntry::~ContactEntry(){
	if( ! db.isNull() ){
		db->delEntry( this );
	}


}

void ContactEntry::setDb( MRef<ContactDb *> d){
	ContactEntry::db = d;
}

string ContactEntry::getName(){
	if( !person.isNull() )
		return person->getName();
	return string("");
}

string ContactEntry::getUri(){
	return uri;
}

string ContactEntry::getDesc(){
	return desc;
}

uint32_t ContactEntry::getId(){
	return id;
}

void ContactEntry::setDesc( string d ){
	this->desc = d;
}

void ContactEntry::setUri( string u ){
	this->uri = u;
}


ContactDb::ContactDb(){
}

void ContactDb::addEntry( ContactEntry * entry ){
	entries.push_back( entry );
}

void ContactDb::delEntry( ContactEntry * entry ){
	list< ContactEntry * >::iterator i;

	for( i = entries.begin(); i != entries.end(); i++ ){
		if( (*i)->getId() == entry->getId() ){
			i = entries.erase( i );
		}
	}
}

ContactEntry * ContactDb::lookUp( string uri ){

	list<ContactEntry *>::iterator i;

        for( i = entries.begin(); i != entries.end(); i++ ){
                if( (*i)->getUri() == uri ){
                        return *i;
                }
        }

        return NULL;
}

ContactEntry * ContactDb::lookUp( uint32_t id ){

	list< ContactEntry * >::iterator i;

        for( i = entries.begin(); i != entries.end(); i++ ){
                if( (*i)->getId() == id ){
                        return *i;
                }
        }

        return NULL;
}


