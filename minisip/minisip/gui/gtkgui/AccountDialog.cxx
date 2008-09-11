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
#include"TransportList.h"

#include<libmcrypto/SipSimSoft.h>

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
			      AccountsList * list,
			      Glib::RefPtr<TransportList> transportList ):
		refXml( theRefXml ),
		certificateDialog( certDialog ),
		list(list),
		transportList( transportList ),
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
	refXml->get_widget( "registerCheck", registerCheck );
	refXml->get_widget( "registerTimeSpin", registerTimeSpin );
	refXml->get_widget( "proxyPortSpin", proxyPortSpin );
	refXml->get_widget( "proxyPortLabel", proxyPortLabel );
	refXml->get_widget( "proxyPortCheck", proxyPortCheck );
	refXml->get_widget( "proxyTransportList", proxyTransportView );
	refXml->get_widget( "proxyTransportCheck", proxyTransportCheck );

	if( proxyTransportView->get_model() != transportList ){
		proxyTransportView->set_model( transportList );
		TransportListColumns *columns = transportList->getColumns();

		Gtk::CellRendererText* crt;
		crt = new Gtk::CellRendererText();
		proxyTransportView->pack_end(*manage(crt), true);
		proxyTransportView->add_attribute(crt->property_text(), columns->name);
	}

	requiresAuthConn = requiresAuthCheck->signal_toggled().connect(
		SLOT( *this, &AccountDialog::requiresAuthCheckChanged ) );
	autodetectProxyConn = autodetectProxyCheck->signal_toggled().connect(
		SLOT( *this, &AccountDialog::autodetectProxyCheckChanged ) );
	proxyConn = proxyCheck->signal_toggled().connect(
		SLOT( *this, &AccountDialog::autodetectProxyCheckChanged ) );
	proxyPortConn = proxyPortCheck->signal_toggled().connect(
		SLOT( *this, &AccountDialog::autodetectProxyCheckChanged ) );
	proxyTransportConn = proxyTransportCheck->signal_toggled().connect(
		SLOT( *this, &AccountDialog::autodetectProxyCheckChanged ) );
	
	securitySettings = new SecuritySettings( refXml, certDialog );

	passwordEntry->set_visibility(FALSE); //hide password phrase under *

	autodetectProxyCheckChanged();

	dialogWindow->show_all();
}

AccountDialog::~AccountDialog(){
	requiresAuthConn.disconnect();
	autodetectProxyConn.disconnect();
	proxyConn.disconnect();
	proxyPortConn.disconnect();
	proxyTransportConn.disconnect();

	delete securitySettings;
	securitySettings = NULL;
}

void AccountDialog::addAccount(){
	reset();
	MRef<SipIdentity*> identity = new SipIdentity;
	MRef<CertificateSet*> ca = CertificateSet::create();
	MRef<CertificateChain*> cert = CertificateChain::create();
	MRef<SipSim*> sim =
		new SipSimSoft( cert, ca );
	identity->setSim( sim );
	securitySettings->setConfig( identity );

	if( dialogWindow->run() == Gtk::RESPONSE_OK ){
		Gtk::TreeModel::iterator iter = list->append();
		(*iter)[list->columns->defaultProxy] = false;
		(*iter)[list->columns->pstnProxy] = false;
		(*iter)[list->columns->identity] = identity;
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
	proxyTransportCheck->set_active( true );

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

		if( proxyTransportCheck->get_active() ){
			(*iter)[list->columns->transport] = "";
		}
		else{
			Gtk::TreeModel::iterator listIter = proxyTransportView->get_active();
			if( listIter ){
				Gtk::TreeModel::Row row = *listIter;
				TransportListColumns *columns =
					transportList->getColumns();

				MRef<SipTransportConfig*> transportConfig =
					(*listIter)[columns->config];

				(*iter)[list->columns->transport] =
					transportConfig->getName();
			}
			else{
				(*iter)[list->columns->transport] = "";
			}
		}

		(*iter)[list->columns->username] = usernameEntry->get_text();
		(*iter)[list->columns->password] = passwordEntry->get_text();
		(*iter)[list->columns->doRegister] = registerCheck->get_active();
		(*iter)[list->columns->registerExpires] = registerTimeSpin->get_value_as_int();

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

	Gtk::TreeModel::iterator transportIter;
	for( transportIter = transportList->children().begin();
	     transportIter != transportList->children().end();
	     transportIter++ ){
		MRef<SipTransportConfig*> transportConfig =
			(*transportIter)[transportList->getColumns()->config];

		if( (*iter)[list->columns->transport] == transportConfig->getName() ){
			proxyTransportView->set_active(*transportIter);
		}
	}

	Glib::ustring transport = (*iter)[list->columns->transport];
	proxyTransportCheck->set_active( transport == "" );

	requiresAuthCheck->set_active( (*iter)[list->columns->username] != "" );
	usernameEntry->set_text( (*iter)[list->columns->username] );
	passwordEntry->set_text( (*iter)[list->columns->password] );
	registerCheck->set_active( (*iter)[list->columns->doRegister] );
	registerTimeSpin->set_value( (*iter)[list->columns->registerExpires] );
	
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
	proxyTransportCheck->set_sensitive( proxyEnabled );

	bool setTo = !proxyEnabled || autodetectProxyCheck->get_active();
	proxyEntry->set_sensitive( ! setTo  );
// 	proxyPortLabel->set_sensitive( ! setTo  );
 	proxyPortCheck->set_sensitive( ! setTo  );

	bool autoPort = setTo || proxyPortCheck->get_active();
	proxyPortSpin->set_sensitive( ! autoPort  );

	bool autoTransport = !proxyEnabled || proxyTransportCheck->get_active();
	proxyTransportView->set_sensitive( ! autoTransport );

}
