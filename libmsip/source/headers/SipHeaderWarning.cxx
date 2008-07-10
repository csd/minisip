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
 * 	SipHeaderValueWarning.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#include<config.h>


#include<libmsip/SipHeaderWarning.h>
#include<libmutil/stringutils.h>

#include<stdlib.h>

using namespace std;

MRef<SipHeaderValue *> warningFactory(const string &build_from){
	                return new SipHeaderValueWarning(build_from);
}

SipHeaderFactoryFuncPtr sipHeaderWarningFactory=warningFactory;



const string sipHeaderValueWarningTypeStr = "Warning";

SipHeaderValueWarning::SipHeaderValueWarning(const string &build_from)
		: SipHeaderValue(SIP_HEADER_TYPE_WARNING,sipHeaderValueWarningTypeStr)
{
	errorCode = 0;
	size_t blank1Pos = build_from.find(" ");
	size_t blank2Pos;
	if (blank1Pos==string::npos){
		return;
	}
	blank2Pos = blank1Pos + 1 + build_from.substr(blank1Pos+1,string::npos).find(" ");
	if (blank2Pos==string::npos){
		return;
	}
	
	errorCode = atoi(build_from.substr(0,3).c_str());
	domainName = build_from.substr(blank1Pos+1, blank2Pos-blank1Pos);
	warning = build_from.substr(blank2Pos);
}


SipHeaderValueWarning::SipHeaderValueWarning(string dn, uint16_t ec, string warn)
		: SipHeaderValue(SIP_HEADER_TYPE_WARNING,sipHeaderValueWarningTypeStr)
{
	this->errorCode=ec;
	this->domainName=dn;
	this->warning=warn;
}

SipHeaderValueWarning::~SipHeaderValueWarning(){

}

string SipHeaderValueWarning::getString() const{
	return /*"Warning: "+*/ itoa(errorCode)+" "+domainName+" \""+warning+"\"";
}

string SipHeaderValueWarning::getWarning() const{
	return warning;
}
		
void SipHeaderValueWarning::setWarning(const string &w){
	this->warning=w;
}

string SipHeaderValueWarning::getDomainName() const{
	return domainName;
}

void SipHeaderValueWarning::setDomainName(const string &d){
	this->domainName=d;
}

uint16_t SipHeaderValueWarning::getErrorCode() const{
	return errorCode;
}

void SipHeaderValueWarning::setErrorCode(const uint16_t& e){
	this->errorCode=e;
}

