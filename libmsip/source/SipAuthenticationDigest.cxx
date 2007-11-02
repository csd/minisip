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
#include<libmsip/SipDialogConfig.h>
#include<libmsip/SipRequest.h>
#include<libmsip/SipHeaderAuthorization.h>
#include<libmsip/SipHeaderProxyAuthenticate.h>
#include<libmsip/SipHeaderProxyAuthorization.h>
#include<libmutil/vmd5.h>
#include<libmutil/stringutils.h>

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

bool SipAuthenticationDigest::update( MRef<SipHeaderValueProxyAuthenticate*> auth ){
	if( type > -1 && type != auth->getType() ){
		mdbg("signaling/sip") << "SipAuthenticationDigest::update non-matching header type" << endl;
		return false;
	}
	type = auth->getType();

	string realmParam = unquote( auth->getParameter("realm") );
	if( realm != nullStr && realm != realmParam ){
		mdbg("signaling/sip") << "SipAuthenticationDigest::update non-matching realm" << endl;
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

string SipAuthenticationDigest::md5ToString(unsigned char *md5){
	const char *digits = {"0123456789abcdef"};
	char strsum[33] = {'\0'};
	for (int i=0 ; i<16; i++){
		int32_t intval = md5[i];
		strsum[2*i] = digits[(intval & 0xF0) >>4];
		strsum[2*i+1] = digits[(intval & 0x0F)];
	}
	return string(strsum);
}

string SipAuthenticationDigest::calcResponse( MRef<SipRequest*> req ) const
{
	unsigned char digest[16];
	MD5Context context;
	MD5Init(&context);
	string u_r_p(getUsername()+":"+realm+":"+getPassword());
	MD5Update(&context, (const unsigned char *)u_r_p.c_str(), (unsigned int)u_r_p.length() );
	MD5Final(digest,&context);
	string md5_u_r_p = md5ToString(digest);

	string uri_part = req->getUri().getRequestUriString();
	MD5Context c2;
	MD5Init(&c2);
	string uristr(req->getMethod()+":"+ uri_part);
	//cerr << "DEBUG: uri_part="<< uri_part<< " sip_method = "<<sip_method<< endl;
	MD5Update(&c2, (const unsigned char *)uristr.c_str(), (unsigned int)uristr.length() );
	MD5Final(digest,&c2);
	string md5_uri = md5ToString(digest);

	MD5Context c3;
	MD5Init(&c3);
	string all(md5_u_r_p+":"+nonce+":"+md5_uri);
	MD5Update(&c3,(const unsigned char *)all.c_str(), (unsigned int)all.length() );
	MD5Final(digest,&c3);

	string auth_string = md5ToString(digest);
	return auth_string;
}

MRef<SipHeaderValueAuthorization*> SipAuthenticationDigest::createAuthorization( MRef<SipRequest*> req ) const{
	MRef<SipHeaderValueAuthorization*> authorization;

	SipUri uri( req->getUri() );
	string response = calcResponse( req );

	if( type == SIP_HEADER_TYPE_WWWAUTHENTICATE ){
		authorization = new SipHeaderValueAuthorization(
			getUsername(),
			realm,
			nonce,
			opaque == nullStr ? "" : opaque,
			uri,
			response,
			"Digest"
			);
	}
	else {
		authorization = new SipHeaderValueProxyAuthorization(
			getUsername(),
			realm,
			nonce,
			opaque == nullStr ? "" : opaque,
			uri,
			response,
			"Digest"
			);
	}

	return authorization;
}

void SipAuthenticationDigest::setCredential( MRef<SipCredential*> credential ){
	cred = credential;
}

MRef<SipCredential*> SipAuthenticationDigest::getCredential() const{
	return cred;
}

const string &SipAuthenticationDigest::getUsername() const{
	static string anonymous = "anonymous";

	if( cred.isNull() )
		return anonymous;
	else
		return cred->getUsername();
}

const string &SipAuthenticationDigest::getPassword() const{
	static string empty = "";

	if( cred.isNull() )
		return empty;
	else
		return cred->getPassword();
}
