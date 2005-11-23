/*
  Copyright (C) 2005, 2004 Erik Eliasson, Johan Bilien
  
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

#include<libmutil/itoa.h>

MRef<SipHeaderValue *> viaFactory(const string &build_from){
	                return new SipHeaderValueVia(build_from);
}

SipHeaderFactoryFuncPtr sipHeaderViaFactory=viaFactory;


const string sipHeaderValueViaTypeStr = "Via";

SipHeaderValueVia::SipHeaderValueVia(const string &build_from)
		: SipHeaderValue(SIP_HEADER_TYPE_VIA,sipHeaderValueViaTypeStr)
{
	unsigned i=0;
	unsigned pos=0;

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


	while (!(build_from[i]==':' || i>=build_from.length())){
		ip+=build_from[i];
		i++;
	}

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

}

SipHeaderValueVia::SipHeaderValueVia()
		: SipHeaderValue(SIP_HEADER_TYPE_VIA,sipHeaderValueViaTypeStr)
{
	port=0;
	protocol="UDP";
	ip="UNKNOWN_IP";
}

SipHeaderValueVia::SipHeaderValueVia(const string &proto, const string &ip, int32_t port, const string &branch)
		: SipHeaderValue(SIP_HEADER_TYPE_VIA,sipHeaderValueViaTypeStr)
{
	setProtocol(proto);
	setIp(ip);
	setPort(port);
	setBranch(branch);
}

SipHeaderValueVia::~SipHeaderValueVia(){
}

string SipHeaderValueVia::getString(){
//	if (!(protocol=="TCP" || protocol=="UDP")){
//		cerr << "Protocol not set"<< endl;
//		throw string("Protocol not set in Via header");
//	}
///	if (port==0){
///		cerr << "ERROR: In SipHeaderValueVia: port not set"<< endl;
///		throw string("Port not set in Via header");
///	}
	string via;
	via = /*"Via: */ "SIP/2.0/"+protocol+" "+ip;
	if (port>0)
		via=via+":"+itoa(port);
	if (branch.length()>0)	
		via=via+";branch="+branch;	//FIXME: Parameters should not be added here - they 
	 					//shoud be set using the parameter functionality.
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

void SipHeaderValueVia::setBranch(const string &branch){
	this->branch = branch;
}
		
string SipHeaderValueVia::getBranch(){
	return branch;
}

