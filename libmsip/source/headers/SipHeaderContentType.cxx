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
 * 	SipHeaderContentType.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#include<config.h>


#include<libmsip/SipHeaderContentType.h>

#include<libmutil/itoa.h>
#include<libmutil/trim.h>

#include<iostream>
using namespace std;

// Ex: Via: SIP/2.0/UDP 10.0.0.2:5043

SipHeaderContentType::SipHeaderContentType(const string &build_from)
		: SipHeader(SIP_HEADER_TYPE_CONTENTTYPE)
{
	content_type=trim(build_from.substr(13)); //strlen("Content-Type:")
}

SipHeaderContentType::SipHeaderContentType()
		: SipHeader(SIP_HEADER_TYPE_CONTENTTYPE)
{
	content_type="NOT_SET";
}

SipHeaderContentType::~SipHeaderContentType(){
}

string SipHeaderContentType::getString(){
	return "Content-Type: "+content_type;
}

string SipHeaderContentType::getContentType(){
	return content_type;
}
		
void SipHeaderContentType::setContentType(const string &content_type){
	this->content_type=content_type;
}

