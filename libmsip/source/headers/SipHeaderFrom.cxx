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

MRef<SipHeaderValue *> fromFactory(const string &build_from){
	                return new SipHeaderValueFrom(build_from);
}

SipHeaderFactoryFuncPtr sipHeaderFromFactory=fromFactory;

const string sipHeaderValueFrom = "From";
		
SipHeaderValueFrom::SipHeaderValueFrom()
		: SipHeaderValue(SIP_HEADER_TYPE_FROM,sipHeaderValueFrom),
		uri("Erik","0.0.0.0","",0)
{
	//tag="";
}
		

SipHeaderValueFrom::SipHeaderValueFrom(const string &build_from) 
		: SipHeaderValue(SIP_HEADER_TYPE_FROM,sipHeaderValueFrom), 
		uri("UNKNOWN","0.0.0.0","",0)
{
	
        size_t ltPos = build_from.find( '<' );
        size_t gtPos = build_from.find( '>' );

        if( ltPos != string::npos && gtPos != string::npos && ltPos + 1 < gtPos ){
                // Assume Username <uri> scheme
                uri = SipURI( build_from.substr( ltPos + 1, gtPos - ltPos - 1 ) );
                uri.setUsersName( trim( build_from.substr( 0, ltPos ) ) );
        }
        else{
                // uri scheme (without Username)
                uri = SipURI( build_from );
                uri.setUsersName( "" );
        }
}

SipHeaderValueFrom::SipHeaderValueFrom(const string &username, const string &ip)
		: SipHeaderValue(SIP_HEADER_TYPE_FROM,sipHeaderValueFrom),
		uri(username,ip,"",0)
{
}

SipHeaderValueFrom::~SipHeaderValueFrom(){

}
		
string SipHeaderValueFrom::getString(){
//	merr << "SipHeaderValueFrom: Runnign get_string"<< end;
	string ret = /*"From: "+*/uri.getUsersName()+"<"+uri.getString()+">";
//	merr << "SipHeaderValueFrom: Runnign get_string(2)"<< end;
//	if (tag.length()>0)
//		ret=ret+";tag="+tag;
//	merr << "SipHeaderValueFrom: returning string: "<< ret<< end;
	return ret;
} 

SipURI &SipHeaderValueFrom::getUri(){
	return uri;
}

void SipHeaderValueFrom::setUri(const SipURI &uri){
	this->uri=uri;
}
		
/*
void SipHeaderValueFrom::setTag(const string &tag){
	this->tag = tag;
}

string SipHeaderValueFrom::getTag(){
	return tag;
}
*/
