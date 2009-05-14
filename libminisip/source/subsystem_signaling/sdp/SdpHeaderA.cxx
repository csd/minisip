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
#include<libminisip/signaling/sdp/SdpHeaderA.h>

#include<iostream>
#include<string.h>

using namespace std;

SdpHeaderA::SdpHeaderA(string buildFrom) : SdpHeader(SDP_HEADER_TYPE_A, 10){
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
	size_t pos;
	
	if(( pos = attributes.find( ":" )) == string::npos )
		return "property";
	return attributes.substr( 0, pos );
}

void SdpHeaderA::getAttFromFileSelector(){
	size_t pos;
	size_t pos2;
	
	name = false;
	type = false;
	size = false;
	hash = false;
	
	pos=14;
	pos2=14;
	
	int searchname;
	int searchtype;
	int searchsize;
	int poshash;
	int iter;
	int counter=0;

	for(iter=15; iter<(int)strlen(attributes.c_str()); iter++){
		if(attributes[iter] == ':')
			counter++;
	}
	//cerr<<counter<<endl;

	while(counter!=1){
		
		counter --;
		pos2 = attributes.find( ":",pos);
		
		//cerr<<pos2<<endl;
		
		pos= pos2-4;

		//cerr<<( attributes.substr(pos, 4) )<<endl;

		if( attributes.substr(pos, 4) == "name"){
			name = true;
			searchname = (int)attributes.find('"',pos2+2);
			//cerr<<searchname<<endl;
			filename = ( attributes.substr(pos2+2,(searchname-pos2-2)) );
			//cerr<<filename<<endl;
		}	
		if( attributes.substr(pos, 4) == "type"){
			type = true;
			searchtype = (int)attributes.find(" ",pos2);
			//cerr<<searchtype<<endl;
			filetype = ( attributes.substr(pos2+1,(searchtype-pos2)) );
			//cerr<<filetype<<endl;
		}
		if( attributes.substr(pos, 4) == "size"){
			size = true;
			searchsize = (int)attributes.find(" ", pos2);
			//cerr<<searchsize<<endl;
			filesizes = ( attributes.substr(pos2+1,(searchsize-pos2)) );
			//cerr<<filesizes<<endl;
		}	
		if( attributes.substr(pos, 4) == "hash"){
			hash = true;
			poshash = (int)attributes.find( ":", pos2+1);
			hashused = ( attributes.substr(pos2+1,poshash-pos2-1));
			hashforfile = ( attributes.substr(poshash+1,(strlen(attributes.c_str())-poshash)) );
			//cerr<<"used hash "<<hashused<<endl;
			//cerr<<"hash "<<hashforfile<<endl;
		}
	
		pos2++;
		pos = pos2;
	}
}


string SdpHeaderA::getAttributeValue(){
	if( getAttributeType() == "property" )
		return attributes;
	uint32_t pos = (int)attributes.find( ":" );
	if( attributes.length() <= pos + 1 )
	{
		cerr << "Invalid a field in SDP packet" << endl;
		return "";
	}
	return attributes.substr( pos + 1, attributes.length() - 2 );
}


