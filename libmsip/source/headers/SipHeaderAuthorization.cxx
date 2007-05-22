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
 * 	SipHeaderValueAuthorization.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#include<config.h>


#include<libmsip/SipHeaderAuthorization.h>
#include<libmutil/stringutils.h>

using namespace std;


MRef<SipHeaderValue *> authorizationFactory(const string &build_from){
	return new SipHeaderValueAuthorization(build_from);
}

SipHeaderFactoryFuncPtr sipHeaderAuthorizationFactory=authorizationFactory;



const string sipHeaderValueAuthorizationTypeString="Authorization";

SipHeaderValueAuthorization::SipHeaderValueAuthorization(int type_, const string &typeStr) 
	: SipHeaderValue(type_, typeStr)
{
	
}	

SipHeaderValueAuthorization::SipHeaderValueAuthorization(const string &build_from)
		: SipHeaderValue(SIP_HEADER_TYPE_AUTHORIZATION, sipHeaderValueAuthorizationTypeString)
{
	init( build_from );
}

SipHeaderValueAuthorization::SipHeaderValueAuthorization(int type_, const string &build_from, const string &typeStr)
		: SipHeaderValue(type_,typeStr)
{
	init( build_from );
}

void SipHeaderValueAuthorization::init(const string &build_from){
	size_t pos = build_from.find_first_not_of(" \t\r\n");
	size_t last = build_from.find_first_of(" \t\r\n", pos);

	if( last != string::npos ){
		auth_method = build_from.substr( pos, last - pos );
		addParameter( new SipHeaderParameter( build_from.substr( last )));
	}
}

SipHeaderValueAuthorization::SipHeaderValueAuthorization(
		const string &username, 
		const string &realm, 
		const string &nonce, 
		const string &opaque,
		const SipUri &uri, 
		const string &response,
		const string &auth_meth)
			: SipHeaderValue(SIP_HEADER_TYPE_AUTHORIZATION, sipHeaderValueAuthorizationTypeString),
			auth_method(auth_meth)
{
	setParameter("algorithm", "MD5");
	setParameter("username", quote(username));
	setParameter("realm", quote(realm));
	setParameter("nonce", quote(nonce));
	setParameter("uri", quote(uri.getRequestUriString()));
	setParameter("response", quote(response));
	if( opaque != "")
		setParameter("opaque", quote(opaque));
}

SipHeaderValueAuthorization::SipHeaderValueAuthorization(int type_,
		const string &username, 
		const string &realm, 
		const string &nonce, 
		const string &opaque,
		const SipUri &uri, 
		const string &response,
		const string &auth_meth,
		const string &typeStr
		)
			: SipHeaderValue(type_,typeStr),
			auth_method(auth_meth)
{
	setParameter("algorithm", "MD5");
	setParameter("username", quote(username));
	setParameter("realm", quote(realm));
	setParameter("nonce", quote(nonce));
	setParameter("uri", quote(uri.getRequestUriString()));
	setParameter("response", quote(response));
	if( opaque != "")
		setParameter("opaque", quote(opaque));
}

SipHeaderValueAuthorization::~SipHeaderValueAuthorization() {

}

string SipHeaderValueAuthorization::getString() const{
	return auth_method;
}

void SipHeaderValueAuthorization::setParameter(const std::string &name,
					       const std::string &value)
{
	addParameter(new SipHeaderParameter(name, value, true));
}

string SipHeaderValueAuthorization::getUsername() const{
	return getParameter("username");
}

void SipHeaderValueAuthorization::setUsername(const string &un){
	setParameter("username", un);
}


string SipHeaderValueAuthorization::getNonce() const{
	return getParameter("none");
}

void SipHeaderValueAuthorization::setNonce(const string &n){
	setParameter("nonce", n);
}

string SipHeaderValueAuthorization::getOpaque() const{
	return getParameter("opaque");
}

void SipHeaderValueAuthorization::setOpaque(const string &n){
	setParameter("opaque", n);
}

string SipHeaderValueAuthorization::getRealm() const{
	return getParameter("realm");
}

void SipHeaderValueAuthorization::setRealm(const string &r){
	setParameter("realm", r);
}

SipUri SipHeaderValueAuthorization::getUri() const{
	return getParameter("uri");
}

void SipHeaderValueAuthorization::setUri(const SipUri &uri){
	setParameter("uri", uri.getRequestUriString());
}

