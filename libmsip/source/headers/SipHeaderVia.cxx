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
 * 	SipHeaderVia.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#include<config.h>

#include<libmsip/SipHeaderVia.h>

#include<libmutil/itoa.h>

// Ex: Via: SIP/2.0/UDP 10.0.0.2:5043

SipHeaderVia::SipHeaderVia(const string &build_from)
		: SipHeader(SIP_HEADER_TYPE_VIA)
{
	unsigned i=17;
	ip="";
	port=0;
	while (build_from[i]==' ')
		i++;

	while (!(build_from[i]==':' || i>=build_from.length())){
		ip+=build_from[i];
		i++;
	}

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
	protocol = string("")+build_from[13]+build_from[14]+build_from[15];

}

SipHeaderVia::SipHeaderVia()
		: SipHeader(SIP_HEADER_TYPE_VIA)
{
	port=0;
	protocol="UDP";
	ip="UNKNOWN_IP";
}

SipHeaderVia::SipHeaderVia(const string &proto, const string &ip, int32_t port, const string &branch)
		: SipHeader(SIP_HEADER_TYPE_VIA)
{
	setProtocol(proto);
	setIp(ip);
	setPort(port);
	setBranch(branch);
}

SipHeaderVia::~SipHeaderVia(){
}

string SipHeaderVia::getString(){
//	if (!(protocol=="TCP" || protocol=="UDP")){
//		cerr << "Protocol not set"<< endl;
//		throw new string("Protocol not set in Via header");
//	}
///	if (port==0){
///		cerr << "ERROR: In SipHeaderVia: port not set"<< endl;
///		throw new string("Port not set in Via header");
///	}
	string via;
	via = "Via: SIP/2.0/"+protocol+" "+ip;
	if (port>0)
		via=via+":"+itoa(port);
	if (branch.length()>0)
		via=via+";branch="+branch;
	return via;
}

string SipHeaderVia::getProtocol(){
	return protocol;
}

void SipHeaderVia::setProtocol(const string &protocol){
	this->protocol=protocol;
}
		
string SipHeaderVia::getIp(){
	return ip;
}
		
void SipHeaderVia::setIp(const string &ip){
	this->ip=ip;
}

int32_t SipHeaderVia::getPort(){
	return port;
}

void SipHeaderVia::setPort(int32_t p){
	this->port=p;
}

void SipHeaderVia::setBranch(const string &branch){
	this->branch = branch;
}
		
string SipHeaderVia::getBranch(){
	return branch;
}

