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
 *	Cesc Santasusana, c e s c dot s a n t a A{T g m a i l dot co m; 2005
 * Purpose
 * 
*/

#include<config.h>

#include<libmutil/stringutils.h>
#include<libmsip/SipHeaderContact.h>
#include<libmsip/SipDialogConfig.h> //needed for the DEFAULT_SIPPROXY_EXPIRES_VALUE_SECONDS define ...

#include<stdlib.h>

using namespace std;

MRef<SipHeaderValue *> contactFactory(const string &build_from){
	return new SipHeaderValueContact(build_from);
}

SipHeaderFactoryFuncPtr sipHeaderContactFactory=contactFactory;



const string sipHeaderValueContactTypeStr = "Contact";
		
SipHeaderValueContact::SipHeaderValueContact(const string &build_from) 
		: SipHeaderValue(SIP_HEADER_TYPE_CONTACT,sipHeaderValueContactTypeStr),uri(build_from)
{
	if( !uri.isValid() ) {
		cerr << "SipHeaderValueContact::constructor - invalid contact uri '" << build_from << "'" << endl;
	}

	featuretag="";
	string tmp;
	tmp = getParameter("expires");
	if( tmp == "" ) {
		setExpires(DEFAULT_SIPPROXY_EXPIRES_VALUE_SECONDS);
	}
#ifdef DEBUG_OUTPUT	
// 	cerr << "SipHeaderValueContact::getString: uri="<< uri.getString() <<endl;
#endif
}

SipHeaderValueContact::SipHeaderValueContact(const SipUri &contactUri,
		int expires
		)
			:SipHeaderValue(SIP_HEADER_TYPE_CONTACT,sipHeaderValueContactTypeStr)
{
	uri = contactUri;
	
	if( expires != -1 )
		this->setExpires(expires);
	else this->removeParameter("expires" ); //-1 indicates that the expires is not to be used
	
	//merr << "SipHeaderValueContact::getString: username="<< username <<end;
}

SipHeaderValueContact::~SipHeaderValueContact(){

}
		
string SipHeaderValueContact::getString() const{
	string user = uri.getString();

//	merr << "SipHeaderValueContact::getString: "<< user <<end;
#if 0
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
#endif	
	string ret = /*"Contact: */ user;
	if( featuretag != "" )
		ret += ";" + featuretag;
		
/*
	string tmp;
	tmp = getParameter("expires");
	if( tmp != "" )
		ret += ";expires=" + tmp;

	tmp = getParameter("transport");
	if( tmp != "" )
		ret += ";transport=" + tmp;
*/
		
	//merr << "SipHeaderValueContact::getString: ret="<< ret<<end;
	return ret;
} 

const SipUri &SipHeaderValueContact::getUri() const{
	return uri;
}

void SipHeaderValueContact::setUri(const SipUri &u){
	this->uri=u;
}

//CESC
int SipHeaderValueContact::getExpires() const{
	return atoi( getParameter("expires").c_str() );
}

void SipHeaderValueContact::setExpires(int expires){
	if( ! (expires >= 0 && expires < 100000) )
		expires = DEFAULT_SIPPROXY_EXPIRES_VALUE_SECONDS;
	this->setParameter( "expires", itoa(expires) );
}

