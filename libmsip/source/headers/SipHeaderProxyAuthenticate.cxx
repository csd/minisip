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
 * 	SipHeaderValueProxyAuthenticate.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#include<config.h>

#include<libmsip/SipHeaderProxyAuthenticate.h>

const string sipHeaderValueProxyAuthenticationTypeStr = "Proxy-Authenticate";
		
SipHeaderValueProxyAuthenticate::SipHeaderValueProxyAuthenticate()
		: SipHeaderValue(SIP_HEADER_TYPE_PROXYAUTHENTICATE,sipHeaderValueProxyAuthenticationTypeStr)
{
	
}
		

SipHeaderValueProxyAuthenticate::SipHeaderValueProxyAuthenticate(const string &/*build_from*/) //TODO: Parse proxy authenticate header value
		: SipHeaderValue(SIP_HEADER_TYPE_PROXYAUTHENTICATE,sipHeaderValueProxyAuthenticationTypeStr)
{
	
}

SipHeaderValueProxyAuthenticate::~SipHeaderValueProxyAuthenticate(){

}
		
string SipHeaderValueProxyAuthenticate::getString(){
	return /*"Proxy-Authenticate: "+*/method+", realm="+realm+", nonce="+nonce+", algorithm="+algorithm;
} 

string SipHeaderValueProxyAuthenticate::getMethod(){
	return method;
}

void SipHeaderValueProxyAuthenticate::setMethod(const string &m){
	this->method=m;
}
	
string SipHeaderValueProxyAuthenticate::getNonce(){
	return nonce;
}

void SipHeaderValueProxyAuthenticate::setNonce(const string &n){
	this->nonce=n;
}
		
string SipHeaderValueProxyAuthenticate::getRealm(){
	return realm;
}

void SipHeaderValueProxyAuthenticate::setRealm(const string &r){
	this->realm=r;
}
			
string SipHeaderValueProxyAuthenticate::getAlgorithm(){
	return algorithm;
}

void SipHeaderValueProxyAuthenticate::setAlgorithm(const string &a){
	this->algorithm=a;
}
	
