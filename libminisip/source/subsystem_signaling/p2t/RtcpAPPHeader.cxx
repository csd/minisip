/*
 Copyright (C) 2004-2006 the Minisip Team
 
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
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */

/* Copyright (C) 2004 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#include <config.h>

#include<libminisip/signaling/p2t/RtcpAPPHeader.h>
#include<libminisip/signaling/p2t/RtcpAPP.h>
#include<libmutil/dbg.h>

RtcpAPPHeader::RtcpAPPHeader(){
	//According RFC
	version=2;
	payloadtype=204;

	//without application-dependent data
	length=2;
	
	//Initialization	
	padding=0;
	subtype=0;
	SSRC=0;
	
	//TODO: take empty character. Which?
	//name[0]='_';
	//name[1]='_';
	//name[2]='_';
	//name[3]='_';
}

void RtcpAPPHeader::setVersion(int v){
	this->version = v;
}

void RtcpAPPHeader::setPadding(int p){
	this->padding = p;
}

void RtcpAPPHeader::setSubtype(int st){
	this->subtype = st;
}

void RtcpAPPHeader::setPayloadtype(int pt){
	this->payloadtype = pt;
}

void RtcpAPPHeader::setLength(int length){
	this->length = length;
}

void RtcpAPPHeader::setSSRC(int ssrc){
	this->SSRC = ssrc;
}

void RtcpAPPHeader::setName(string name){
	this->name[0] = name[0];
	this->name[1] = name[1];
	this->name[2] = name[2];
	this->name[3] = name[3];
}


int RtcpAPPHeader::getVersion(){
	return version;
}

int RtcpAPPHeader::getPadding(){
	return padding;
}

int RtcpAPPHeader::getSubtype(){
	return subtype;
}

int RtcpAPPHeader::getPayloadtype(){
	return payloadtype;
}

int RtcpAPPHeader::getLength(){
	return length;
}

int RtcpAPPHeader::getSSRC(){
	return SSRC;
}

string RtcpAPPHeader::getName(){
	return name;
}





int RtcpAPPHeader::size(){
	return 12;
}

char *RtcpAPPHeader::getBytes(){
	char *ret = new char[size()];
	struct rtcpAPPheader *hdrptr = (struct rtcpAPPheader *)ret;
	hdrptr->v=version;
	hdrptr->p=padding;
	hdrptr->st=subtype;
	hdrptr->pt=payloadtype;
	hdrptr->length=hton16(this->length);
	hdrptr->ssrc=hton32(SSRC);
	//hdrptr->name=hton32(this->name);
	hdrptr->c1=this->name[0];
	hdrptr->c2=this->name[1];
	hdrptr->c3=this->name[2];
	hdrptr->c4=this->name[3];
	return ret;
}


