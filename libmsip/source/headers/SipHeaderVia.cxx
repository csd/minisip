/*
  Copyright (C) 2005, 2004 Erik Eliasson, Johan Bilien
  Copyright (C) 2005  Mikael Magnusson
  
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
 * 	SipHeaderValueVia.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#include<config.h>

#include<libmsip/SipHeaderVia.h>
#include<libmsip/SipException.h>
#include<libmutil/stringutils.h>

#include<stdlib.h>

using namespace std;

MRef<SipHeaderValue *> viaFactory(const string &build_from){
	                return new SipHeaderValueVia(build_from);
}

SipHeaderFactoryFuncPtr sipHeaderViaFactory=viaFactory;


const string sipHeaderValueViaTypeStr = "Via";

SipHeaderValueVia::SipHeaderValueVia(const string &build_from)
		: SipHeaderValue(SIP_HEADER_TYPE_VIA,sipHeaderValueViaTypeStr)
{
	size_t i=0;
	size_t pos=0;

	ip="";
	port=0;
	while (isWS(build_from[i]))
		i++;

	// Parse Via protocol (name and version)
	pos = build_from.find( '/', i );
	if( pos == string::npos ){
		throw SipExceptionInvalidMessage("SipHeaderValueVia malformed - via value did not contain version");
	}

	const string proto = trim(build_from.substr(i,pos-i)); 
	if (proto != "SIP") {
		throw SipExceptionInvalidMessage("SipHeaderValueVia malformed - version protocol" );
	}

	i = pos + 1; 

	pos = build_from.find( '/', i);
	if( pos == string::npos ){
		throw SipExceptionInvalidMessage("SipHeaderValueVia malformed - via value did not contain version");
	}

	const string ver = trim(build_from.substr( i, pos - i ));
	if (ver!= "2.0") {
		throw SipExceptionInvalidMessage("SipHeaderValueVia malformed - version unknown" );
	}
	i = pos + 1;


	while (isWS(build_from[i]))
		i++;

	// Parse Via transport
	pos = build_from.find_first_of(" \t\n\t", i);
	if( pos == string::npos ){
		throw SipExceptionInvalidMessage("SipHeaderValueVia malformed - could not determine transport protocol");
	}
	protocol = trim(build_from.substr( i, pos - i ));
	i = pos + 1;

	pos = build_from.find_first_not_of(" \t\n\r", i);
	if( pos == string::npos ){
		throw SipExceptionInvalidMessage("SipHeaderValueVia malformed - could not determine sent-by");
	}
	i = pos;

	size_t start = pos;

	// Parse sent-by host
	// Search for end of host name
	pos = build_from.find_first_of( "[:;, \t\n\r", i );
	if( pos == string::npos ){
		pos = build_from.size();
	}

	size_t end = pos;

	if( build_from[pos] == '[' ){
		// IPv6 address
		start = pos + 1;
		pos = build_from.find( ']', start );
		if( pos == string::npos ){
			throw SipExceptionInvalidMessage("SipHeaderValueVia malformed - could not determine sent-by");
		}

		end = pos;
		pos++;
	}

	ip = build_from.substr( start, end - start );
	i = pos;

	pos = build_from.find_first_not_of( " \t\n\r", i );
	if( pos != string::npos && build_from[pos] == ':' ){
		i = pos + 1;

		i = build_from.find_first_not_of(";, \t\n\r", i );
		if( i == string::npos ){
			throw SipExceptionInvalidMessage("SipHeaderValueVia malformed - could not determine port number");
		}

		pos = build_from.find_first_of(";, \t\n\r", i );
		if( pos == string::npos ){
			pos = build_from.length() + i;
		}
		
		string portstr = build_from.substr( i, pos - i );
		
		port = atoi( portstr.c_str() );
	}
	
}

SipHeaderValueVia::SipHeaderValueVia()
		: SipHeaderValue(SIP_HEADER_TYPE_VIA,sipHeaderValueViaTypeStr)
{
	port=0;
	protocol="UDP";
	ip="UNKNOWN_IP";
}

SipHeaderValueVia::SipHeaderValueVia(const string &proto, const string &ip_, int32_t port_)
		: SipHeaderValue(SIP_HEADER_TYPE_VIA,sipHeaderValueViaTypeStr)
{
	setProtocol(proto);
	setIp(ip_);
	setPort(port_);
}

SipHeaderValueVia::~SipHeaderValueVia(){
}

string SipHeaderValueVia::getString() const{
	string via;
	via = "SIP/2.0/"+protocol+" ";
	if( ip.find(':') != string::npos )
		// IPv6
		via += '[' + ip + ']';
	else
		via += ip;
		
	if (port>0)
		via=via+":"+itoa(port);
	
	return via;
}

string SipHeaderValueVia::getProtocol() const{
	return protocol;
}

void SipHeaderValueVia::setProtocol(const string &p){
	this->protocol=p;
}
		
string SipHeaderValueVia::getIp() const{
	return ip;
}
		
void SipHeaderValueVia::setIp(const string &i){
	this->ip=i;
}

int32_t SipHeaderValueVia::getPort() const{
	return port;
}

void SipHeaderValueVia::setPort(int32_t p){
	this->port=p;
}

