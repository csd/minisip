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
 * 	SipHeaderValueCallID.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#include<config.h>

#include<libmsip/SipHeaderCallID.h>

#include<libmutil/itoa.h>
#include<libmutil/trim.h>

#ifdef DEBUG_OUTPUT
#include<iostream>
#endif

MRef<SipHeaderValue *> callidFactory(const string &build_from){
	                return new SipHeaderValueCallID(build_from);
}

SipHeaderFactoryFuncPtr sipHeaderCallIdFactory=callidFactory;


const string sipHeaderValueCallIdTypeStr = "Call-ID";

SipHeaderValueCallID::SipHeaderValueCallID(const string &build_from)
	: SipHeaderValue(SIP_HEADER_TYPE_CALLID,sipHeaderValueCallIdTypeStr)
{
	unsigned i=0;
	while (!(i>=build_from.length())){
		id+=build_from[i];
		i++;
	}
	id=trim(id);
}

SipHeaderValueCallID::SipHeaderValueCallID()
	: SipHeaderValue(SIP_HEADER_TYPE_CALLID,sipHeaderValueCallIdTypeStr)
{
	id="NOT_SET";
}

SipHeaderValueCallID::~SipHeaderValueCallID(){
}

string SipHeaderValueCallID::getString(){
	return /*"Call-ID: "+*/id;
}

string SipHeaderValueCallID::getId(){
	return id;
}
		
void SipHeaderValueCallID::setId(const string &id){
	this->id=id;
}

void SipHeaderValueCallID::generateId(){
#ifdef DEBUG_OUTPUT
	cerr<<"TODO: running not implemented function (SipHeaderValueCallID::generate_id)"<< endl;
#endif
	this->id = string("RANDOM_NUMBER")+"@"+"LOCAL_IP";
}

