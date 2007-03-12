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

#include"ContactDialog.h"
#include<libminisip/contacts/ContactDb.h>
#include<libminisip/contacts/PhoneBook.h>

using namespace std;

ContactDialog::ContactDialog():Gtk::Dialog( "Contact information", false ){
	Gtk::VBox * vbox = get_vbox();//= new Gtk::VBox;
	Gtk::Table * table = new Gtk::Table( 3, 2, false );

	vbox->pack_start( *table, false, false );

	Gtk::Label * nameLabel = new Gtk::Label( "Name:" );
	table->attach( *nameLabel, 0, 1, 0, 1 );
	table->attach( *(contactNameEntry = new Gtk::Entry), 1, 2, 0, 1 );

	Gtk::Label * typeLabel = new Gtk::Label( "Type:" );
	table->attach( *typeLabel, 0, 1, 1, 2 );
	table->attach( *(contactTypeEntry = new Gtk::Entry), 1, 2, 1, 2 );

	Gtk::Label * uriLabel = new Gtk::Label( "URI:" );
	table->attach( *uriLabel, 0, 1, 2, 3 );
	table->attach( *(contactUriEntry = new Gtk::Entry), 1, 2, 2, 3 );

	add_button( Gtk::Stock::OK, Gtk::RESPONSE_OK );
	add_button( Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL );
	vbox->show_all();
	
}

void ContactDialog::edit( MRef<ContactEntry *> entry ){
	contactNameEntry->set_text( Glib::locale_to_utf8( entry->getName() ) );
	contactTypeEntry->set_text( Glib::locale_to_utf8( entry->getDesc() ) );
	contactUriEntry->set_text( Glib::locale_to_utf8( entry->getUri() ) );
	contactNameEntry->set_sensitive( false );
}

void ContactDialog::edit( MRef<PhoneBookPerson *> person ){
	contactNameEntry->set_text( Glib::locale_to_utf8( person->getName() ) );
	contactTypeEntry->set_sensitive( false );
	contactUriEntry->set_sensitive( false );
}

void ContactDialog::edit( MRef<PhoneBook *> phonebook ){
	contactNameEntry->set_text( Glib::locale_to_utf8( phonebook->getName() ) );
	contactTypeEntry->set_sensitive( false );
	contactUriEntry->set_sensitive( false );
}

void ContactDialog::addContact( MRef<PhoneBookPerson *> person ){
	if( ! person.isNull() ){
		contactNameEntry->set_text( Glib::locale_to_utf8( person->getName() ) );
		contactNameEntry->set_sensitive( false );
	}
}

Glib::ustring ContactDialog::getNameString(){
	return contactNameEntry->get_text();
}

Glib::ustring ContactDialog::getTypeString(){
	return contactTypeEntry->get_text();
}

Glib::ustring ContactDialog::getUriString(){
	return contactUriEntry->get_text();
}
