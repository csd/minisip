/*
  Copyright (C) 2006 Erik Eliasson
  
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

/* Name
 * 	SipHeaderValueRSeq.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#include<config.h>


#include<libmsip/SipHeaderRSeq.h>
#include<libmsip/SipException.h>
#include<libmutil/stringutils.h>

#include<stdlib.h>

using namespace std;

MRef<SipHeaderValue *> rseqFactory(const string &build_from){
	                return new SipHeaderValueRSeq(build_from);
}

SipHeaderFactoryFuncPtr sipHeaderRSeqFactory=rseqFactory;



const string sipHeaderValueRSeqTypeStr = "RSeq";

SipHeaderValueRSeq::SipHeaderValueRSeq(const string &build_from)
		: SipHeaderValue(SIP_HEADER_TYPE_RSEQ,sipHeaderValueRSeqTypeStr)
{
	unsigned i=0;
	unsigned len=(unsigned)build_from.size();
	while (isWS(build_from[i]))
		i++;
	string num="";
	while (i<len && build_from[i]>='0' && build_from[i]<='9'){
		num+=build_from[i];
		i++;
	}

	while (i<len && isWS(build_from[i]))
		i++;

	if (num.size()==0 || i<len ){ 
		throw SipExceptionInvalidMessage("SipHeaderValueRSeq malformed");
	}
#ifdef _MSC_VER
	seq=(uint32_t)_atoi64(num.c_str());
#else
	seq=atoll(num.c_str());
#endif
}

SipHeaderValueRSeq::SipHeaderValueRSeq(uint32_t n)
		: SipHeaderValue(SIP_HEADER_TYPE_RSEQ,sipHeaderValueRSeqTypeStr)
{
	seq=n;
}

SipHeaderValueRSeq::~SipHeaderValueRSeq(){
}

string SipHeaderValueRSeq::getString() const{
	return itoa(seq);
}

uint32_t SipHeaderValueRSeq::getRSeq() const{
	return seq;
}
		
void SipHeaderValueRSeq::setRSeq( uint32_t rseq ){
	seq = rseq;
}
