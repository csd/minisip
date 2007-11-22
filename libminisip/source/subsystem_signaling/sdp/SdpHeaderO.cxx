/*
 Copyright (C) 2004-2006 the Minisip Team
 
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
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */

/* Copyright (C) 2004 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

/* Name
 * 	SdpHeaderO.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#include<config.h>

#include<libminisip/signaling/sdp/SdpHeaderO.h>

#ifdef DEBUG_OUTPUT
#include<iostream>
#endif

using namespace std;

SdpHeaderO::SdpHeaderO(string buildFrom):SdpHeader(SDP_HEADER_TYPE_O, 2){
	size_t len = buildFrom.length();
	if (buildFrom.substr(0,2)!="o="){
#ifdef DEBUG_OUTPUT
		std::cerr << "ERROR: Origin sdp header is not starting with <o=>:"<<buildFrom<< std::endl;
#endif
	}
	size_t i=2;
	while ( buildFrom[i]==' ' && i<len )
		i++;
	
	username="";
	while ( buildFrom[i]!=' ' && i<len )
		username+=buildFrom[i++];

	while ( buildFrom[i]==' ' && i<len )
		i++;

	session_id="";
	while ( buildFrom[i]!=' ' && i<len )
		session_id+=buildFrom[i++];

	while ( buildFrom[i]==' ' && i<len )
		i++;
	
	version="";
	while ( buildFrom[i]!=' ' && i<len )
		version+=buildFrom[i++];

	while ( buildFrom[i]==' ' && i<len )
		i++;

	net_type="";
	while ( buildFrom[i]!=' ' && i<len )
		net_type+=buildFrom[i++];

	while ( buildFrom[i]==' ' && i<len )
		i++;

	addr_type="";
	while ( buildFrom[i]!=' ' && i<len )
		addr_type+=buildFrom[i++];

	while ( buildFrom[i]==' ' && i<len )
		i++;

	addr="";
	while ( buildFrom[i]!=' ' && i<len )
		addr+=buildFrom[i++];

}

SdpHeaderO::SdpHeaderO(string username, string session_id, string version, string net_type, string addr_type, string addr):SdpHeader(SDP_HEADER_TYPE_O, 2){
	this->username=username;
	this->session_id=session_id;
	this->version=version;
	this->net_type=net_type;
	this->addr_type = addr_type;
	this->addr=addr;
}

SdpHeaderO::~SdpHeaderO(){

}

string SdpHeaderO::getUsername(){
	return username;
}

void SdpHeaderO::setUsername(string username){
	this->username=username;
}

string SdpHeaderO::getSessionId(){
	return session_id;
}

void SdpHeaderO::setSessionId(string session_id){
	this->session_id=session_id;
}

string SdpHeaderO::getVersion(){
	return version;
}

void SdpHeaderO::setVersion(string version){
	this->version=version;
}

string SdpHeaderO::getNetType(){
	return net_type;
}

void SdpHeaderO::setNetType(string net_type){
	this->net_type=net_type;
}

string SdpHeaderO::getAddrType(){
	return addr_type;
}
void SdpHeaderO::setAddrType(string addr_type){
	this->addr_type=addr_type;
}

string SdpHeaderO::getAddr(){
	return addr;
}

void SdpHeaderO::setAddr(string addr){
	this->addr=addr;
}

string SdpHeaderO::getString(){
	string ret="o=";
	if (username=="")
		ret+="-";
	else
		ret+=username;
	ret+=" ";
	if (session_id=="")
		ret+="-";
	else
		ret+=session_id;
	ret+=" ";
	if (version=="")
		ret+="-";
	else
		ret+=version;
	ret+=" ";
	if (net_type=="")
		ret+="-";
	else
		ret+=net_type;
	ret+=" ";
	if (addr_type=="")
		ret+="-";
	else
		ret+=addr_type;
	ret+=" ";
	if (addr=="")
		ret+="-";
	else
		ret+=addr;
	return ret;
}


