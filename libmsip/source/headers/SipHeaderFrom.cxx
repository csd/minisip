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
 * 	SipHeaderFrom.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#include<config.h>


#include<libmsip/SipHeaderFrom.h>
#include<libmutil/trim.h>

		
SipHeaderFrom::SipHeaderFrom()
		: SipHeader(SIP_HEADER_TYPE_FROM),
		uri("Erik","0.0.0.0","phone",0)
{
	tag="";
}
		

SipHeaderFrom::SipHeaderFrom(const string &build_from) 
		: SipHeader(SIP_HEADER_TYPE_FROM), 
		uri("UNKNOWN","0.0.0.0","phone",0)
{
	string users_name;
	unsigned i=5;
	while (build_from[i]!='<'){
		users_name+=build_from[i];
		i++;
	}

	i++; //go past '<'

	string uri_str;
	while (build_from[i]!='>'){
		uri_str+=build_from[i];
		i++;
	}
	uri=SipURI(uri_str);
	uri.setUsersName(trim(users_name));

	tag="";

	while (build_from[i]=='>' || build_from[i]==' ')
		i++;
	
	if (build_from.length()>=i+5 && build_from.substr(i,5)==";tag="){
		tag = build_from.substr(i+5, build_from.length()-5-1);	
	}
}

SipHeaderFrom::SipHeaderFrom(const string &username, const string &ip)
		: SipHeader(SIP_HEADER_TYPE_FROM),
		uri(username,ip,"phone",0)
{
}

SipHeaderFrom::~SipHeaderFrom(){

}
		
string SipHeaderFrom::getString(){
//	merr << "SipHeaderFrom: Runnign get_string"<< end;
	string ret = "From: "+uri.getUsersName()+"<"+uri.getString()+">";
//	merr << "SipHeaderFrom: Runnign get_string(2)"<< end;
	if (tag.length()>0)
		ret=ret+";tag="+tag;
//	merr << "SipHeaderFrom: returning string: "<< ret<< end;
	return ret;
} 

SipURI &SipHeaderFrom::getUri(){
	return uri;
}

void SipHeaderFrom::setUri(const SipURI &uri){
	this->uri=uri;
}
		
void SipHeaderFrom::setTag(const string &tag){
	this->tag = tag;
}

string SipHeaderFrom::getTag(){
	return tag;
}
