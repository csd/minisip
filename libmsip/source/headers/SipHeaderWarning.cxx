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
 * 	SipHeaderWarning.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#include<config.h>


#include<libmsip/SipHeaderWarning.h>

#include<libmutil/itoa.h>

SipHeaderWarning::SipHeaderWarning(const string &build_from)
		: SipHeader(SIP_HEADER_TYPE_WARNING)
{
	size_t blank1Pos = build_from.find(" ");
	size_t blank2Pos, blank3Pos;
	if (blank1Pos==string::npos){
		return;
	}
	blank2Pos = build_from.substr(blank1Pos,string::npos).find(" ");
	if (blank2Pos==string::npos){
		return;
	}
	blank3Pos = build_from.substr(blank2Pos,string::npos).find(" ");
	if (blank3Pos==string::npos){
		return;
	}

	errorCode=atoi(build_from.substr(blank1Pos+1, 3).c_str());
	domainName=build_from.substr(blank2Pos+1, blank3Pos-blank2Pos);
	warning=build_from.substr(blank3Pos+2, build_from.size()-blank3Pos-1);

}


SipHeaderWarning::SipHeaderWarning(string domainName, uint16_t errorCode, string warning)
		: SipHeader(SIP_HEADER_TYPE_WARNING)
{
	this->errorCode=errorCode;
	this->domainName=domainName;
	this->warning=warning;
}

SipHeaderWarning::~SipHeaderWarning(){

}

string SipHeaderWarning::getString(){
	return "Warning: "+itoa(errorCode)+" "+domainName+" \""+warning+"\"";
}

string SipHeaderWarning::getWarning(){
	return warning;
}
		
void SipHeaderWarning::setWarning(const string &warning){
	this->warning=warning;
}

string SipHeaderWarning::getDomainName(){
	return domainName;
}

void SipHeaderWarning::setDomainName(const string &domainName){
	this->domainName=domainName;
}

uint16_t SipHeaderWarning::getErrorCode(){
	return errorCode;
}

void SipHeaderWarning::setErrorCode(const uint16_t& errorCode){
	this->errorCode=errorCode;
}
