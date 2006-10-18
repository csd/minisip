/*
  Copyright (C) 2005, 2004 Erik Eliasson, Johan Bilien
  Copyright (C) 2006 Mikael Magnusson
  
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
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
 *          Mikael Magnusson <mikma@users.sourceforge.net>
*/


/* Name
 * 	SipHeaderValueProxyAuthenticate.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#include<config.h>

#include<libmsip/SipHeaderProxyAuthenticate.h>

using namespace std;

MRef<SipHeaderValue *> proxyauthFactory(const string &build_from){
	                return new SipHeaderValueProxyAuthenticate(build_from);
}

SipHeaderFactoryFuncPtr sipHeaderProxyAuthenticateFactory=proxyauthFactory;


const string sipHeaderValueProxyAuthenticationTypeStr = "Proxy-Authenticate";
		
SipHeaderValueProxyAuthenticate::SipHeaderValueProxyAuthenticate(int type, const string &typeStr, const string &build_from)
		: SipHeaderValue(type, typeStr), authMethod("")
{
	init(build_from);
}

SipHeaderValueProxyAuthenticate::SipHeaderValueProxyAuthenticate(const string &build_from)
		: SipHeaderValue(SIP_HEADER_TYPE_PROXYAUTHENTICATE,sipHeaderValueProxyAuthenticationTypeStr)
{
	init(build_from);
}

void SipHeaderValueProxyAuthenticate::init(const string &build_from){
	size_t pos = build_from.find_first_not_of(" \t\r\n");
	size_t last = build_from.find_first_of(" \t\r\n", pos);

	if( last != string::npos ){
		authMethod = build_from.substr( pos, last - pos );
		addParameter( new SipHeaderParameter( build_from.substr( last )));
	}
}

SipHeaderValueProxyAuthenticate::~SipHeaderValueProxyAuthenticate(){

}
		
string SipHeaderValueProxyAuthenticate::getString() const{
	return getAuthMethod();
} 

string SipHeaderValueProxyAuthenticate::getAuthMethod() const{
	return authMethod;
}
