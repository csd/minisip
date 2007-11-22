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
 * 	SdpHeaderC.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#include<config.h>

#include<libminisip/signaling/sdp/SdpHeaderC.h>
#include<libmnetutil/IPAddress.h>
#include<iostream>

using namespace std;

SdpHeaderC::SdpHeaderC(string buildFrom):SdpHeader(SDP_HEADER_TYPE_C, 4){
	size_t len = buildFrom.length();
	if (buildFrom.substr(0,2)!="c="){
		std::cerr << "ERROR: Contact sdp header is not starting with <c=>"<< std::endl;
	}
	size_t i=2;
	while ( buildFrom[i]==' ' && i<len )
		i++;

	netType="";
	while ( buildFrom[i]!=' ' && i<len )
		netType+=buildFrom[i++];

	while ( buildFrom[i]==' ' && i<len )
		i++;

	addrType="";
	while ( buildFrom[i]!=' ' && i<len )
		addrType+=buildFrom[i++];

	while ( buildFrom[i]==' ' && i<len )
		i++;

	addr="";
	while (buildFrom[i]!=' ' && i<len)
		addr+=buildFrom[i++];

}

SdpHeaderC::SdpHeaderC(string net_type, string addr_type, string addr_):SdpHeader(SDP_HEADER_TYPE_C, 4){
	this->netType=net_type;
	this->addrType = addr_type;
	this->addr=addr_;
}

SdpHeaderC::~SdpHeaderC(){

}

const string &SdpHeaderC::getNetType()const{
	return netType;
}

void SdpHeaderC::setNetType(string net_type){
	this->netType=net_type;
}

const string &SdpHeaderC::getAddrType()const{
	return addrType;
}
void SdpHeaderC::setAddrType(string addr_type){
	this->addrType=addr_type;
}

const string &SdpHeaderC::getAddr()const{
//	cerr << "Returning addr: "<< addr << endl;
	return addr;
}

void SdpHeaderC::setAddr(string a){
	this->addr=a;
}

string SdpHeaderC::getString(){
	string ret="c=";
	if (netType=="")
		ret+="-";
	else
		ret+=netType;
	ret+=" ";
	if (addrType=="")
		ret+="-";
	else
		ret+=addrType;
	ret+=" ";
	if (addr=="")
		ret+="-";
	else
		ret+=addr;
	return ret;
}

MRef<IPAddress*> SdpHeaderC::getIPAdress(){
	if( !ipAddr ){
		if( netType != "IN" )
			return NULL;

		if( addrType != "IP4" && addrType != "IP6" )
			return NULL;

		ipAddr = IPAddress::create( addr, addrType == "IP6" );
	}

	return ipAddr;
}
