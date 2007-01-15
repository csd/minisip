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

using namespace std;

AccountDialog::AccountDialog( Glib::RefPtr<Gnome::Glade::Xml>  refXml,
			      AccountsList * list ):
		list(list){

	refXml->get_widget( "accountDialog", dialogWindow );

	dialogWindow->set_size_request( -1, 300 );

	// Create the active widgets
	refXml->get_widget( "nameEntry", nameEntry );
	refXml->get_widget( "uriEntry", uriEntry );
	refXml->get_widget( "autodetectProxyCheck", autodetectProxyCheck );
	refXml->get_widget( "proxyLabel", proxyLabel );
	refXml->get_widget( "proxyEntry", proxyEntry );
	refXml->get_widget( "requiresAuthCheck", requiresAuthCheck );
	refXml->get_widget( "usernameEntry", usernameEntry );
	refXml->get_widget( "usernameLabel", usernameLabel );
	refXml->get_widget( "passwordEntry", passwordEntry );
	refXml->get_widget( "passwordLabel", passwordLabel );
	refXml->get_widget( "udpRadio", udpRadio );
	refXml->get_widget( "tcpRadio", tcpRadio );
	refXml->get_widget( "tlsRadio", tlsRadio );
	refXml->get_widget( "registerTimeSpin", registerTimeSpin );
	refXml->get_widget( "proxyPortSpin", proxyPortSpin );
	refXml->get_widget( "proxyPortLabel", proxyPortLabel );

	requiresAuthCheck->signal_toggled().connect(
		SLOT( *this, &AccountDialog::requiresAuthCheckChanged ) );
	autodetectProxyCheck->signal_toggled().connect(
		SLOT( *this, &AccountDialog::autodetectProxyCheckChanged ) );

	autodetectProxyCheckChanged();

	dialogWindow->show_all();
}

AccountDialog::~AccountDialog(){
}

void AccountDialog::addAccount(){
	if( dialogWindow->run() == Gtk::RESPONSE_OK ){
		Gtk::TreeModel::iterator iter = list->append();
		(*iter)[list->columns->name] = nameEntry->get_text();
		(*iter)[list->columns->uri] = uriEntry->get_text();
		
		if( autodetectProxyCheck->get_active() ){
			(*iter)[list->columns->autodetectSettings] = true;
			(*iter)[list->columns->proxy] = "";
			(*iter)[list->columns->port] = 5060;
		} else{
			(*iter)[list->columns->autodetectSettings] = false;
			(*iter)[list->columns->proxy] = proxyEntry->get_text();
			(*iter)[list->columns->port] = proxyPortSpin->get_value_as_int();
		}
		
		(*iter)[list->columns->defaultProxy] = false;
		(*iter)[list->columns->pstnProxy] = false;
		(*iter)[list->columns->username] = usernameEntry->get_text();
		(*iter)[list->columns->password] = passwordEntry->get_text();
		(*iter)[list->columns->registerExpires] = registerTimeSpin->get_value_as_int();
		if( tcpRadio->get_active() ){
			(*iter)[list->columns->transport] = "TCP";
		} else if( tlsRadio->get_active() ){
			(*iter)[list->columns->transport] = "TLS";
		} else{
			(*iter)[list->columns->transport] = "UDP";
		}
	}

	dialogWindow->hide();
}

void AccountDialog::editAccount( Gtk::TreeModel::iterator iter ){
	nameEntry->set_text( (*iter)[list->columns->name] );
	uriEntry->set_text( (*iter)[list->columns->uri] );
	
	autodetectProxyCheck->set_active(  (*iter)[list->columns->autodetectSettings] ); //(*iter)[list->columns->proxy] != "" );
	//note ... the values for proxy and port will show whatever we found ... even in autodetect mode
	proxyEntry->set_text( (*iter)[list->columns->proxy] );
	proxyPortSpin->set_value( (double)((*iter)[list->columns->port]) );
	
	requiresAuthCheck->set_active( (*iter)[list->columns->username] != "" );
	usernameEntry->set_text( (*iter)[list->columns->username] );
	passwordEntry->set_text( (*iter)[list->columns->password] );
	registerTimeSpin->set_value( (*iter)[list->columns->registerExpires] );
	
	if( (*iter)[list->columns->transport] == "TCP" ){
		tcpRadio->set_active( true );
	} else if( (*iter)[list->columns->transport] == "TLS" ){
		tlsRadio->set_active( true );
	} else{
		udpRadio->set_active( true );
	}
	if( dialogWindow->run() == Gtk::RESPONSE_OK ){
		(*iter)[list->columns->name] = nameEntry->get_text();
		(*iter)[list->columns->uri] = uriEntry->get_text();
		
		(*iter)[list->columns->autodetectSettings] = autodetectProxyCheck->get_active();
		if( autodetectProxyCheck->get_active() ){
			(*iter)[list->columns->proxy] = "";
			(*iter)[list->columns->port] = 5060;
		} else{
			(*iter)[list->columns->proxy] = proxyEntry->get_text();
			(*iter)[list->columns->port] = proxyPortSpin->get_value_as_int();
		}
		
		(*iter)[list->columns->defaultProxy] = false;
		(*iter)[list->columns->pstnProxy] = false;
		(*iter)[list->columns->username] = usernameEntry->get_text();
		(*iter)[list->columns->password] = passwordEntry->get_text();
		(*iter)[list->columns->registerExpires] = registerTimeSpin->get_value_as_int();
		
		if( tcpRadio->get_active() ){
			(*iter)[list->columns->transport] = "TCP";
		} else if( tlsRadio->get_active() ){
			(*iter)[list->columns->transport] = "TLS";
		}else{
			(*iter)[list->columns->transport] = "UDP";
		}
	}
	dialogWindow->hide();
}

void AccountDialog::requiresAuthCheckChanged(){
	bool setTo = requiresAuthCheck->get_active();
	usernameEntry->set_sensitive( setTo );
	usernameLabel->set_sensitive( setTo );
	passwordEntry->set_sensitive( setTo );
	passwordLabel->set_sensitive( setTo );
	if( !setTo ) {
		usernameEntry->set_text( "" );
		passwordEntry->set_text( "" );
	}
}
		
void AccountDialog::autodetectProxyCheckChanged(){
	bool setTo = autodetectProxyCheck->get_active();
	proxyEntry->set_sensitive( ! setTo  );
	proxyLabel->set_sensitive( ! setTo  );
	proxyPortSpin->set_sensitive( ! setTo  );
	proxyPortLabel->set_sensitive( ! setTo  );
}
