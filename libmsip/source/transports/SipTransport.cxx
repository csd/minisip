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

SipTransportRegistry::SipTransportRegistry(){
	registerPlugin( new SipTransportUdp( NULL ) );
	registerPlugin( new SipTransportTcp( NULL ) );
	registerPlugin( new SipTransportTls( NULL ) );
}

MRef<SipTransport*> SipTransportRegistry::findTransport( string protocol, bool secure ) const{
	list< MRef<MPlugin*> >::const_iterator iter;
	list< MRef<MPlugin*> >::const_iterator stop = plugins.end();

	for( iter = plugins.begin(); iter != stop; iter++ ){
		MRef<MPlugin*> plugin = *iter;

		if( !plugin )
			continue;

		MRef<SipTransport*> transport =
			dynamic_cast<SipTransport*>( *plugin );

		if( !transport )
			continue;

		if( transport->isSecure() == secure &&
		    transport->getProtocol() == protocol ){
			mdbg << "SipTransport: tranport found!!! =  " << protocol << endl;
			return transport;
		}
	}

	return NULL;
}
