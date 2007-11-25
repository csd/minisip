/*
  Copyright (C) 2006-2007 Mikael Magnusson
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/*
 * Authors: Mikael Magnusson <mikma@users.sourceforge.net>
*/

#include<config.h>

#include<libmsip/SipStack.h>
#include<libmsip/SipTransport.h>
#include<libminisip/signaling/sip/SipSoftPhoneConfiguration.h>
#include"TransportList.h"

using namespace std;

TransportListColumns::TransportListColumns(){
	add( config );
	add( name );
	add( scheme );
	add( protocol );
	add( description );
	add( enabled );
}

Glib::RefPtr<TransportList> TransportList::create(){
	TransportListColumns *columns = new TransportListColumns();

	return Glib::RefPtr<TransportList>( new TransportList( columns ) );
}

TransportList::TransportList( TransportListColumns *columns ):
		Gtk::ListStore( *columns ),
		columns( columns )
{
}


void TransportList::loadFromConfig( MRef<SipSoftPhoneConfiguration *> config ){
	list< MRef<SipTransportConfig*> > &transports =
		config->sipStackConfig->transports;
	list< MRef<SipTransportConfig*> >::iterator i;
	list< MRef<SipTransportConfig*> >::iterator last = transports.end();

	for( i = transports.begin(); i != last; i++ ){
		MRef<SipTransportConfig*> transportConfig = (*i);
		MRef<SipTransport*> transport =
			SipTransportRegistry::getInstance()->findTransportByName( transportConfig->getName() );
		Gtk::TreeModel::iterator iter = append();

		(*iter)[columns->config] = transportConfig;
		(*iter)[columns->name] = transport->getName();
		(*iter)[columns->scheme] =
			transport->isSecure() ? "sips" : "sip";
		(*iter)[columns->protocol] = transport->getProtocol();
		(*iter)[columns->description] = transport->getDescription();
		(*iter)[columns->enabled] = transportConfig->isEnabled();
	}
}

std::string TransportList::saveToConfig( MRef<SipSoftPhoneConfiguration *> config ){
	string err = "";
	list< MRef<SipTransportConfig*> > transports =
		config->sipStackConfig->transports;
	Gtk::TreeModel::iterator iter;

	for( iter = children().begin(); iter != children().end(); iter++ ){
		MRef<SipTransportConfig*> transportConfig = (*iter)[columns->config];
		bool enabled = (*iter)[columns->enabled];

		transportConfig->setEnabled( enabled );
	}

	return err;
}

TransportListColumns *TransportList::getColumns() const{
	return columns;
}
