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

#ifndef GTK_PHONE_BOOK_H
#define GTK_PHONE_BOOK_H

#include<gtkmm.h>
#include<libmutil/MemObject.h>

class PhoneBookPerson;
class PhoneBook;
class ContactEntry;

class PhoneBookTree : public Gtk::TreeModel::ColumnRecord{
	
	public:
		PhoneBookTree();
	
		Gtk::TreeModelColumn<Glib::ustring> name;
		Gtk::TreeModelColumn<Glib::ustring> uri;
		Gtk::TreeModelColumn< MRef<ContactEntry *> > contactEntry;
		Gtk::TreeModelColumn< MRef<PhoneBookPerson *> > person;
		Gtk::TreeModelColumn< MRef<PhoneBook *> > phonebook;
	private:
		
};

class PhoneBookModel : public Gtk::TreeStore{
	
	public:
		PhoneBookModel( PhoneBookTree * tree );
		void setPhoneBook( MRef<PhoneBook *> phonebook );

		PhoneBookTree * tree;

		void removeContact( Glib::RefPtr<Gtk::TreeSelection> selection );
		void editContact( Glib::RefPtr<Gtk::TreeSelection> selection );
		void addContact( Glib::RefPtr<Gtk::TreeSelection> selection,
				 bool address );
		
		void setFont( Gtk::CellRenderer * renderer, const Gtk::TreeModel::iterator & iter );

	private:
		MRef<PhoneBook *> defaultPhonebook;
		

};







#endif
