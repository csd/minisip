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
#include"AccountsList.h"
#include"AccountDialog.h"
#include"../../../sip/SipSoftPhoneConfiguration.h"
// #include<libmnetutil/NetworkException.h>

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

Glib::RefPtr<AccountsList> AccountsList::create( AccountsListColumns * columns ){
	return Glib::RefPtr<AccountsList>::RefPtr<AccountsList>( 
			new AccountsList( columns ) );
}
	
AccountsList::AccountsList( AccountsListColumns * columns ):
	Gtk::ListStore( *columns ),
	columns( columns ){
	

}

void AccountsList::loadFromConfig( MRef<SipSoftPhoneConfiguration *> config ){
	list< MRef<SipIdentity *> > identities = config->identities;
	list< MRef<SipIdentity *> >::iterator i;

	for( i = identities.begin(); i != identities.end(); i++ ){
		(*i)->lock();
		Gtk::TreeModel::iterator iter = append();
		(*iter)[columns->identity] = (*i);
		(*iter)[columns->name] = (*i)->identityIdentifier;
		(*iter)[columns->uri] = (*i)->sipUsername + "@" + (*i)->sipDomain;
		(*iter)[columns->autodetectSettings] = (*i)->getSipProxy()->autodetectSettings;
		(*iter)[columns->proxy] = (*i)->getSipProxy()->sipProxyAddressString;
		(*iter)[columns->port] = (*i)->getSipProxy()->sipProxyPort;
		(*iter)[columns->transport] = (*i)->getSipProxy()->getTransport();
		(*iter)[columns->doRegister] = (*i)->registerToProxy;
		(*iter)[columns->username] = (*i)->getSipProxy()->sipProxyUsername;
		(*iter)[columns->password] = (*i)->getSipProxy()->sipProxyPassword;
		(*iter)[columns->registerExpires] = (*i)->getSipProxy()->getDefaultExpires_int();
		(*iter)[columns->defaultProxy] = ( (*i) == config->inherited->sipIdentity );
		(*iter)[columns->pstnProxy] = ( (*i) == config->pstnIdentity );
		(*i)->unlock();
	}

}

string AccountsList::saveToConfig( MRef<SipSoftPhoneConfiguration *> config ){
	config->identities.clear();
	config->inherited->sipIdentity = NULL;
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
			merr << retProxy << end; 
		}

		identity->getSipProxy()->sipProxyUsername = 
			Glib::locale_from_utf8( (*iter)[columns->username] );
		
		identity->getSipProxy()->sipProxyPassword = 
			Glib::locale_from_utf8( (*iter)[columns->password] );

		if( (*iter)[columns->pstnProxy] ){
			identity->securitySupport = false;
			config->usePSTNProxy = true;
			config->pstnIdentity = identity;
		}

		if( !config->inherited->sipIdentity ||
				(*iter)[columns->defaultProxy] ){
			config->inherited->sipIdentity = identity;
		}

		identity->getSipProxy()->setDefaultExpires( (*iter)[columns->registerExpires] );

		config->identities.push_back( identity );
	}
	
	return err;
}

void AccountsList::addAccount(){
	AccountDialog dialog( this );
	dialog.addAccount();
}

void AccountsList::editAccount( Gtk::TreeModel::iterator iter ){
	AccountDialog dialog( this );
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
