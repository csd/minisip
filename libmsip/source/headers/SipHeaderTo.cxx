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
 * 	SipHeaderTo.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#include<config.h>

#include<libmsip/SipHeaderTo.h>
#include<libmutil/trim.h>

		
SipHeaderTo::SipHeaderTo()
		: SipHeader(SIP_HEADER_TYPE_TO),
		uri("Erik","0.0.0.0","phone",0)
{
	
}
		

SipHeaderTo::SipHeaderTo(const string &build_from) 
		: SipHeader(SIP_HEADER_TYPE_TO),
		uri("UNKNOWN","0.0.0.0","phone",0)
{
	string users_name;
	int32_t i=3;
	tag="";
	while (build_from[i]!='<'){ 		//FIX not past eos
		users_name+=build_from[i];
		i++;
	}

	i++; //go past '<'

	string uri_str="";
	while (build_from[i]!='>'){		//FIXME: do not go past eos
		uri_str+=build_from[i];
		i++;
	}
	i++;
	
	if (build_from.length()-1>(unsigned)i){
		if (build_from.substr(i,5)==string(";tag=")){
			tag = build_from.substr(i+5, build_from.length()-i-5);
			
		}else{
		
		}
			
	}
	uri=SipURI(uri_str);
	uri.setUsersName(trim(users_name));
}


SipHeaderTo::SipHeaderTo(const string &username, const string &ip)
		: SipHeader(SIP_HEADER_TYPE_TO),
		uri(username, ip,"phone",0)
{

}

SipHeaderTo::~SipHeaderTo(){

}

string SipHeaderTo::getString(){
	//cerr << "To::get_string" << endl;
	string ret = "To: "+uri.getUsersName()+"<"+uri.getString()+">";
	if (tag.length()>0)
		ret=ret+";tag="+tag;
	
	return ret;
} 

SipURI &SipHeaderTo::getUri(){
	return uri;
}

void SipHeaderTo::setUri(const SipURI &uri){
	this->uri=uri;
}
		
void SipHeaderTo::setTag(const string &tag){
	this->tag=tag;
}

string SipHeaderTo::getTag(){
	return tag;
}

