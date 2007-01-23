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

/* Copyright (C) 2004-2007
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
 *          Mikael Magnusson <mikma@users.sourceforge.net>
*/

#include<config.h>

#include"AccountDialog.h"
#include"AccountsList.h"
#include"CertificateDialog.h"
#include"SettingsDialog.h"

#ifdef OLDLIBGLADEMM
#define SLOT(a,b) SigC::slot(a,b)
#define BIND SigC::bind
#else
#define SLOT(a,b) sigc::mem_fun(a,b)
#define BIND sigc::bind
#endif

#ifdef HILDON_SUPPORT
const int DEFAULT_HEIGHT = 300;
#else
const int DEFAULT_HEIGHT = 425;
#endif

using namespace std;

AccountDialog::AccountDialog( Glib::RefPtr<Gnome::Glade::Xml>  theRefXml,
			      CertificateDialog * certDialog,
			      AccountsList * list ):
		refXml( theRefXml ),
		certificateDialog( certDialog ),
		list(list),
		securitySettings(NULL)
{
	refXml->get_widget( "accountDialog", dialogWindow );

	dialogWindow->set_size_request( -1, DEFAULT_HEIGHT );

	// Create the active widgets
	refXml->get_widget( "nameEntry", nameEntry );
	refXml->get_widget( "uriEntry", uriEntry );
	refXml->get_widget( "autodetectProxyCheck", autodetectProxyCheck );
	refXml->get_widget( "proxyCheck", proxyCheck );
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
	refXml->get_widget( "registerCheck", registerCheck );
	refXml->get_widget( "registerTimeSpin", registerTimeSpin );
	refXml->get_widget( "proxyPortSpin", proxyPortSpin );
	refXml->get_widget( "proxyPortLabel", proxyPortLabel );
	refXml->get_widget( "proxyPortCheck", proxyPortCheck );

	requiresAuthConn = requiresAuthCheck->signal_toggled().connect(
		SLOT( *this, &AccountDialog::requiresAuthCheckChanged ) );
	autodetectProxyConn = autodetectProxyCheck->signal_toggled().connect(
		SLOT( *this, &AccountDialog::autodetectProxyCheckChanged ) );
	proxyConn = proxyCheck->signal_toggled().connect(
		SLOT( *this, &AccountDialog::autodetectProxyCheckChanged ) );
	proxyPortConn = proxyPortCheck->signal_toggled().connect(
		SLOT( *this, &AccountDialog::autodetectProxyCheckChanged ) );
	
	securitySettings = new SecuritySettings( refXml, certDialog );

	autodetectProxyCheckChanged();

	dialogWindow->show_all();
}

AccountDialog::~AccountDialog(){
	requiresAuthConn.disconnect();
	autodetectProxyConn.disconnect();
	proxyConn.disconnect();
	proxyPortConn.disconnect();

	delete securitySettings;
	securitySettings = NULL;
}

void AccountDialog::addAccount(){
	reset();
	if( dialogWindow->run() == Gtk::RESPONSE_OK ){
		Gtk::TreeModel::iterator iter = list->append();
		apply( iter );
	}
	dialogWindow->hide();
}

void AccountDialog::reset(){
	nameEntry->set_text( "" );
	uriEntry->set_text( "" );
	
	proxyCheck->set_active( false );
	autodetectProxyCheck->set_active( false );
	proxyEntry->set_text( "" );
	proxyPortSpin->set_value( 5060 );
	proxyPortCheck->set_active( true );
	udpRadio->set_active( true );

	requiresAuthCheck->set_active( true );
	usernameEntry->set_text( "" );
	passwordEntry->set_text( "" );
	registerCheck->set_active( false );
	registerTimeSpin->set_value( 1000 );

	securitySettings->reset();
}

void AccountDialog::apply( Gtk::TreeModel::iterator iter ){
		(*iter)[list->columns->name] = nameEntry->get_text();
		(*iter)[list->columns->uri] = uriEntry->get_text();
		
		if( !proxyCheck->get_active() ){
			// No proxy
			(*iter)[list->columns->autodetectSettings] = false;
			(*iter)[list->columns->proxy] = "";
			(*iter)[list->columns->port] = 0;
		}
		else if( autodetectProxyCheck->get_active() ){
			// No address and port
			(*iter)[list->columns->autodetectSettings] = true;
			(*iter)[list->columns->proxy] = "";
			(*iter)[list->columns->port] = 0;
		}
		else if( proxyPortCheck->get_active() ){
			// No port
			(*iter)[list->columns->autodetectSettings] = false;
			(*iter)[list->columns->proxy] = proxyEntry->get_text();
			(*iter)[list->columns->port] = 0;
		} else{
			// Address and port
			(*iter)[list->columns->autodetectSettings] = false;
			(*iter)[list->columns->proxy] = proxyEntry->get_text();
			(*iter)[list->columns->port] = proxyPortSpin->get_value_as_int();
		}
		
		(*iter)[list->columns->defaultProxy] = false;
		(*iter)[list->columns->pstnProxy] = false;
		(*iter)[list->columns->username] = usernameEntry->get_text();
		(*iter)[list->columns->password] = passwordEntry->get_text();
		(*iter)[list->columns->doRegister] = registerCheck->get_active();
		(*iter)[list->columns->registerExpires] = registerTimeSpin->get_value_as_int();
		if( tcpRadio->get_active() ){
			(*iter)[list->columns->transport] = "TCP";
		} else if( tlsRadio->get_active() ){
			(*iter)[list->columns->transport] = "TLS";
		} else{
			(*iter)[list->columns->transport] = "UDP";
		}

		securitySettings->apply();
}

void AccountDialog::editAccount( Gtk::TreeModel::iterator iter ){
	nameEntry->set_text( (*iter)[list->columns->name] );
	uriEntry->set_text( (*iter)[list->columns->uri] );

	bool autodetect = (*iter)[list->columns->autodetectSettings];
	autodetectProxyCheck->set_active( autodetect );
	//note ... the values for proxy and port will show whatever we found ... even in autodetect mode

	Glib::ustring proxy = (*iter)[list->columns->proxy];
	proxyCheck->set_active( proxy != "" );
	proxyEntry->set_text( proxy );

	int port = (*iter)[list->columns->port];
	if( port > 0 ){
		proxyPortSpin->set_value( (double)(port) );
		proxyPortCheck->set_active( false );
	}
	else{
		proxyPortSpin->set_value( (double)(5060) );
		proxyPortCheck->set_active( true );
	}
	
	requiresAuthCheck->set_active( (*iter)[list->columns->username] != "" );
	usernameEntry->set_text( (*iter)[list->columns->username] );
	passwordEntry->set_text( (*iter)[list->columns->password] );
	registerCheck->set_active( (*iter)[list->columns->doRegister] );
	registerTimeSpin->set_value( (*iter)[list->columns->registerExpires] );
	
	if( (*iter)[list->columns->transport] == "TCP" ){
		tcpRadio->set_active( true );
	} else if( (*iter)[list->columns->transport] == "TLS" ){
		tlsRadio->set_active( true );
	} else{
		udpRadio->set_active( true );
	}

	MRef<SipIdentity*> identity = (*iter)[list->columns->identity]; 

	if( identity ){
		securitySettings->setConfig( identity );
	}

	if( dialogWindow->run() == Gtk::RESPONSE_OK ){
		apply( iter );
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
	bool proxyEnabled = proxyCheck->get_active();
// 	proxyLabel->set_sensitive( proxyEnabled );
	autodetectProxyCheck->set_sensitive( proxyEnabled );
	udpRadio->set_sensitive( proxyEnabled );
	tcpRadio->set_sensitive( proxyEnabled );
	tlsRadio->set_sensitive( proxyEnabled );

	bool setTo = !proxyEnabled || autodetectProxyCheck->get_active();
	proxyEntry->set_sensitive( ! setTo  );
// 	proxyPortLabel->set_sensitive( ! setTo  );
 	proxyPortCheck->set_sensitive( ! setTo  );

	bool autoPort = setTo || proxyPortCheck->get_active();
	proxyPortSpin->set_sensitive( ! autoPort  );
}
