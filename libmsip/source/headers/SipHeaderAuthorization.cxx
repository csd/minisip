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

/* Name
 * 	SipHeaderValueAuthorization.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#include<config.h>


#include<libmsip/SipHeaderAuthorization.h>
#include<libmutil/vmd5.h>

const string sipHeaderValueAuthorizationTypeString="Authorization";

SipHeaderValueAuthorization::SipHeaderValueAuthorization() 
	: SipHeaderValue(SIP_HEADER_TYPE_AUTHORIZATION,sipHeaderValueAuthorizationTypeString),uri("NONAME","0.0.0.0", "phone",0)
{

}

SipHeaderValueAuthorization::SipHeaderValueAuthorization(int type, const string &typeStr) 
	: SipHeaderValue(type, typeStr),uri("NONAME","0.0.0.0", "phone",0)
{
	
}	

SipHeaderValueAuthorization::SipHeaderValueAuthorization(const string &/*build_from*/)	//TODO: parse authorization header 
		: SipHeaderValue(SIP_HEADER_TYPE_AUTHORIZATION, sipHeaderValueAuthorizationTypeString), 
		uri("NONAME","0.0.0.0", "phone",0)
{
	
}

SipHeaderValueAuthorization::SipHeaderValueAuthorization(int type, const string &/*build_from*/, const string &typeStr) //TODO: parse authorization header
		: SipHeaderValue(type,typeStr), 
		uri("NONAME","0.0.0.0", "phone",0){
//	type = SIP_HEADER_TYPE_AUTHORIZATION;

}

SipHeaderValueAuthorization::SipHeaderValueAuthorization(const string &sip_method,
		const string &username, 
		const string &realm, 
		const string &nonce, 
		const SipURI &uri, 
		const string &auth_id, 
		const string &password,
		const string &auth_method)
			: SipHeaderValue(SIP_HEADER_TYPE_AUTHORIZATION, sipHeaderValueAuthorizationTypeString),
                        sipMethod(sip_method),
			username(username),
			realm(realm),
			nonce(nonce),
			uri(uri),
			auth_id(auth_id),
			password(password),
			auth_method(auth_method)
{

}

SipHeaderValueAuthorization::SipHeaderValueAuthorization(int type,
		const string &sip_method,
		const string &username, 
		const string &realm, 
		const string &nonce, 
		const SipURI &uri, 
		const string &auth_id, 
		const string &password,
		const string &auth_method,
		const string &typeStr
		)
			: SipHeaderValue(type,typeStr),
                        sipMethod(sip_method),
			username(username),
			realm(realm),
			nonce(nonce),
			uri(uri),
			auth_id(auth_id),
			password(password),
			auth_method(auth_method)
{

}

SipHeaderValueAuthorization::~SipHeaderValueAuthorization() {

}

string SipHeaderValueAuthorization::getString(){
	uri.setUserType("");
	return /*"Authorization: "+*/ auth_method+
		" algoritm=\"md5\""+", username=\""+auth_id+
		"\", realm=\""+realm+"\", nonce=\""+nonce+
		"\", uri=\""+uri.getUserIpString()+"\", response=\""+
		calcResponse()+"\"";
} 

string SipHeaderValueAuthorization::md5ToString(unsigned char *md5){
	char *digits = {"0123456789abcdef"};
	char strsum[33] = {'\0'};
	for (int i=0 ; i<16; i++){
		int32_t intval = md5[i];
		strsum[2*i] = digits[(intval & 0xF0) >>4];
		strsum[2*i+1] = digits[(intval & 0x0F)];
	}
	return string(strsum);
}



string SipHeaderValueAuthorization::calcResponse(){
	unsigned char digest[16];
	MD5Context context;
	MD5Init(&context);
	string u_r_p(auth_id+":"+realm+":"+password);
	MD5Update(&context, (const unsigned char *)u_r_p.c_str(), u_r_p.length() );
	MD5Final(digest,&context);
	string md5_u_r_p = md5ToString(digest);

	string uri_part = uri.getUserIpString();
	MD5Context c2;
	MD5Init(&c2);
	string uristr(sipMethod+":"+/*sip:"+username+"@"+uri.get_ip()*/ uri_part);
	//cerr << "DEBUG: uri_part="<< uri_part<< " sip_method = "<<sip_method<< endl;
	MD5Update(&c2, (const unsigned char *)uristr.c_str(), (uristr).length() );
	MD5Final(digest,&c2);
	string md5_uri = md5ToString(digest);

	MD5Context c3;
	MD5Init(&c3);
	string all(md5_u_r_p+":"+nonce+":"+md5_uri);
	MD5Update(&c3,(const unsigned char *)all.c_str(), all.length() );
	MD5Final(digest,&c3);

	string auth_string = md5ToString(digest);
	return auth_string;
}

string SipHeaderValueAuthorization::getSipMethod(){
	return sipMethod;
}

void SipHeaderValueAuthorization::setSipMethod(const string &m){
	this->sipMethod=m;
}

string SipHeaderValueAuthorization::getUsername(){
	return username;
}

void SipHeaderValueAuthorization::setUsername(const string &un){
	this->username=un;
}


string SipHeaderValueAuthorization::getNonce(){
	return nonce;
}

void SipHeaderValueAuthorization::setNonce(const string &n){
	this->nonce=n;
}

string SipHeaderValueAuthorization::getRealm(){
	return realm;
}

void SipHeaderValueAuthorization::setRealm(const string &r){
	this->realm=r;
}

SipURI SipHeaderValueAuthorization::getUri(){
	return uri;
}

void SipHeaderValueAuthorization::setUri(const SipURI &uri){
	this->uri=uri;
}

