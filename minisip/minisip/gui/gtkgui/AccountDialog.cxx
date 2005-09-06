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

#define ADD_FIRST_COLUMN( w ) table->attach( *w, 0, 1, row, row+1 );
#define ADD_SECOND_COLUMN( w ) table->attach( *w, 1, 2, row, row+1 );row++;
#define ADD( w ) table->attach( *w, 0, 2, row, row+1 );row++;


AccountDialog::AccountDialog( AccountsList * list ):Gtk::Dialog( "Sip account settings", false ){
#ifdef HILDON_SUPPORT
	// FIXME
	set_size_request( -1, 400 );
#else
	set_size_request( -1, 600 );
#endif

	// Create the active widgets
	nameEntry = manage( new Gtk::Entry );
	uriEntry = manage( new Gtk::Entry );
	autodetectProxyCheck = manage( new Gtk::CheckButton( "Manually specify SIP proxy:" ) );
	proxyEntry = manage( new Gtk::Entry );
	requiresAuthCheck = manage( new Gtk::CheckButton( "Requires authentication" ) );
	usernameEntry = manage( new Gtk::Entry );
	passwordEntry = manage( new Gtk::Entry );
	udpRadio = manage( new Gtk::RadioButton( "UDP" ) );
	Gtk::RadioButton::Group group = udpRadio->get_group();
	tcpRadio = manage( new Gtk::RadioButton( group, "TCP" ) );
	tlsRadio = manage( new Gtk::RadioButton( group, "TLS" ) );
	
	registerTimeSpin = manage( new Gtk::SpinButton() );
	proxyPortSpin = manage( new Gtk::SpinButton() );

	proxyPortSpin->set_range( 1, 65535 );
	proxyPortSpin->set_increments( 1, 10 );
	proxyPortSpin->set_value( 5060 );
	
	registerTimeSpin->set_range( 1, 36000 );
	registerTimeSpin->set_increments( 1, 10 );
	registerTimeSpin->set_value( 1000 );
	


	uint8_t row = 0;
	this->list = list;
	Gtk::VBox * vbox = get_vbox();
	Gtk::ScrolledWindow * scrolledWindow = 
		manage( new Gtk::ScrolledWindow() );
	scrolledWindow->set_policy( Gtk::POLICY_NEVER, Gtk::POLICY_AUTOMATIC );
	scrolledWindow->set_shadow_type( Gtk::SHADOW_NONE );
	vbox->pack_start( *scrolledWindow, true, true );
	vbox->set_spacing( 12 );
	
	Gtk::HBox * hbox = manage( new Gtk::HBox( false, 12 ) );
	scrolledWindow->add( *hbox );

	// Access the viewport
	dynamic_cast<Gtk::Viewport *>( hbox->get_parent() )
		->set_shadow_type( Gtk::SHADOW_NONE );
	
	Gtk::Table * table = manage( new Gtk::Table( 10, 2, false ) );
	table->set_col_spacings( 6 );
	table->set_row_spacings( 6 );
	table->set_homogeneous( true );
	Gtk::Label * label;

	label = manage( new Gtk::Label( "" ) );
	hbox->pack_start( *label, false, false );

	vbox = manage( new Gtk::VBox( false, 6 ) );
	hbox->pack_start( *vbox, true, true );
	
	label = manage( new Gtk::Label( "" ) );
	hbox->pack_start( *label, false, false );

	label = manage( new Gtk::Label( "<b><big>General</big></b>" , 0.0, 0.0 ) );
	label->set_use_markup( true );
	label->set_justify( Gtk::JUSTIFY_LEFT );

//	vbox->pack_start( *label, false, false, 12 );

	vbox->pack_start( *table, true, true, 12 );

	ADD( label )

	label = manage(  new Gtk::Label( "Account name:", 0.0, 0.0 ) );
	label->set_justify( Gtk::JUSTIFY_LEFT );
	label->set_padding( 12, 0 );
	ADD_FIRST_COLUMN( label )
	ADD_SECOND_COLUMN( nameEntry );

	label = manage( new Gtk::Label( "SIP URI:", 0.0, 0.0 ) );
	label->set_justify( Gtk::JUSTIFY_LEFT );
	label->set_padding( 12, 0 );
	ADD_FIRST_COLUMN( label )
	ADD_SECOND_COLUMN( uriEntry )
	
	label = manage( new Gtk::Label( "<b><big>SIP proxy</big></b>" , 0.0, 0.0 ) );
	label->set_use_markup( true );
	label->set_justify( Gtk::JUSTIFY_LEFT );
	ADD( label )
	
	hbox = manage( new Gtk::HBox( false, 12 ) );
	label = manage( new Gtk::Label( "" ) );
	hbox->pack_start( *label, false, false );
	hbox->pack_start( *autodetectProxyCheck, false, false );
	ADD( hbox )
	
	proxyLabel = manage( new Gtk::Label( "SIP proxy:", 0.0, 0.0 ) );
	proxyLabel->set_justify( Gtk::JUSTIFY_LEFT );
	proxyLabel->set_padding( 24, 0 );
	
	ADD_FIRST_COLUMN( proxyLabel )
	ADD_SECOND_COLUMN( proxyEntry )
	
	proxyPortLabel = manage( new Gtk::Label( "Network port:", 0.0, 0.0 ) );
	proxyPortLabel->set_justify( Gtk::JUSTIFY_LEFT );
	proxyPortLabel->set_padding( 24, 0 );
	
	ADD_FIRST_COLUMN( proxyPortLabel )
	
	Gtk::Alignment * alignment = 
		manage( new Gtk::Alignment( 1.0 ) );
	alignment->add( *proxyPortSpin );
	table->attach( *alignment, 1, 2, row, row+1, Gtk::FILL );row++;
//	ADD_SECOND_COLUMN( proxyPortSpin )
	
	label = manage( new Gtk::Label( "Transport method:", 0.0, 0.0 ) );
	label->set_justify( Gtk::JUSTIFY_LEFT );
	label->set_padding( 12, 0 );
	
	ADD_FIRST_COLUMN( label )
	ADD_SECOND_COLUMN( udpRadio )
	ADD_SECOND_COLUMN( tcpRadio )
	ADD_SECOND_COLUMN( tlsRadio )

	

	label = manage( new Gtk::Label( "<b><big>Registration</big></b>" , 0.0, 0.0 ) );
	label->set_use_markup( true );
	label->set_justify( Gtk::JUSTIFY_LEFT );
	ADD( label )

	hbox = manage( new Gtk::HBox( false, 12 ) );
	label = manage( new Gtk::Label( "" ) );
	hbox->pack_start( *label, false, false );
	hbox->pack_start( *requiresAuthCheck, false, false );

	ADD( hbox )

	
	usernameLabel = manage( new Gtk::Label( "Username:", 0.0, 0.0 ) );
	usernameLabel->set_justify( Gtk::JUSTIFY_LEFT );
	usernameLabel->set_padding( 24, 0 );
	
	ADD_FIRST_COLUMN( usernameLabel )
	ADD_SECOND_COLUMN( usernameEntry )
	
	passwordLabel = manage( new Gtk::Label( "Password:", 0.0, 0.0 ) );
	passwordLabel->set_justify( Gtk::JUSTIFY_LEFT );
	passwordLabel->set_padding( 24, 0 );
	
	ADD_FIRST_COLUMN( passwordLabel )
	ADD_SECOND_COLUMN( passwordEntry )


	requiresAuthCheck->signal_toggled().connect( SLOT(
		*this, &AccountDialog::requiresAuthCheckChanged ) );
	autodetectProxyCheck->signal_toggled().connect( SLOT(
		*this, &AccountDialog::autodetectProxyCheckChanged ) );
	
	autodetectProxyCheck->set_active( true );

	label = manage( new Gtk::Label( "Registration time (in s):", 0.0, 0.0 ) );
	label->set_justify( Gtk::JUSTIFY_LEFT );
	label->set_padding( 12, 0 );

	ADD_FIRST_COLUMN( label )

	ADD_SECOND_COLUMN( registerTimeSpin )
	

	requiresAuthCheckChanged();
	autodetectProxyCheckChanged();
	
	add_button( Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL );
	add_button( Gtk::Stock::OK, Gtk::RESPONSE_OK );

	show_all();
}

AccountDialog::~AccountDialog(){
}

void AccountDialog::addAccount(){
	if( run() == Gtk::RESPONSE_OK ){
		Gtk::TreeModel::iterator iter = list->append();
		(*iter)[list->columns->name] = nameEntry->get_text();
		(*iter)[list->columns->uri] = uriEntry->get_text();
		if( !autodetectProxyCheck->get_active() ){
			(*iter)[list->columns->proxy] = "";
		}
		else{
			(*iter)[list->columns->proxy] = proxyEntry->get_text();
		}
		(*iter)[list->columns->defaultProxy] = false;
		(*iter)[list->columns->pstnProxy] = false;
		(*iter)[list->columns->username] = usernameEntry->get_text();
		(*iter)[list->columns->password] = passwordEntry->get_text();
		(*iter)[list->columns->port] = proxyPortSpin->get_value_as_int();
		(*iter)[list->columns->registerExpires] = registerTimeSpin->get_value_as_int();
		if( tcpRadio->get_active() ){
			(*iter)[list->columns->transport] = "TCP";
		}
		else if( tlsRadio->get_active() ){
			(*iter)[list->columns->transport] = "TLS";
		}
		else{
			(*iter)[list->columns->transport] = "UDP";
		}
	}
}

void AccountDialog::editAccount( Gtk::TreeModel::iterator iter ){
	nameEntry->set_text( (*iter)[list->columns->name] );
	uriEntry->set_text( (*iter)[list->columns->uri] );
	autodetectProxyCheck->set_active(  (*iter)[list->columns->proxy] != "" );
	proxyEntry->set_text( (*iter)[list->columns->proxy] );
	requiresAuthCheck->set_active( (*iter)[list->columns->username] != "" );
	usernameEntry->set_text( (*iter)[list->columns->username] );
	passwordEntry->set_text( (*iter)[list->columns->password] );
	proxyPortSpin->set_value( (double)((*iter)[list->columns->port]) );
	registerTimeSpin->set_value( (*iter)[list->columns->registerExpires] );
	if( (*iter)[list->columns->transport] == "TCP" ){
		tcpRadio->set_active( true );
	}
	else if( (*iter)[list->columns->transport] == "TLS" ){
		tlsRadio->set_active( true );
	}
	else{
		udpRadio->set_active( true );
	}
	
	if( run() == Gtk::RESPONSE_OK ){
		(*iter)[list->columns->name] = nameEntry->get_text();
		(*iter)[list->columns->uri] = uriEntry->get_text();
		if( !autodetectProxyCheck->get_active() ){
			(*iter)[list->columns->proxy] = "";
		}
		else{
			(*iter)[list->columns->proxy] = proxyEntry->get_text();
		}
		(*iter)[list->columns->defaultProxy] = false;
		(*iter)[list->columns->pstnProxy] = false;
		(*iter)[list->columns->username] = usernameEntry->get_text();
		(*iter)[list->columns->password] = passwordEntry->get_text();
		(*iter)[list->columns->port] = proxyPortSpin->get_value_as_int();
		(*iter)[list->columns->registerExpires] = registerTimeSpin->get_value_as_int();
		if( tcpRadio->get_active() ){
			(*iter)[list->columns->transport] = "TCP";
		}
		else if( tlsRadio->get_active() ){
			(*iter)[list->columns->transport] = "TLS";
		}
		else{
			(*iter)[list->columns->transport] = "UDP";
		}


	}
}
		

void AccountDialog::requiresAuthCheckChanged(){
	usernameEntry->set_sensitive( requiresAuthCheck->get_active() );
	usernameLabel->set_sensitive( requiresAuthCheck->get_active() );
	passwordEntry->set_sensitive( requiresAuthCheck->get_active() );
	passwordLabel->set_sensitive( requiresAuthCheck->get_active() );
	if( !requiresAuthCheck->get_active() ){
		usernameEntry->set_text( "" );
		passwordEntry->set_text( "" );
	}
}
		
void AccountDialog::autodetectProxyCheckChanged(){
	proxyEntry->set_sensitive( autodetectProxyCheck->get_active() );
	proxyLabel->set_sensitive( autodetectProxyCheck->get_active() );
	proxyPortSpin->set_sensitive( autodetectProxyCheck->get_active() );
	proxyPortLabel->set_sensitive( autodetectProxyCheck->get_active() );
}
