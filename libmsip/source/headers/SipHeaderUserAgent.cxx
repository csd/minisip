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
 * 	SipHeaderValueUserAgent.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#include<config.h>


#include<libmsip/SipHeaderUserAgent.h>

#include<libmutil/itoa.h>

const string sipHeaderValueTypeUserAgentTypeStr = "User-Agent";

SipHeaderValueUserAgent::SipHeaderValueUserAgent(const string &build_from)
		: SipHeaderValue(SIP_HEADER_TYPE_USERAGENT,sipHeaderValueTypeUserAgentTypeStr)
{
	user_agent=build_from;
}

SipHeaderValueUserAgent::SipHeaderValueUserAgent()
		: SipHeaderValue(SIP_HEADER_TYPE_USERAGENT,sipHeaderValueTypeUserAgentTypeStr)
{
	user_agent="NOT_SET";
}

SipHeaderValueUserAgent::~SipHeaderValueUserAgent(){

}

string SipHeaderValueUserAgent::getString(){
	return /*"User-Agent: "+*/ user_agent;
}

string SipHeaderValueUserAgent::getUserAgent(){
	return user_agent;
}
		
void SipHeaderValueUserAgent::setUserAgent(const string &ua){
	this->user_agent=ua;
}

