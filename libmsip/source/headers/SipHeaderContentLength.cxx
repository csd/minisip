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
 * 	SipHeaderValueContentLength.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#include<config.h>


#include<libmsip/SipHeaderContentLength.h>

#include<libmutil/itoa.h>

const string sipHeaderValueContentLengthTypeStr = "Content-Length";

SipHeaderValueContentLength::SipHeaderValueContentLength(const string &build_from)
		: SipHeaderValue(SIP_HEADER_TYPE_CONTENTLENGTH,sipHeaderValueContentLengthTypeStr)
{
	unsigned i=0;
	while (build_from[i]==' ')
		i++;
	string num="";
	while (build_from[i]>='0' && build_from[i]<='9' && !(i>=build_from.length())){
		num+=build_from[i];
		i++;
	}
	content_length=atoi(num.c_str());
}

SipHeaderValueContentLength::SipHeaderValueContentLength()
		: SipHeaderValue(SIP_HEADER_TYPE_CONTENTLENGTH,sipHeaderValueContentLengthTypeStr)
{
	content_length=0;
}

SipHeaderValueContentLength::SipHeaderValueContentLength(int32_t length)
		: SipHeaderValue(SIP_HEADER_TYPE_CONTENTLENGTH,sipHeaderValueContentLengthTypeStr)
{
	content_length=length;
}

SipHeaderValueContentLength::~SipHeaderValueContentLength(){
}

string SipHeaderValueContentLength::getString(){
	return /*"Content-Length: "+*/itoa(content_length);
}

int32_t SipHeaderValueContentLength::getContentLength(){
	return content_length;
}
		
void SipHeaderValueContentLength::setContentLength(int32_t content_length){
	this->content_length=content_length;
}

