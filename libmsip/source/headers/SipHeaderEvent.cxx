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
 * 	SipHeaderEvent.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#include<config.h>


#include<assert.h>
#include<libmsip/SipHeaderEvent.h>

#include<libmutil/itoa.h>
#include<libmutil/trim.h>

#ifdef DEBUG_OUTPUT
#include<iostream>
#endif

// Ex: Via: SIP/2.0/UDP 10.0.0.2:5043

SipHeaderEvent::SipHeaderEvent(const string &build_from)
		: SipHeader(SIP_HEADER_TYPE_EVENT)
{
	assert(build_from.size()>6);
	event= trim(build_from.substr(5));
#ifdef DEBUG_OUTPUT
	cerr << "DEBUG: parsed Event to: "<< event << endl;;
#endif
}

SipHeaderEvent::SipHeaderEvent()
		: SipHeader(SIP_HEADER_TYPE_EVENT)
{
	event="NOT_SET";
}

SipHeaderEvent::~SipHeaderEvent(){
}

string SipHeaderEvent::getString(){
	return "Event: "+event;
}

string SipHeaderEvent::getEvent(){
	return event;
}
		
void SipHeaderEvent::setEvent(const string &event){
	this->event=event;
}

