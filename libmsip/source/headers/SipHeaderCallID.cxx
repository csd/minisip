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

