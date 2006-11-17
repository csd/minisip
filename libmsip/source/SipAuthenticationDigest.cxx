/*
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
 * Authors: Mikael Magnusson <mikma@users.sourceforge.net>
*/

#include<config.h>

#include<libmsip/SipAuthenticationDigest.h>
#include<libmsip/SipRequest.h>
#include<libmsip/SipHeaderAuthorization.h>
#include<libmsip/SipHeaderProxyAuthenticate.h>
#include<libmsip/SipHeaderProxyAuthorization.h>

using namespace std;

std::string SipAuthenticationDigest::nullStr = string("\0", 1);

SipAuthenticationDigest::SipAuthenticationDigest( MRef<SipHeaderValueProxyAuthenticate*> auth ):
		type(-1),realm(nullStr),nonce(nullStr),opaque(nullStr),qop(nullStr){
	update( auth );
}

const std::string &SipAuthenticationDigest::getRealm() const{ 
	return realm; 
}

bool SipAuthenticationDigest::getStale() const{ 
	return stale;
}


string unquote(string str){
	if( str.length() > 1 && str[0] == '"' && str[str.length() - 1] == '"' )
		return str.substr(1, str.length() - 2);
	else
		return str;
}

bool SipAuthenticationDigest::update( MRef<SipHeaderValueProxyAuthenticate*> auth ){
	if( type > -1 && type != auth->getType() ){
		mdbg << "SipAuthenticationDigest::update non-matching header type" << end;
		return false;
	}
	type = auth->getType();

	string realmParam = unquote( auth->getParameter("realm") );
	if( realm != nullStr && realm != realmParam ){
		mdbg << "SipAuthenticationDigest::update non-matching realm" << end;
		return false;
	}
	realm = realmParam;

	nonce = unquote( auth->getParameter("nonce") );

	if( auth->hasParameter("opaque") )
		opaque = unquote( auth->getParameter("opaque") );
	else
		opaque = nullStr;

	if( auth->hasParameter("qop") )
		qop = auth->getParameter("qop");
	else
		qop = nullStr;

	if( auth->hasParameter("stale") ){
		stale = ( auth->getParameter("stale") != "false" );
	}
	else
		stale = false;

	return true;
}

MRef<SipHeaderValueAuthorization*> SipAuthenticationDigest::createAuthorization( MRef<SipRequest*> req ) const{
	MRef<SipHeaderValueAuthorization*> authorization;

	SipUri uri( req->getUri() );

	if( type == SIP_HEADER_TYPE_WWWAUTHENTICATE ){
		authorization = new SipHeaderValueAuthorization(
			req->getMethod(),
			"",
			realm,
			nonce,
			opaque == nullStr ? "" : opaque,
			uri,
			username,
			password,
			"Digest"
			);
	}
	else {
		authorization = new SipHeaderValueProxyAuthorization(
			req->getMethod(),
			"",
			realm,
			nonce,
			opaque == nullStr ? "" : opaque,
			uri,
			username,
			password,
			"Digest"
			);
	}

	return authorization;
}

void SipAuthenticationDigest::setCredential(const std::string &username, const std::string &password){
	this->username = username;
	this->password = password;
}
