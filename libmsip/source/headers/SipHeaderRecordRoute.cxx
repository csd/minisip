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
 * 	SipHeaderRecordRoute.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#include<config.h>

#include<assert.h>
#include<libmsip/SipHeaderRecordRoute.h>

#include<libmutil/itoa.h>
#include<libmutil/trim.h>

#ifdef DEBUG_OUTPUT
#include<iostream>
#endif

// Ex: Record-Route: <sip:vatn@213.100.38.57;ftag=2064763305;lr>,<...>

SipHeaderRecordRoute::SipHeaderRecordRoute(const string &build_from)
		: SipHeader(SIP_HEADER_TYPE_RECORDROUTE)
{
	assert(build_from.size()>13);
	route = trim(build_from.substr(13));
#ifdef DEBUG_OUTPUT
	cerr << "DEBUG: parsed route to: "<< route << endl;;
#endif
}

SipHeaderRecordRoute::~SipHeaderRecordRoute(){
}

string SipHeaderRecordRoute::getString(){
	return "Record-Route: "+route;
}

string SipHeaderRecordRoute::getRoute(){
	return route;
}

