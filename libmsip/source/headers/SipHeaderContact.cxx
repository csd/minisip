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
 * 	SipHeaderValueContact.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#include<config.h>

#include<libmutil/itoa.h>

#include<libmsip/SipHeaderContact.h>

#include<libmsip/SipDialogConfig.h> //needed for the DEFAULT_SIPPROXY_EXPIRES_VALUE_SECONDS define ...

MRef<SipHeaderValue *> contactFactory(const string &build_from){
	                return new SipHeaderValueContact(build_from);
}

SipHeaderFactoryFuncPtr sipHeaderContactFactory=contactFactory;



const string sipHeaderValueContactTypeStr = "Contact";
		
SipHeaderValueContact::SipHeaderValueContact()
	: SipHeaderValue(SIP_HEADER_TYPE_CONTACT,sipHeaderValueContactTypeStr),uri("Erik","0.0.0.0","",0)
{
	featuretag= "";
	expires=DEFAULT_SIPPROXY_EXPIRES_VALUE_SECONDS;
}

SipHeaderValueContact::SipHeaderValueContact(const string &build_from) 
		: SipHeaderValue(SIP_HEADER_TYPE_CONTACT,sipHeaderValueContactTypeStr), uri("UNKNOWN","0.0.0.0","",0)
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
	expires=DEFAULT_SIPPROXY_EXPIRES_VALUE_SECONDS;
	//merr << "SipHeaderValueContact::getString: uri="<< uri <<end;
}

SipHeaderValueContact::SipHeaderValueContact(const string &username, 
		const string &ip, 
		int32_t port, 
		const string &user_type, 
		const string &transport,
		int expires
		)
			:SipHeaderValue(SIP_HEADER_TYPE_CONTACT,sipHeaderValueContactTypeStr),
			 uri(username,ip,user_type,port)
{
	if( expires != -1 )
		this->setExpires(expires);
	else this->expires = -1; //-1 indicates that the expires is not to be used
	if (transport!="")
		uri.setTransport(transport);
	//merr << "SipHeaderValueContact::getString: username="<< username <<end;
}

SipHeaderValueContact::~SipHeaderValueContact(){

}
		
string SipHeaderValueContact::getString(){
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
	
	string ret = /*"Contact: */"<"+user+">";
	if( this->expires != -1 )
		ret += ";" + featuretag + "expires=" + itoa(this->expires); //TODO: XXX
		
	//merr << "SipHeaderValueContact::getString: ret="<< ret<<end;
	return ret;
} 

SipURI SipHeaderValueContact::getUri(){
	return uri;
}

void SipHeaderValueContact::setUri(const SipURI &uri){
	this->uri=uri;
}

//CESC
int SipHeaderValueContact::getExpires() {
	return this->expires;
}

void SipHeaderValueContact::setExpires(int _expires){
	if( _expires >= 0 && _expires < 100000 ) 
		this->expires = _expires;
	else this->expires = DEFAULT_SIPPROXY_EXPIRES_VALUE_SECONDS;
}

