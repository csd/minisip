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
 * 	SipHeaderContentLength.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#include<config.h>


#include<libmsip/SipHeaderContentLength.h>

#include<libmutil/itoa.h>

SipHeaderContentLength::SipHeaderContentLength(const string &build_from)
		: SipHeader(SIP_HEADER_TYPE_CONTENTLENGTH)
{
	unsigned i=15;
	while (build_from[i]==' ')
		i++;
	string num="";
	while (build_from[i]>='0' && build_from[i]<='9' && !(i>=build_from.length())){
		num+=build_from[i];
		i++;
	}
	content_length=atoi(num.c_str());
}

SipHeaderContentLength::SipHeaderContentLength()
		: SipHeader(SIP_HEADER_TYPE_CONTENTLENGTH)
{
	content_length=0;
}

SipHeaderContentLength::SipHeaderContentLength(int32_t length)
		: SipHeader(SIP_HEADER_TYPE_CONTENTLENGTH)
{
	content_length=length;
}

SipHeaderContentLength::~SipHeaderContentLength(){
}

string SipHeaderContentLength::getString(){
	return "Content-Length: "+itoa(content_length);
}

int32_t SipHeaderContentLength::getContentLength(){
	return content_length;
}
		
void SipHeaderContentLength::setContentLength(int32_t content_length){
	this->content_length=content_length;
}

