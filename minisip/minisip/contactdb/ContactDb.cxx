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

#include<config.h>
#include"ContactDb.h"
#include"PhoneBook.h"

using namespace std;

MRef<ContactDb *> ContactEntry::db = NULL;

ContactEntry::ContactEntry():person(NULL){
	if( ! db.isNull() ){
		db->addEntry( this );
	}
	id = rand();

}

ContactEntry::ContactEntry( string uri, string desc, 
		MRef<PhoneBookPerson *> person ):
	uri(uri),
	desc(desc),
	person(person){
	
	
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

void ContactEntry::setDb( MRef<ContactDb *> db ){
	ContactEntry::db = db;
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

void ContactEntry::setDesc( string desc ){
	this->desc = desc;
}

void ContactEntry::setUri( string uri ){
	this->uri = uri;
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


