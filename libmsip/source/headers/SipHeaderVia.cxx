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
	while (build_from[i]==' ')
		i++;

	// Parse Via protocol (name and version)
	pos = build_from.find( '/', i );
	if( pos == string::npos ){
		throw SipExceptionInvalidMessage("SipHeaderValueVia malformed - via value did not contain version");
	}

	pos = build_from.find( '/', pos + 1);
	if( pos == string::npos ){
		throw SipExceptionInvalidMessage("SipHeaderValueVia malformed - via value did not contain version");
	}

	const string proto = build_from.substr( i, pos - i );
	if (proto != "SIP/2.0") {
		throw SipExceptionInvalidMessage("SipHeaderValueVia malformed - version unknown");
	}
	i = pos + 1;


	// Parse Via transport
	pos = build_from.find( ' ', i );
	if( pos == string::npos ){
		throw SipExceptionInvalidMessage("SipHeaderValueVia malformed - could not determine transport protocol");
	}
	protocol = build_from.substr( i, pos - i );
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

/*
	//FIXME: The following code considers parameters -  they are not
	//part of the header value and should not be parsed here (they
	//should not be in the string passed to this method)!!
	string portstr="";
	if (build_from[i]==':'){
		i++;
		while (!(build_from[i]==';' || build_from[i]==' ' || i>=build_from.length())){
			portstr+=build_from[i];
			i++;
		}
		port=atoi(portstr.c_str());
	}

	i++;
	if (build_from.length()-1>i){
		if (build_from.substr(i,7)==string("branch=")){
//			int end = i+7;
			int n=0;
			while (i+7+n<build_from.length() && build_from[i+7+n]!=';')
				n++;
			//branch = build_from.substr(i+7, build_from.length()-7-1);
			branch = build_from.substr(i+7, n);
//			cerr << "branch parsed to: "<< branch<< endl;;
		}
	}
*/
	
}

SipHeaderValueVia::SipHeaderValueVia()
		: SipHeaderValue(SIP_HEADER_TYPE_VIA,sipHeaderValueViaTypeStr)
{
	port=0;
	protocol="UDP";
	ip="UNKNOWN_IP";
}

SipHeaderValueVia::SipHeaderValueVia(const string &proto, const string &ip, int32_t port/*, const string &branch*/)
		: SipHeaderValue(SIP_HEADER_TYPE_VIA,sipHeaderValueViaTypeStr)
{
	setProtocol(proto);
	setIp(ip);
	setPort(port);
}

SipHeaderValueVia::~SipHeaderValueVia(){
}

string SipHeaderValueVia::getString(){
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

string SipHeaderValueVia::getProtocol(){
	return protocol;
}

void SipHeaderValueVia::setProtocol(const string &protocol){
	this->protocol=protocol;
}
		
string SipHeaderValueVia::getIp(){
	return ip;
}
		
void SipHeaderValueVia::setIp(const string &ip){
	this->ip=ip;
}

int32_t SipHeaderValueVia::getPort(){
	return port;
}

void SipHeaderValueVia::setPort(int32_t p){
	this->port=p;
}

