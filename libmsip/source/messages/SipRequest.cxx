/*
  Copyright (C) 2005 Mikael Magnusson
  
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
 * Author(s): Mikael Magnusson <mikma@users.sourceforge.net>
*/


/* Name
 * 	SipRequest.cxx
 * Author
 *      Mikael Magnusson, mikma@users.sourceforge.net
 * Purpose
 *      Any SIP request request to libmsip
*/

#include<config.h>

#include<libmsip/SipRequest.h>
#include<libmsip/SipException.h>
#include<libmsip/SipHeader.h>
#include<libmsip/SipHeaderRoute.h>

const int SipRequest::type=11;

SipRequest::SipRequest(string branch, int type, const string &method,
		       const string &uri) :
		SipMessage(branch, type), method(method),
		uri(uri)
{
	if( this->uri == "" )
		this->uri = "sip:";
}

SipRequest::SipRequest(string &build_from): SipMessage(SipRequest::type, build_from){
	init(build_from);
}

SipRequest::SipRequest(int type, string &build_from)
	: SipMessage(type, build_from){
	init(build_from);
}

void SipRequest::init(string &build_from){
	int start = 0;
	int pos;
	int pos2;
	int end = 0;
	int length = build_from.length();
	string requestLine;

	// Skip white space
	start = build_from.find_first_not_of( ' ', start );
	if( start == string::npos ){
		throw new SipExceptionInvalidMessage();
	}

	end = build_from.find_first_of( "\r\n", start );
	if( end == string::npos ){
		throw new SipExceptionInvalidMessage();
	}

	requestLine = build_from.substr( start, end - start );
	start = 0;
	end = requestLine.length();

	// Parse method
	pos = requestLine.find( ' ', start );
	if( pos == string::npos ){
		throw new SipExceptionInvalidMessage();
	}

	method = build_from.substr( start, pos - start );

	// Parse version
	pos2 = requestLine.rfind( ' ', end - 1 );
	if( pos2 == string::npos ){
		throw new SipExceptionInvalidMessage();
	}

	string version = requestLine.substr( pos2 + 1, end - pos2 );

	if( version != "SIP/2.0" ){
		throw new SipExceptionInvalidMessage();
	}	  

	uri = requestLine.substr( pos + 1, pos2 - pos );
}

SipRequest::~SipRequest(){
}

string SipRequest::getString(){
	return getMethod() + " " + getUri() + " SIP/2.0\r\n"
		+ getHeadersAndContent();
}


void SipRequest::setMethod(const string &method){
	this->method = method;
}

string SipRequest::getMethod(){
	return method;
}

static string buildUri(const string &name)
{
	string ret ="";
	
	//FIXME sanitize the request uri ... if we used a SipURI object, this would not be needed
	string username; //hide the class::username ... carefull
	size_t pos;
	username = name;
	
	pos = username.find('<');
	if( pos != string::npos ) {
		username.erase( 0, pos + 1 ); //erase the part in front of the '<'
		pos = username.find('>');
		username.erase( pos );
	}

	if (username.length()>4 && username.substr(0,4)=="sip:")
		ret = username;
	else
		ret = "sip:"+username;

#if 0
	if (username.find("@")==string::npos)
		ret = ret+"@"+ip;
#endif

	return ret;
}

void SipRequest::setUri(const string &uri){
	this->uri = buildUri(uri);
}

string SipRequest::getUri(){
	return uri;
}

void SipRequest::addRoute(const string &route)
{
	MRef<SipHeaderValue*> routeValue = (SipHeaderValue*)new SipHeaderValueRoute( route );
	MRef<SipHeader*> routeHdr = new SipHeader( routeValue );
	int i;
	int pos = 0;
	
	for( i = 0; i < headers.size(); i++ ) {
		if( headers[i]->getType() == SIP_HEADER_TYPE_ROUTE ) {
			pos = i;
			break;
		}
	}

	headers.insert( pos, routeHdr );	
}

void SipRequest::addRoute(const string &addr, int32_t port,
			  const string &transport)
{
	string uri = "<sip:" + addr;

	if( port ){
		char buf[20];
		snprintf(buf, sizeof(buf), "%d", port);
		uri = uri + ":" + buf;
	}

	if( transport != "" ){
		uri = uri + ";transport=" + transport;
	}

	uri = uri + ";lr>";
	
	addRoute( uri );
}
