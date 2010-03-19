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

#include<libmsip/SipTransport.h>
#include"SipTransportUdp.h"
#include"SipTransportTcp.h"
#include"SipTransportTls.h"
#include"SipTransportDtlsUdp.h"

#ifdef HAVE_SCTP
#include"SipTransportSctp.h"
#include"SipTransportTlsSctp.h"
#endif

#include<algorithm>

using namespace std;

SipTransport::SipTransport(): MPlugin(){
}

SipTransport::SipTransport( MRef<Library *> lib ): MPlugin( lib ){
}

string SipTransport::getUriScheme() const{
	return isSecure() ? "sips" : "sip";
}

int32_t SipTransport::getDefaultPort() const{
	return isSecure() ? 5061 : 5060;
}

string SipTransport::getSrv() const{
	const string &service = getUriScheme();
	const string &proto = getProtocol();

	return "_" + service + "._" + proto;
}

MRef<StreamSocket *> SipTransport::connect( const IPAddress &addr, uint16_t port, MRef<CertificateSet *> cert_db, MRef<CertificateChain *> certChain ){
	throw Exception("Connection less transport");
}


SipTransportRegistry::SipTransportRegistry(){
	registerPlugin( new SipTransportUdp( NULL ) );
	registerPlugin( new SipTransportTcp( NULL ) );
	registerPlugin( new SipTransportTls( NULL ) );
#ifdef HAVE_DTLS
	registerPlugin( new SipTransportDtlsUdp( NULL ) );
#endif
#ifdef HAVE_SCTP
	registerPlugin( new SipTransportSctp( NULL ) );
	registerPlugin( new SipTransportTlsSctp( NULL ) );
#endif
}

list<string> SipTransportRegistry::getNaptrServices( bool secureOnly ) const{
	list<string> services;
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

		if( !secureOnly || transport->isSecure() ){
			services.push_back( transport->getNaptrService() );
		}
	}

	return services;
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
			//mdbg << "SipTransport: transport found!!! =  " << lcProt << endl;
			return transport;
		}
	}

	mdbg << "SipTransport: transport not found!!! = '" << lcProt << "'" << endl;
	return NULL;
}

MRef<SipTransport*> SipTransportRegistry::findTransport( const SipUri &uri ) const{
	return findTransport( uri.getTransport(), uri.getProtocolId() == "sips" );
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

MRef<SipTransport*> SipTransportRegistry::findTransport( int32_t socketType ) const{
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

		if( transport->getSocketType() == socketType ){
			mdbg << "SipTransport: Socket type found!!! =  " << socketType << endl;
			return transport;
		}
	}

	return NULL;
}

MRef<SipTransport*> SipTransportRegistry::findTransportByName( const std::string &name ) const{
	MRef<MPlugin*> transport = findPlugin( name );

	if( transport ){
		return dynamic_cast<SipTransport*>(*transport);
	}

	return NULL;
}

MRef<SipTransport*> SipTransportRegistry::findTransportByNaptr( const std::string &service ) const{
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

		if( transport->getNaptrService() == service ){
			mdbg << "SipTransport: NAPTR service found!!! =  " << service << endl;
			return transport;
		}
	}

	return NULL;
}

list<MRef<SipTransportConfig*> > SipTransportRegistry::createDefaultConfig() const{
	list< MRef<SipTransportConfig* > > result;
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

		MRef<SipTransportConfig*> config =
			new SipTransportConfig( transport->getName() );

		result.push_back( config );
	}

	return result;
}
