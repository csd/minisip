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
 * 	SipHeaderProxyAuthenticate.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#include<config.h>

#include<libmsip/SipHeaderProxyAuthenticate.h>

		
SipHeaderProxyAuthenticate::SipHeaderProxyAuthenticate()
		: SipHeader(SIP_HEADER_TYPE_PROXYAUTHENTICATE)
{
	
}
		

SipHeaderProxyAuthenticate::SipHeaderProxyAuthenticate(const string &build_from)
		: SipHeader(SIP_HEADER_TYPE_PROXYAUTHENTICATE)
{
	
}

SipHeaderProxyAuthenticate::~SipHeaderProxyAuthenticate(){

}
		
string SipHeaderProxyAuthenticate::getString(){
	return "Proxy-Authenticate: "+method+", realm="+realm+", nonce="+nonce+", algorithm="+algorithm;
} 

string SipHeaderProxyAuthenticate::getMethod(){
	return method;
}

void SipHeaderProxyAuthenticate::setMethod(const string &m){
	this->method=m;
}
	
string SipHeaderProxyAuthenticate::getNonce(){
	return nonce;
}

void SipHeaderProxyAuthenticate::setNonce(const string &n){
	this->nonce=n;
}
		
string SipHeaderProxyAuthenticate::getRealm(){
	return realm;
}

void SipHeaderProxyAuthenticate::setRealm(const string &r){
	this->realm=r;
}
			
string SipHeaderProxyAuthenticate::getAlgorithm(){
	return algorithm;
}

void SipHeaderProxyAuthenticate::setAlgorithm(const string &a){
	this->algorithm=a;
}
	
