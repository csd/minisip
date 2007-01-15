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

#ifndef ACCOUNT_DIALOG
#define ACCOUNT_DIALOG

#include<libglademm/xml.h>
#include<gtkmm.h>

class AccountsList;

class AccountDialog{
	public:
		AccountDialog( Glib::RefPtr<Gnome::Glade::Xml>  refXml,
			       AccountsList * list );
		~AccountDialog();

		void addAccount();
		void editAccount( Gtk::TreeModel::iterator iter );
	private:

		void requiresAuthCheckChanged();
		void autodetectProxyCheckChanged();

		Gtk::Dialog * dialogWindow;

		Gtk::Entry * nameEntry;
		Gtk::Entry * uriEntry;
		
		Gtk::CheckButton * autodetectProxyCheck;
		Gtk::Label * proxyLabel;
		Gtk::Entry * proxyEntry;
		Gtk::Label * proxyPortLabel;
		Gtk::SpinButton * proxyPortSpin;

		Gtk::RadioButton * udpRadio;
		Gtk::RadioButton * tcpRadio;
		Gtk::RadioButton * tlsRadio;

		Gtk::CheckButton * registerCheck;
		
		Gtk::CheckButton * requiresAuthCheck;
		Gtk::Entry * usernameEntry;
		Gtk::Label * usernameLabel;
		Gtk::Entry * passwordEntry;
		Gtk::Label * passwordLabel;

		Gtk::SpinButton * registerTimeSpin;
		
		AccountsList * list;

};




#endif
