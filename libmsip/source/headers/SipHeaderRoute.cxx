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
 * 	SipHeaderRoute.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#include<config.h>

#include<assert.h>
#include<list>
#include<libmsip/SipHeaderRoute.h>

#include<libmutil/itoa.h>
#include<libmutil/trim.h>

// Ex: Route: <sip:vatn@213.100.38.57;ftag=2064763305;lr>,<...>

SipHeaderRoute::SipHeaderRoute(const string &build_from)
		: SipHeader(SIP_HEADER_TYPE_ROUTE)
{
	assert(build_from.size()>6);
	route = trim(build_from.substr(6));
}

SipHeaderRoute::SipHeaderRoute(list<string> &routeSet)
		: SipHeader(SIP_HEADER_TYPE_ROUTE)
{
	for (list<string>::iterator i=routeSet.begin(); i!=routeSet.end(); i++){
		if (i!=routeSet.begin())
			route = route+',';
		route = route + (*i);
	}
}

SipHeaderRoute::~SipHeaderRoute(){
}

string SipHeaderRoute::getString(){
	return "Route: "+route;
}

string SipHeaderRoute::getRoute(){
	return route;
}

