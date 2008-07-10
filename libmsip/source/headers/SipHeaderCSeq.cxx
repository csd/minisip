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
 * 	SipHeaderValueCSeq.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#include<config.h>


#include<libmsip/SipHeaderCSeq.h>

#include<libmutil/stringutils.h>
#include<stdlib.h>

using namespace std;

MRef<SipHeaderValue *> cseqFactory(const string &build_from){
	                return new SipHeaderValueCSeq(build_from);
}

SipHeaderFactoryFuncPtr sipHeaderCSeqFactory=cseqFactory;



const string sipHeaderValueCSeqTypeStr = "CSeq";

SipHeaderValueCSeq::SipHeaderValueCSeq(const string &build_from):SipHeaderValue(SIP_HEADER_TYPE_CSEQ,sipHeaderValueCSeqTypeStr){
	unsigned maxlen=(unsigned)build_from.size();
	unsigned i=0;

	while (i<maxlen && isWS(build_from[i]))
		i++;

	string num;
	while (i<maxlen && (build_from[i]>='0' && build_from[i]<='9') || build_from[i]=='-'){
		num+=build_from[i];
		i++;
	}
	
	while (i<maxlen && isWS(build_from[i]) )
		i++;

	method = build_from.substr(i);

	setCSeq(atoi((trim(num)).c_str()));
}

SipHeaderValueCSeq::SipHeaderValueCSeq(const string &meth, int s) : SipHeaderValue(SIP_HEADER_TYPE_CSEQ,sipHeaderValueCSeqTypeStr), method(meth),seq(s) {
}

SipHeaderValueCSeq::~SipHeaderValueCSeq(){
}

string SipHeaderValueCSeq::getString() const{
	return itoa(seq)+" "+method;
}

string SipHeaderValueCSeq::getMethod() const{
	return method;
}
		
void SipHeaderValueCSeq::setMethod(const string &m){
	this->method=m;
}

void SipHeaderValueCSeq::setCSeq(int32_t n){
	this->seq = n;
}

int32_t SipHeaderValueCSeq::getCSeq() const{
	return seq;
}

