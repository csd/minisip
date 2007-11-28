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

#include"AccountsList.h"
#include"AccountDialog.h"
#include"TransportList.h"

#include<libmsip/SipTransport.h>
#include<libminisip/signaling/sip/SipSoftPhoneConfiguration.h>

using namespace std;

AccountsListColumns::AccountsListColumns(){
	add( identity );
	add( name );
	add( uri );
	add( autodetectSettings );
	add( proxy );
	add( port );
	add( transport );
	add( doRegister );
	add( status );
	add( username );
	add( password );
	add( registerExpires );
	add( defaultProxy );
	add( pstnProxy );
}

Glib::RefPtr<AccountsList> AccountsList::create( Glib::RefPtr<Gnome::Glade::Xml>  refXml,
						 CertificateDialog * certDialog,
						 AccountsListColumns * columns,
						 Glib::RefPtr<TransportList> transportList ){
	return Glib::RefPtr<AccountsList>::RefPtr<AccountsList>( 
		new AccountsList( refXml, certDialog, columns, transportList ) );
}
	
AccountsList::AccountsList( Glib::RefPtr<Gnome::Glade::Xml>  refXml,
			    CertificateDialog * certDialog,
			    AccountsListColumns * columns,
			    Glib::RefPtr<TransportList> transportList ):
	Gtk::ListStore( *columns ),
	columns( columns ),
	refXml( refXml ),
	certDialog( certDialog ),
	transportList( transportList ){
	

}

void AccountsList::loadFromConfig( MRef<SipSoftPhoneConfiguration *> config ){
	list< MRef<SipIdentity *> > identities = config->identities;
	list< MRef<SipIdentity *> >::iterator i;

	for( i = identities.begin(); i != identities.end(); i++ ){
		(*i)->lock();
		Gtk::TreeModel::iterator iter = append();
		(*iter)[columns->identity] = (*i);
		(*iter)[columns->name] = (*i)->identityIdentifier;
		(*iter)[columns->uri] = (*i)->getSipUri().getRequestUriString();
		(*iter)[columns->autodetectSettings] = false;

		const list<SipUri> & routeSet = (*i)->getRouteSet();

		if( !routeSet.empty() ){
			SipUri proxyUri = *routeSet.begin();

			(*iter)[columns->proxy] = proxyUri.getIp();
			(*iter)[columns->port] = proxyUri.getPort();

			MRef<SipTransport*> transport =
				SipTransportRegistry::getInstance()->findTransport( proxyUri );

			if( transport )
				(*iter)[columns->transport] = transport->getName();
			else
				(*iter)[columns->transport] = "";
		}
		else {
			(*iter)[columns->proxy] = "";
			(*iter)[columns->port] = 0;
			(*iter)[columns->transport] = "";
		}

		(*iter)[columns->doRegister] = (*i)->registerToProxy;
		MRef<SipCredential*> cred = (*i)->getCredential();
		if( cred ){
			(*iter)[columns->username] = cred->getUsername();
			(*iter)[columns->password] = cred->getPassword();
		}
		else {
			(*iter)[columns->username] = "";
			(*iter)[columns->password] = "";
		}
		(*iter)[columns->registerExpires] = (*i)->getSipRegistrar()->getDefaultExpires_int();
		(*iter)[columns->defaultProxy] = ( (*i) == config->defaultIdentity );
		(*iter)[columns->pstnProxy] = ( (*i) == config->pstnIdentity );
		(*i)->unlock();
	}

}

string AccountsList::saveToConfig( MRef<SipSoftPhoneConfiguration *> config ){
	config->identities.clear();
	config->defaultIdentity = NULL;
	config->pstnIdentity = NULL;
	config->usePSTNProxy = false;
	Gtk::TreeModel::iterator iter;
	MRef<SipIdentity *> identity;
	string err = "";

	for( iter = children().begin(); iter != children().end(); iter ++ ){
		identity = new SipIdentity;
		
		string name = Glib::locale_from_utf8( (*iter)[columns->name] );
		identity->setIdentityName( name );
		
		string uri = Glib::locale_from_utf8( (*iter)[columns->uri] );
		identity->setSipUri( uri );
		
		string proxy = Glib::locale_from_utf8( (*iter)[columns->proxy] );
		
		identity->setDoRegister( (*iter)[columns->doRegister] );

		string retProxy = identity->setSipProxy( 
					(*iter)[columns->autodetectSettings], 
					uri,  
					Glib::locale_from_utf8( (*iter)[columns->transport] ), 
					proxy, 
					(*iter)[columns->port] );
		if( retProxy != "" ) {
			#ifdef DEBUG_OUTPUT
			cerr << "AccountList::saveToConfig:: Account <" << name << "> warning: " << retProxy << endl; 
			#endif
			merr << retProxy << endl; 
		}

		string username = 
			Glib::locale_from_utf8( (*iter)[columns->username] );
		string password =
			Glib::locale_from_utf8( (*iter)[columns->password] );
		MRef<SipCredential*> cred;

		if( username != "" ){
			cred = new SipCredential( username, password );
		}

		identity->setCredential( cred );

		if( (*iter)[columns->pstnProxy] ){
			identity->securityEnabled= false;
			config->usePSTNProxy = true;
			config->pstnIdentity = identity;
		}

		if( !config->defaultIdentity ||
				(*iter)[columns->defaultProxy] ){
			config->defaultIdentity = identity;
		}

		SipUri registrarUri( identity->getSipUri().getUserIpString() );
		registrarUri.setUser( "" );

		MRef<SipRegistrar*> registrar =
			new SipRegistrar( registrarUri );
		identity->setSipRegistrar( registrar );
		identity->getSipRegistrar()->setDefaultExpires( (*iter)[columns->registerExpires] );

		MRef<SipIdentity *> oldId = (*iter)[columns->identity];
		if( oldId ){
			// Copy PSK and SipSim from old identity
			identity->setPsk( oldId->getPsk() );
			identity->setSim( oldId->getSim() );

			identity->dhEnabled = oldId->dhEnabled;
			identity->checkCert = oldId->checkCert;
			identity->pskEnabled = oldId->pskEnabled;
			identity->ka_type = oldId->ka_type;
			identity->securityEnabled = oldId->securityEnabled;
		}

		config->identities.push_back( identity );
	}
	
	return err;
}

void AccountsList::addAccount(){
	AccountDialog dialog( refXml, certDialog, this, transportList );
	dialog.addAccount();
}

void AccountsList::editAccount( Gtk::TreeModel::iterator iter ){
	AccountDialog dialog( refXml, certDialog, this, transportList );
	dialog.editAccount( iter );
}

void AccountsList::setDefaultAccount( Gtk::TreeModel::iterator defaultIter ){
	Gtk::TreeModel::iterator iter;

	if( (*defaultIter)[columns->defaultProxy] ){
		return;
	}
	
	for( iter = children().begin(); iter != children().end(); iter ++ ){
		(*iter)[columns->defaultProxy] = false;
	}

	(*defaultIter)[columns->defaultProxy] = true;
	return;
}

void AccountsList::setPstnAccount( Gtk::TreeModel::iterator defaultIter ){
	Gtk::TreeModel::iterator iter;

	if( (*defaultIter)[columns->pstnProxy] ){
		(*defaultIter)[columns->pstnProxy] = false;
		return;
	}
	
	for( iter = children().begin(); iter != children().end(); iter ++ ){
		(*iter)[columns->pstnProxy] = false;
	}

	(*defaultIter)[columns->pstnProxy] = true;
	return;
}

AccountsListColumns * AccountsList::getColumns(){
	return columns;
}
