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
#include<libmnetutil/NetworkException.h>

using namespace std;

AccountsListColumns::AccountsListColumns(){
	add( name );
	add( uri );
	add( proxy );
	add( defaultProxy );
	add( pstnProxy );
	add( username );
	add( password );
	add( doRegister );
}

AccountsList::AccountsList( AccountsListColumns * columns ):
	Gtk::ListStore( *columns ),
	columns( columns ){
	

}

void AccountsList::setTreeView( Gtk::TreeView * treeView ){
	const Glib::RefPtr<AccountsList> accountsList = 
		(const Glib::RefPtr<AccountsList>)(this);
	treeView->set_model( accountsList );
#ifndef IPAQ
	treeView->append_column_editable( "Register", columns->doRegister );
#else
	treeView->append_column_editable( "R", columns->doRegister );
#endif
	treeView->append_column( "Account", columns->name );
#ifndef IPAQ
	treeView->append_column( "Default", columns->defaultProxy );
	treeView->append_column( "PSTN", columns->pstnProxy );
#else
	treeView->append_column( "D", columns->defaultProxy );
	treeView->append_column( "P", columns->pstnProxy );
#endif
	treeView->columns_autosize();

}

void AccountsList::loadFromConfig( MRef<SipSoftPhoneConfiguration *> config ){
	list< MRef<SipIdentity *> > identities = config->identities;
	list< MRef<SipIdentity *> >::iterator i;

	for( i = identities.begin(); i != identities.end(); i++ ){
		Gtk::TreeModel::iterator iter = append();
		(*iter)[columns->name] = (*i)->identityIdentifier;
		(*iter)[columns->uri] = (*i)->sipUsername + "@" + (*i)->sipDomain;
		(*iter)[columns->proxy] = (*i)->sipProxy.sipProxyAddressString;
		(*iter)[columns->username] = (*i)->sipProxy.sipProxyUsername;
		(*iter)[columns->password] = (*i)->sipProxy.sipProxyPassword;
		(*iter)[columns->doRegister] = (*i)->registerToProxy;
		(*iter)[columns->defaultProxy] = ( (*i) == config->inherited.sipIdentity );
		(*iter)[columns->pstnProxy] = ( (*i) == config->pstnIdentity );
	}

}

string AccountsList::saveToConfig( MRef<SipSoftPhoneConfiguration *> config ){
	config->identities.clear();
	config->inherited.sipIdentity = NULL;
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

		uint16_t port = 5060;

		try{
		
			if( proxy != "" ){
				identity->sipProxy = SipProxy( proxy );
			}
			else{
				string foundProxy = SipProxy::findProxy( uri, port );
				if( foundProxy != "unknown" ){
					identity->sipProxy = SipProxy( foundProxy );
				}

				else{
					err += "Could not autodetect the proxy "
						"for the SIP account " + name + ". "
						"Minisip will not register to "
						"that account.";
					identity->setDoRegister( false );
				}
			}

		}
		catch( NetworkException * exc ){
			err += "Minisip encountered the following network "
				"error when resolving the proxy address of "
				"the SIP account " + name + ": " +
				exc->errorDescription() +". " +
				"Minisip will not register to "
				"that account.";
			identity->setDoRegister( false );
		}

		//FIXME
		identity->sipProxy.sipProxyPort = port;

		identity->sipProxy.sipProxyUsername = 
			Glib::locale_from_utf8( (*iter)[columns->username] );
		
		identity->sipProxy.sipProxyPassword = 
			Glib::locale_from_utf8( (*iter)[columns->password] );


		if( (*iter)[columns->pstnProxy] ){
			identity->securitySupport = false;
			config->usePSTNProxy = true;
			config->pstnIdentity = identity;
		}

		if( !config->inherited.sipIdentity ||
				(*iter)[columns->defaultProxy] ){
			config->inherited.sipIdentity = identity;
		}

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
