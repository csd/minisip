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
 * 	SipHeaderValueFrom.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#include<config.h>


#include<libmsip/SipHeaderFrom.h>
#include<libmutil/trim.h>

const string sipHeaderValueFrom = "From";
		
SipHeaderValueFrom::SipHeaderValueFrom()
		: SipHeaderValue(SIP_HEADER_TYPE_FROM,sipHeaderValueFrom),
		uri("Erik","0.0.0.0","phone",0)
{
	tag="";
}
		

SipHeaderValueFrom::SipHeaderValueFrom(const string &build_from) 
		: SipHeaderValue(SIP_HEADER_TYPE_FROM,sipHeaderValueFrom), 
		uri("UNKNOWN","0.0.0.0","phone",0)
{
	
	string users_name;
	unsigned i=0;
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

SipHeaderValueFrom::SipHeaderValueFrom(const string &username, const string &ip)
		: SipHeaderValue(SIP_HEADER_TYPE_FROM,sipHeaderValueFrom),
		uri(username,ip,"phone",0)
{
}

SipHeaderValueFrom::~SipHeaderValueFrom(){

}
		
string SipHeaderValueFrom::getString(){
//	merr << "SipHeaderValueFrom: Runnign get_string"<< end;
	string ret = /*"From: "+*/uri.getUsersName()+"<"+uri.getString()+">";
//	merr << "SipHeaderValueFrom: Runnign get_string(2)"<< end;
	if (tag.length()>0)
		ret=ret+";tag="+tag;
//	merr << "SipHeaderValueFrom: returning string: "<< ret<< end;
	return ret;
} 

SipURI &SipHeaderValueFrom::getUri(){
	return uri;
}

void SipHeaderValueFrom::setUri(const SipURI &uri){
	this->uri=uri;
}
		
void SipHeaderValueFrom::setTag(const string &tag){
	this->tag = tag;
}

string SipHeaderValueFrom::getTag(){
	return tag;
}
