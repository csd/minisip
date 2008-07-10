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
 * 	SipHeaderValueExpires.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#include<config.h>


#include<libmsip/SipHeaderExpires.h>

#include<libmutil/stringutils.h>

#include<stdlib.h>

using namespace std;

MRef<SipHeaderValue *> expiresFactory(const string &build_from){
	                return new SipHeaderValueExpires(build_from);
}

SipHeaderFactoryFuncPtr sipHeaderExpiresFactory=expiresFactory;



const string sipHeaderValueExpiresTypeStr = "Expires";

SipHeaderValueExpires::SipHeaderValueExpires(const string &build_from)
		: SipHeaderValue(SIP_HEADER_TYPE_EXPIRES,sipHeaderValueExpiresTypeStr)
{
	string tmp = trim(build_from);
	char *endptr;
	const char *s = tmp.c_str();
	int n = strtol(s, &endptr, 10);
	if (*endptr==0){
		timeout=n;
	}else{
		merr << "WARNING: Could not parse Expires header - setting to 300 instead"<<endl;
		timeout=300;
	}
}

SipHeaderValueExpires::SipHeaderValueExpires(int sec)
		: SipHeaderValue(SIP_HEADER_TYPE_EXPIRES,sipHeaderValueExpiresTypeStr)
{
	timeout=sec;
}

SipHeaderValueExpires::~SipHeaderValueExpires(){
}

string SipHeaderValueExpires::getString() const{
	return itoa(timeout);
}

int32_t SipHeaderValueExpires::getTimeout() const{
	return timeout;
}
		
/*
void SipHeaderValueExpires::setTimeout(int32_t timeout){
	this->timeout=timeout;
}
*/
