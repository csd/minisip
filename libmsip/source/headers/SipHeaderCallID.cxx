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
 * 	SipHeaderCallID.cxx
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


SipHeaderCallID::SipHeaderCallID(const string &build_from)
	: SipHeader(SIP_HEADER_TYPE_CALLID)
{
	unsigned i=9;
	while (!(i>=build_from.length())){
		id+=build_from[i];
		i++;
	}
	id=trim(id);
}

SipHeaderCallID::SipHeaderCallID()
	: SipHeader(SIP_HEADER_TYPE_CALLID)
{
	id="NOT_SET";
}

SipHeaderCallID::~SipHeaderCallID(){
}

string SipHeaderCallID::getString(){
	return "Call-ID: "+id;
}

string SipHeaderCallID::getId(){
	return id;
}
		
void SipHeaderCallID::setId(const string &id){
	this->id=id;
}

void SipHeaderCallID::generateId(){
#ifdef DEBUG_OUTPUT
	cerr<<"TODO: running not implemented function (SipHeaderCallID::generate_id)"<< endl;
#endif
	this->id = string("RANDOM_NUMBER")+"@"+"LOCAL_IP";
}

