/*
  Copyright (C) 2005, 2004 Erik Eliasson, Johan Bilien
  Copyright (C) 2006 Mikael Magnusson
  
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
 * 	SipHeaderValueSubscriptionState.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * 	Mikael Magnusson <mikma@users.sourceforge.net>
 * Purpose
 * 
*/

#include<config.h>

#include<libmsip/SipHeaderSubscriptionState.h>

#include<libmutil/stringutils.h>

#ifdef DEBUG_OUTPUT
#include<iostream>
#endif

using namespace std;

MRef<SipHeaderValue *> subscriptionStateFactory(const string &build_from){
	                return new SipHeaderValueSubscriptionState(build_from);
}

SipHeaderFactoryFuncPtr sipHeaderSubscriptionStateFactory=subscriptionStateFactory;


const string sipHeaderValueSubscriptionStateTypeStr = "Subscription-State";

SipHeaderValueSubscriptionState::SipHeaderValueSubscriptionState(const string &build_from)
		: SipHeaderValueString(SIP_HEADER_TYPE_SUBSCRIPTIONSTATE,sipHeaderValueSubscriptionStateTypeStr,build_from)
{
}

