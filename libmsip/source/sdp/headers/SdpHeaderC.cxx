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
 * 	SdpHeaderC.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#include<config.h>

#include<libmsip/SdpHeaderC.h>
#include<libmutil/itoa.h>
#include<iostream>

SdpHeaderC::SdpHeaderC(string buildFrom):SdpHeader(SDP_HEADER_TYPE_C, 4){
	if (buildFrom.substr(0,2)!="c="){
		std::cerr << "ERROR: Contact sdp header is not starting with <c=>"<< std::endl;
	}
	unsigned i=2;
	while (buildFrom[i]==' ')
		i++;

	netType="";
	while (buildFrom[i]!=' ')
		netType+=buildFrom[i++];

	while (buildFrom[i]==' ')
		i++;

	addrType="";
	while (buildFrom[i]!=' ')
		addrType+=buildFrom[i++];

	while (buildFrom[i]==' ')
		i++;

	addr="";
	while (buildFrom[i]!=' ' && !(i>=buildFrom.length()))
		addr+=buildFrom[i++];

}

SdpHeaderC::SdpHeaderC(string net_type, string addr_type, string addr):SdpHeader(SDP_HEADER_TYPE_C, 4){
	this->netType=net_type;
	this->addrType = addr_type;
	this->addr=addr;
}

string SdpHeaderC::getNetType(){
	return netType;
}

void SdpHeaderC::setNetType(string net_type){
	this->netType=net_type;
}

string SdpHeaderC::getAddrType(){
	return addrType;
}
void SdpHeaderC::setAddrType(string addr_type){
	this->addrType=addr_type;
}

string SdpHeaderC::getAddr(){
//	cerr << "Returning addr: "<< addr << endl;
	return addr;
}

void SdpHeaderC::setAddr(string addr){
	this->addr=addr;
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


