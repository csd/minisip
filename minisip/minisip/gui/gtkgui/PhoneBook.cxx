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
#include"PhoneBook.h"
#include"ContactDialog.h"
#include<libminisip/contacts/PhoneBook.h>
#include<libminisip/contacts/ContactDb.h>

using namespace std;

PhoneBookTree::PhoneBookTree(){
	add( name );
	add( uri );
	add( contactEntry );
	add( person );
	add( phonebook );

}

PhoneBookModel::PhoneBookModel( PhoneBookTree * tree ):
		Gtk::TreeStore( *tree ){
	
	this->tree = tree;

}

void PhoneBookModel::setPhoneBook( MRef<PhoneBook *> phonebook ){
	string phonebookName;
	
	if( !defaultPhonebook ){
		defaultPhonebook = phonebook;
	}
		
	phonebookName = phonebook->getName();
	
	list< MRef<PhoneBookPerson *> > persons = phonebook->getPersons();
	list< MRef<PhoneBookPerson *> >::iterator iPerson;

	string contact;

	for( iPerson = persons.begin(); iPerson != persons.end(); iPerson++ ){

		contact = (*iPerson)->getName();

		if( contact != "" ){
			list< MRef<ContactEntry *> > entries = (*iPerson)->getEntries();
			list< MRef<ContactEntry *> >::iterator  iEntry;

			Gtk::TreeModel::iterator contactparent = append( /*(*phonebookparent).*/children());
			(*contactparent)[tree->name] = Glib::locale_to_utf8( contact );
			(*contactparent)[tree->person] = *iPerson;
			(*contactparent)[tree->phonebook] = phonebook;
			(*contactparent)[tree->contactEntry] = NULL;

			for( iEntry = entries.begin(); iEntry != entries.end(); iEntry ++ ){

				string desc = (*iEntry)->getDesc();
				string uri  = (*iEntry)->getUri();

				Gtk::TreeModel::iterator item = append( (*contactparent).children() );
				(*item)[tree->name] = Glib::locale_to_utf8( desc + ": " + uri );
				(*item)[tree->uri] = Glib::locale_to_utf8( uri );
				(*item)[tree->contactEntry] = *iEntry;
				(*item)[tree->person] = *iPerson;
				(*item)[tree->phonebook] = phonebook;
			}
		}
	}
}

void PhoneBookModel::addContact( Glib::RefPtr<Gtk::TreeSelection> selection, 
			bool address ){
	if( address && selection->count_selected_rows() == 0 ){
		return;
	}

	
	ContactDialog dialog;
	MRef<ContactEntry *> entry;
	MRef<PhoneBookPerson *> person;
	MRef<PhoneBook *> phonebook;
	Gtk::TreeModel::iterator newItem;
	TreeModel::iterator i =  selection->get_selected();
	
	if( selection->count_selected_rows() == 0 ){
		phonebook = defaultPhonebook;
		if( !phonebook ){
			/* No phonebook, we can't do anything */
			return;
		}
		person = NULL;
	}
	else{
		person = (*i)[tree->person];
		phonebook = (*i)[tree->phonebook];
	}

	if( address ){
		dialog.addContact( (*i)[tree->person] );
	}
	else{
		dialog.addContact( NULL );
	}

	
	if( dialog.run() != Gtk::RESPONSE_OK || dialog.getNameString() == "" ){
		return;
	}

	/* If we are not at the root, we add a contact entry */
	if( address ){
		MRef<ContactEntry *> selectedEntry = (*i)[tree->contactEntry];

		/* If we have selected a person */
		if( selectedEntry.isNull() ){
			newItem = append( (*i)->children() );
		}
		else{ /* we have selected a contact entry */
			newItem = append( (*i)->parent()->children() );
		}
	}
	else{ /* We were at the root, we create a new person */
		Gtk::TreeModel::iterator newPersonItem;
		person = new PhoneBookPerson( 
			Glib::locale_from_utf8( dialog.getNameString() ) );
		phonebook->addPerson( person );

		/* Add the GUI line for that person */
		newPersonItem = append();

		(*newPersonItem)[tree->name] = dialog.getNameString();
		(*newPersonItem)[tree->uri] = "";
		(*newPersonItem)[tree->contactEntry] = NULL;
		(*newPersonItem)[tree->person] = person;
		(*newPersonItem)[tree->phonebook] = phonebook;

		/* Add the GUI contact line */
		newItem = append( (*newPersonItem)->children() );
	}
	
	entry = new ContactEntry( 
			Glib::locale_from_utf8( dialog.getUriString() ),
			Glib::locale_from_utf8( dialog.getTypeString()),
			person );
	
	(*newItem)[tree->name] = dialog.getTypeString() + ": " + dialog.getUriString();
	(*newItem)[tree->uri] = dialog.getUriString();
	(*newItem)[tree->person] = person;
	(*newItem)[tree->contactEntry] = entry;
	(*newItem)[tree->phonebook] = phonebook;

	person->addEntry( entry );
	
	phonebook->save();
	
}

void PhoneBookModel::removeContact( Glib::RefPtr<Gtk::TreeSelection> selection ){
	if( selection->count_selected_rows() == 0 ){
		return;
	}
	TreeModel::iterator i =  selection->get_selected();
	MRef<PhoneBookPerson *> person = (*i)[tree->person];
	MRef<ContactEntry *> entry;
	MRef<PhoneBook *> phonebook;
	if( person.isNull() ){
		/* Trying to delete a phonebook, we don't allow that yet */
		return;
	}

	entry = (*i)[tree->contactEntry];
	phonebook = (*i)[tree->phonebook];

	if( phonebook.isNull() ){
		return;
	}

	if( !entry.isNull() && (*i)->parent()->children().size() == 1 ){
		/* If only one contact, we erase the person */
		erase( (*i)->parent() );
		
	}
	else{
		erase( i );
	}
	
	if( entry.isNull() ){ /* Deleting a person */
		phonebook->delPerson( person );
	}
	else{
		person->delEntry( entry );
		
	}
	
	phonebook->save();
}

void PhoneBookModel::editContact( Glib::RefPtr<Gtk::TreeSelection> selection ){
	ContactDialog dialog;
	TreeModel::iterator i =  selection->get_selected();
	MRef<PhoneBookPerson *> person; 
	MRef<ContactEntry *> entry;
	MRef<PhoneBook *> phonebook;

	if( selection->count_selected_rows() == 0 ){
		return;
	}
	person = (*i)[tree->person];
	entry = (*i)[tree->contactEntry];
	phonebook = (*i)[tree->phonebook];

	if( person.isNull() ){ /* Editing a phonebook */
		dialog.edit( phonebook );
		
		if( dialog.run() == Gtk::RESPONSE_OK ){
			/* Update the GUI */
			(*i)[tree->name] = dialog.getNameString();
			/* Update the internal DB */
			phonebook->setName(
			  Glib::locale_from_utf8( dialog.getNameString() ) );
			/* Save */
			phonebook->save();
		}
	}
	else if( entry.isNull() ){
		/* Editing a name */
		dialog.edit( person );

		if( dialog.run() == Gtk::RESPONSE_OK ){
			/* Update the Gui */
			(*i)[tree->name] = dialog.getNameString();
			/* Update the internal DB */
			person->setName( 
			  Glib::locale_from_utf8( dialog.getNameString() ) );
			/* Save */
			phonebook->save();
		}

						 
	} 
	else{
		/* Editing a contact entry */
		dialog.edit( entry );

		if( dialog.run() == Gtk::RESPONSE_OK ){
			/* Update the Gui */
			(*i)[tree->name] = dialog.getTypeString() + ": " + dialog.getUriString();
			(*i)[tree->uri] = dialog.getUriString();
			/* Update the internal DB */
			entry->setDesc( Glib::locale_from_utf8( dialog.getTypeString() ) );
			entry->setUri( Glib::locale_from_utf8( (*i)[tree->uri] ) );
			/* Save */
			phonebook->save();


		}
	}

}

void PhoneBookModel::setFont( Gtk::CellRenderer * renderer, 
		const Gtk::TreeModel::iterator & iter ){

	Gtk::CellRendererText * textR = 
		(Gtk::CellRendererText *)renderer;

	if( !(*iter)->children().empty() ){
		/* Not a leaf, make it bold */
		textR->property_markup().set_value( "<b>" + 
				(*iter)[tree->name] + "</b>" );

		textR->property_is_expanded().set_value( 
				(*iter)->children().size() >= 1 );
#ifndef OLDLIBGLADEMM
                renderer->property_cell_background().set_value(
                                "#FFFFFF" );
#endif

	}
	else{
		MRef<ContactEntry *> entry = (*iter)[tree->contactEntry];
		MRef<PhoneBook *> pb = (*iter)[tree->phonebook];
		if( entry ){
			string col;
			if (entry->isOnline()){
				col = "#0000FF";
			}else if (entry->isOffline()){
				//col = "#FF2020";
				col = "#000000";
			}else{
				//col = "#0000FF";
				col = "#000000";
			}
			textR->property_markup().set_value( 
				entry->getDesc() + "\n     " + 
				"<span size=\"x-small\"foreground=\"" + 
				col +  "\">" + 
				entry->getUri() + "</span>" );
#ifndef OLDLIBGLADEMM
                        renderer->property_cell_background().set_value(
                                entry->getPersonIndex()%2?"#EFEFEF":"#FFFFFF" );
#endif

		}
		/*
		else{
			textR->property_markup().set_value( "<b><u>" + 
				pb->getName() + "</u></b>" );
		}
		*/

	}
}
		
	
