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

/* Copyright (C) 2004, 2005 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#include<config.h>
#include"AccountDialog.h"
#include"AccountsList.h"

#ifdef OLDLIBGLADEMM
#define SLOT(a,b) SigC::slot(a,b)
#define BIND SigC::bind
#else
#define SLOT(a,b) sigc::mem_fun(a,b)
#define BIND sigc::bind
#endif


AccountDialog::AccountDialog( AccountsList * list ):Gtk::Dialog( "Sip account settings", false ){
	this->list = list;
	Gtk::VBox * vbox = get_vbox();
	Gtk::Table * table = new Gtk::Table( 7, 2, false );

	vbox->pack_start( *table, false, false );

	Gtk::Label * nameLabel = new Gtk::Label( "Account name:" );
	table->attach( *nameLabel, 0, 1, 0, 1 );
	table->attach( *(nameEntry = new Gtk::Entry), 1, 2, 0, 1 );

	Gtk::Label * uriLabel = new Gtk::Label( "SIP URI:" );
	table->attach( *uriLabel, 0, 1, 1, 2 );
	table->attach( *(uriEntry = new Gtk::Entry), 1, 2, 1, 2 ); 
	
	table->attach( *(autodetectProxyCheck = new Gtk::CheckButton( "Autodetect proxy" ) ),
			0, 2, 2, 3 );

	
	Gtk::Label * proxyLabel = new Gtk::Label( "SIP proxy:" );
	table->attach( *proxyLabel, 0, 1, 3, 4 );
	table->attach( *(proxyEntry = new Gtk::Entry), 1, 2, 3, 4 ); 

	table->attach( *(requiresAuthCheck = 
			new Gtk::CheckButton( "Requires authentication" ) ),
			0, 2, 5, 6 );
	
	Gtk::Label * usernameLabel = new Gtk::Label( "Username:" );
	table->attach( *usernameLabel, 0, 1, 6, 7 );
	table->attach( *(usernameEntry = new Gtk::Entry), 1, 2, 6, 7 );
	
	Gtk::Label * passwordLabel = new Gtk::Label( "Password:" );
	table->attach( *passwordLabel, 0, 1, 7, 8 );
	table->attach( *(passwordEntry = new Gtk::Entry), 1, 2, 7, 8 );

	add_button( Gtk::Stock::OK, Gtk::RESPONSE_OK );
	add_button( Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL );

	requiresAuthCheck->signal_toggled().connect( SLOT(
		*this, &AccountDialog::requiresAuthCheckChanged ) );
	autodetectProxyCheck->signal_toggled().connect( SLOT(
		*this, &AccountDialog::autodetectProxyCheckChanged ) );
	
	autodetectProxyCheck->set_active( true );

	requiresAuthCheckChanged();
	autodetectProxyCheckChanged();

	show_all();
}

void AccountDialog::addAccount(){
	if( run() == Gtk::RESPONSE_OK ){
		Gtk::TreeModel::iterator iter = list->append();
		(*iter)[list->columns->name] = nameEntry->get_text();
		(*iter)[list->columns->uri] = uriEntry->get_text();
		(*iter)[list->columns->proxy] = proxyEntry->get_text();
		(*iter)[list->columns->defaultProxy] = false;
		(*iter)[list->columns->pstnProxy] = false;
		(*iter)[list->columns->username] = usernameEntry->get_text();
		(*iter)[list->columns->password] = passwordEntry->get_text();
	}
}

void AccountDialog::editAccount( Gtk::TreeModel::iterator iter ){
	nameEntry->set_text( (*iter)[list->columns->name] );
	uriEntry->set_text( (*iter)[list->columns->uri] );
	autodetectProxyCheck->set_active(  (*iter)[list->columns->proxy] == "" );
	proxyEntry->set_text( (*iter)[list->columns->proxy] );
	requiresAuthCheck->set_active( (*iter)[list->columns->username] != "" );
	usernameEntry->set_text( (*iter)[list->columns->username] );
	passwordEntry->set_text( (*iter)[list->columns->password] );
	
	if( run() == Gtk::RESPONSE_OK ){
		(*iter)[list->columns->name] = nameEntry->get_text();
		(*iter)[list->columns->uri] = uriEntry->get_text();
		(*iter)[list->columns->proxy] = proxyEntry->get_text();
		(*iter)[list->columns->defaultProxy] = false;
		(*iter)[list->columns->pstnProxy] = false;
		(*iter)[list->columns->username] = usernameEntry->get_text();
		(*iter)[list->columns->password] = passwordEntry->get_text();
	}
}
		

void AccountDialog::requiresAuthCheckChanged(){
	usernameEntry->set_sensitive( requiresAuthCheck->get_active() );
	passwordEntry->set_sensitive( requiresAuthCheck->get_active() );
	if( !requiresAuthCheck->get_active() ){
		usernameEntry->set_text( "" );
		passwordEntry->set_text( "" );
	}
}
		
void AccountDialog::autodetectProxyCheckChanged(){
	proxyEntry->set_sensitive( !autodetectProxyCheck->get_active() );
	if( autodetectProxyCheck->get_active() ){
		proxyEntry->set_text( "" );
	}
}
