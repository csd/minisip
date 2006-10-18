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
 * 	SipHeaderValueRoute.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#include<config.h>

#include<list>
#include<libmsip/SipHeaderRoute.h>

#include<libmutil/stringutils.h>

using namespace std;

// Ex: Route: <sip:vatn@213.100.38.57;ftag=2064763305;lr>,<...>

MRef<SipHeaderValue *> routeFactory(const string &build_from){
	                return new SipHeaderValueRoute(build_from);
}

SipHeaderFactoryFuncPtr sipHeaderRouteFactory=routeFactory;



const string sipHeaderValueRouteTypeStr = "Route";

SipHeaderValueRoute::SipHeaderValueRoute(const string &build_from)
		: SipHeaderValue(SIP_HEADER_TYPE_ROUTE,sipHeaderValueRouteTypeStr)
{
	route = trim(build_from);
}

//FIXME: This is against what a header value should contains.
//This implementation puts several header values into a header
//value (each comma separated value should be a header value by
//it self - this is how it will be represented when it is 
//received).
SipHeaderValueRoute::SipHeaderValueRoute(list<string> &routeSet)
		: SipHeaderValue(SIP_HEADER_TYPE_ROUTE,sipHeaderValueRouteTypeStr)
{
	for (list<string>::iterator i=routeSet.begin(); i!=routeSet.end(); i++){
		#ifdef DEBUG_OUTPUT
		cerr << "SipHeaderRoute:: route " << *i << endl;
		#endif
		if (i!=routeSet.begin())
			route = route+',';
		route = route + (*i);
	}
}

SipHeaderValueRoute::~SipHeaderValueRoute(){
}

string SipHeaderValueRoute::getString() const{
	return route;
}

string SipHeaderValueRoute::getRoute() const{
	return route;
}

