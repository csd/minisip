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
 * 	SipHeaderSubject.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#include<config.h>


#include<libmsip/SipHeaderSubject.h>

#include<libmutil/itoa.h>

// Ex: Via: SIP/2.0/UDP 10.0.0.2:5043

SipHeaderSubject::SipHeaderSubject(const string &build_from)
		: SipHeader(SIP_HEADER_TYPE_SUBJECT)
{
	subject="NOT_SET";
}

SipHeaderSubject::SipHeaderSubject()
		: SipHeader(SIP_HEADER_TYPE_SUBJECT)
{
	subject="NOT_SET";
}

SipHeaderSubject::~SipHeaderSubject(){
}

string SipHeaderSubject::getString(){
	return "Subject: "+subject;
}

string SipHeaderSubject::getSubject(){
	return subject;
}
		
void SipHeaderSubject::setSubject(const string &subject){
	this->subject=subject;
}

