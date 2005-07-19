/*
 *  This program is free software; you can redistribute it and/or m!!!!!odify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have re!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!l,,,,,,,,;;ceived a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/* Copyright (C) 2004, 2005
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#ifndef ACCOUNTS_STATUS_WIDGET_H
#define ACCOUNTS_STATUS_WIDGET_H

#include<gtkmm.h>
#include<AccountsList.h>


class AccountsStatusWidget: public Gtk::TreeView{
	public:
		AccountsStatusWidget( Glib::RefPtr<AccountsList> list );
	private:

		Gtk::TreeModelColumn<Glib::ustring> nameColumn;
		Gtk::TreeModelColumn<Glib::ustring> statusColumn;

		AccountsListColumns * columns;

		void drawAccount( Gtk::CellRenderer * renderer,
				const Gtk::TreeModel::iterator & iter );
		void drawStatus( Gtk::CellRenderer * renderer,
				const Gtk::TreeModel::iterator & iter );

};

#endif
