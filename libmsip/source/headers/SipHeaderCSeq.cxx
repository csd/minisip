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
 * 	SipHeaderValueCSeq.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#include<config.h>


#include<libmsip/SipHeaderCSeq.h>

#include<libmutil/itoa.h>
#include<libmutil/trim.h>

const string sipHeaderValueCSeqTypeStr = "CSeq";

SipHeaderValueCSeq::SipHeaderValueCSeq(const string &build_from):SipHeaderValue(SIP_HEADER_TYPE_CSEQ,sipHeaderValueCSeqTypeStr){
	unsigned i=0;
	while (build_from[i]==' ')
		i++;
	string num;
	while ((build_from[i]>='0' && build_from[i]<='9') || build_from[i]=='-'){
		num+=build_from[i];
		i++;
	}
	
	while (build_from[i]==' ')
		i++;
	
	method="";
	while (!(i>=build_from.length())){
		method+=build_from[i];
		i++;
	}
	setCSeq(atoi((trim(num)).c_str()));
}

SipHeaderValueCSeq::SipHeaderValueCSeq() : SipHeaderValue(SIP_HEADER_TYPE_CSEQ,sipHeaderValueCSeqTypeStr){
	method="NOT_SET";
}

SipHeaderValueCSeq::~SipHeaderValueCSeq(){
}

string SipHeaderValueCSeq::getString(){
	return /*"CSeq: "+*/itoa(seq)+" "+method;
}

string SipHeaderValueCSeq::getMethod(){
	return method;
}
		
void SipHeaderValueCSeq::setMethod(const string &method){
	this->method=method;
}

void SipHeaderValueCSeq::setCSeq(int32_t n){
	this->seq = n;
}

int32_t SipHeaderValueCSeq::getCSeq(){
	return seq;
}
