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
 * 	SipHeaderValueContact.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#include<config.h>

#include<libmsip/SipHeaderContact.h>

const string sipHeaderValueContactTypeStr = "Contact";
		
SipHeaderValueContact::SipHeaderValueContact()
	: SipHeaderValue(SIP_HEADER_TYPE_CONTACT,sipHeaderValueContactTypeStr),uri("Erik","0.0.0.0","phone",0)
{
	featuretag= "";	
}

SipHeaderValueContact::SipHeaderValueContact(const string &build_from) 
		: SipHeaderValue(SIP_HEADER_TYPE_CONTACT,sipHeaderValueContactTypeStr), uri("UNKNOWN","0.0.0.0","phone",0)
{
	unsigned i=0;
	while (build_from[i]!='<')
		i++;

	i++;
	string uri="";
	while (!(build_from[i]=='>' || i>= build_from.length())){
		uri+=build_from[i];
		i++;
	}

	SipURI u(uri);
	setUri(u);
	
	featuretag="";
}

SipHeaderValueContact::SipHeaderValueContact(const string &username, 
		const string &ip, 
		int32_t port, 
		const string &user_type, 
		const string &transport
		)
			:SipHeaderValue(SIP_HEADER_TYPE_CONTACT,sipHeaderValueContactTypeStr),
			 uri(username,ip,user_type,port)
{
	if (transport!="")
		uri.setTransport(transport);
}

SipHeaderValueContact::~SipHeaderValueContact(){

}
		
string SipHeaderValueContact::getString(){
//	merr << "SipHeaderValueContact::getString"<<end;

//	return "Contact: <+erik@debug.org>;expires=1000"; //TODO: XXX
	
	string user = uri.getString();
//	merr << "SipHeaderValueContact::getString: "<< user <<end;
	string name;
	if (user.find("@")!=string::npos){
		name = "";
		unsigned i=0;
		while (user[i]!='@')
			name=name + user[i++];
		while (user[i]!=':' && user[i]!=';')
			i++;
		string args="";
		while (user[i]!='\0' && i<user.length())
			args=args+user[i++];
		
		user = name+"@"+uri.getIp()+args;
	}
//	merr << "SipHeaderValueContact::getString: creating ret" <<end;
	
	string ret = /*"Contact: */"<"+user+">;" + featuretag + "expires=1000"; //TODO: XXX
//	merr << "SipHeaderValueContact::getString: ret="<< ret<<end;
	return ret;
} 

SipURI SipHeaderValueContact::getUri(){
	return uri;
}

void SipHeaderValueContact::setUri(const SipURI &uri){
	this->uri=uri;
}

