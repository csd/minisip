/*
  Copyright (C) 2006-2007  Mikael Magnusson
  
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

#include"SipTransport.h"
#include"SipTransportUdp.h"
#include"SipTransportTcp.h"
#include"SipTransportTls.h"

using namespace std;

SipTransport::SipTransport(): MPlugin(){
}

SipTransport::SipTransport( MRef<Library *> lib ): MPlugin( lib ){
}

int32_t SipTransport::getDefaultPort(){
	return isSecure() ? 5061 : 5060;
}

MRef<StreamSocket *> SipTransport::connect( const IPAddress &addr, uint16_t port, MRef<CertificateSet *> cert_db, MRef<CertificateChain *> certChain ){
	throw Exception("Connection less transport");
}


SipTransportRegistry::SipTransportRegistry(){
	registerPlugin( new SipTransportUdp( NULL ) );
	registerPlugin( new SipTransportTcp( NULL ) );
	registerPlugin( new SipTransportTls( NULL ) );
}

MRef<SipTransport*> SipTransportRegistry::findTransport( const string &protocol, bool secure ) const{
	string lcProt = protocol;
	list< MRef<MPlugin*> >::const_iterator iter;
	list< MRef<MPlugin*> >::const_iterator stop = plugins.end();

	transform( lcProt.begin(), lcProt.end(),
		   lcProt.begin(), (int(*)(int))tolower );

	for( iter = plugins.begin(); iter != stop; iter++ ){
		MRef<MPlugin*> plugin = *iter;

		if( !plugin )
			continue;

		MRef<SipTransport*> transport =
			dynamic_cast<SipTransport*>( *plugin );

		if( !transport )
			continue;

		if( transport->isSecure() == secure &&
		    transport->getProtocol() == lcProt ){
			mdbg << "SipTransport: tranport found!!! =  " << lcProt << endl;
			return transport;
		}
	}

	return NULL;
}

MRef<SipTransport*> SipTransportRegistry::findViaTransport( const string &protocol ) const{
	string ucProt = protocol;
	list< MRef<MPlugin*> >::const_iterator iter;
	list< MRef<MPlugin*> >::const_iterator stop = plugins.end();

	transform( ucProt.begin(), ucProt.end(),
		   ucProt.begin(), (int(*)(int))toupper );

	for( iter = plugins.begin(); iter != stop; iter++ ){
		MRef<MPlugin*> plugin = *iter;

		if( !plugin )
			continue;

		MRef<SipTransport*> transport =
			dynamic_cast<SipTransport*>( *plugin );

		if( !transport )
			continue;

		if( transport->getViaProtocol() == ucProt ){
			mdbg << "SipTransport: Via transport found!!! =  " << ucProt << endl;
			return transport;
		}
	}

	return NULL;
}
