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
 * 	SipHeaderValueAccept.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#include<config.h>


#include<assert.h>
#include<libmsip/SipHeaderAccept.h>

#include<libmutil/itoa.h>
#include<libmutil/trim.h>

#ifdef DEBUG_OUTPUT
#include<iostream>
#endif

const string sipHeaderValueAcceptTypeStr="Accept";

SipHeaderValueAccept::SipHeaderValueAccept(const string &build_from):SipHeaderValue(SIP_HEADER_TYPE_ACCEPT, sipHeaderValueAcceptTypeStr){
	accept = trim(build_from);
#ifdef DEBUG_OUTPUT
	cerr << "DEBUG: parsed Accept to: "<< accept<< endl;;
#endif
}

SipHeaderValueAccept::SipHeaderValueAccept():SipHeaderValue(SIP_HEADER_TYPE_ACCEPT,sipHeaderValueAcceptTypeStr){
	accept="NOT_SET";
}

SipHeaderValueAccept::~SipHeaderValueAccept(){
}

string SipHeaderValueAccept::getString(){
	return /*"Accept: "+*/accept;
}

string SipHeaderValueAccept::getAccept(){
	return accept;
}
		
void SipHeaderValueAccept::setAccept(const string &accept){
	this->accept=accept;
}

