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
 * 	SdpHeaderA.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#include<config.h>

#include<libmutil/massert.h>
#include<libminisip/sdp/SdpHeaderA.h>

#include<libmutil/itoa.h>
#include<libmutil/trim.h>
#include<iostream>

using namespace std;

SdpHeaderA::SdpHeaderA(string buildFrom) : SdpHeader(SDP_HEADER_TYPE_A, 9){
	massert(buildFrom.substr(0,2)=="a=");
	attributes= trim(buildFrom.substr(2, buildFrom.length()-2));
}

SdpHeaderA::~SdpHeaderA(){

}

string SdpHeaderA::getAttributes(){
	return attributes;
}
void SdpHeaderA::setAttributes(string attr){
	this->attributes= attr;
}

string SdpHeaderA::getString(){
	return "a="+attributes;
}

string SdpHeaderA::getAttributeType(){
	uint32_t pos;
	
	if(( pos = attributes.find( ":" )) == string::npos )
		return "property";
	return attributes.substr( 0, pos );
}

string SdpHeaderA::getAttributeValue(){
	if( getAttributeType() == "property" )
		return attributes;
	uint32_t pos = attributes.find( ":" );
	if( attributes.length() <= pos + 1 )
	{
		cerr << "Invalid a field in SDP packet" << endl;
		return "";
	}
	return attributes.substr( pos + 1, attributes.length() - 2 );
}

