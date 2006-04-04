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
 * 	SipHeaderValueProxyAuthenticate.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#include<config.h>

#include<libmutil/trim.h>
#include<libmsip/SipHeaderProxyAuthenticate.h>

using namespace std;

MRef<SipHeaderValue *> proxyauthFactory(const string &build_from){
	                return new SipHeaderValueProxyAuthenticate(build_from);
}

SipHeaderFactoryFuncPtr sipHeaderProxyAuthenticateFactory=proxyauthFactory;


const string sipHeaderValueProxyAuthenticationTypeStr = "Proxy-Authenticate";
		
SipHeaderValueProxyAuthenticate::SipHeaderValueProxyAuthenticate(int type, string typeStr, const string &build_from)
		: SipHeaderValue(type, typeStr)
{
	init(build_from);
}

SipHeaderValueProxyAuthenticate::SipHeaderValueProxyAuthenticate(const string &build_from)
		: SipHeaderValue(SIP_HEADER_TYPE_PROXYAUTHENTICATE,sipHeaderValueProxyAuthenticationTypeStr)
{
	init(build_from);
}

void SipHeaderValueProxyAuthenticate::init(const string &build_from){
	string line = trim(build_from);
	if (line.substr(0,6)=="Digest"){
		hasDigest=true;
		line = trim(line.substr(6));
	}else{
		hasDigest=false;
	}

	size_t pos = line.find("=");
	if (pos==string::npos){
		cerr << "Warning: could not find key-value parir in authenticate header"<<endl;
	}else{
		property = trim(line.substr(0,pos));
		value = trim(line.substr(pos+1));
		if (value.size()>=2 && value[0]=='\"' && value[value.size()-1]=='\"'){
			value = trim(value.substr(1,value.size()-2));
			hasQuotes=true;
		}else{
			hasQuotes=false;
		}
	}
}

SipHeaderValueProxyAuthenticate::~SipHeaderValueProxyAuthenticate(){

}
		
string SipHeaderValueProxyAuthenticate::getString(){
	string ret;
	if (hasDigest)
		ret+="Digest ";
	string v;
	if (hasQuotes)
		v="\""+value+"\"";
	else
		v=value;
	ret=ret+property+"="+v;
	return ret;
} 


