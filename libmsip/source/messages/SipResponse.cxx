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
 * 	SipResponse.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#include<config.h>

#include<libmsip/SipResponse.h>
#include<libmutil/massert.h>
#include<libmutil/itoa.h>
#include<libmutil/dbg.h>

#include<libmsip/SipHeaderVia.h>
#include<libmsip/SipHeaderFrom.h>
#include<libmsip/SipHeaderTo.h>
#include<libmsip/SipHeaderCallID.h>
#include<libmsip/SipHeaderCSeq.h>
#include<libmsip/SipHeaderMaxForwards.h>

#include<libmsip/SipException.h>
#include<libmsip/SipUtils.h>

const int SipResponse::type=8;

SipResponse::SipResponse(string branch, 
		int32_t status, 
		string status_desc, 
		MRef<SipMessage*> inv)
			:SipMessage(branch, SipResponse::type)
{
	setContent(NULL);

	this->status_code=status;
	this->status_desc=status_desc;

	MRef<SipHeaderValue*> mf = new SipHeaderValueMaxForwards(70);
	addHeader(new SipHeader(*mf));
	
	int noHeaders = inv->getNoHeaders();
	for (int i=0 ; i < noHeaders; i++){			//FIX: deep copy
		MRef<SipHeader*> header = inv->getHeaderNo(i);
		int headerType = header->getType();
		switch (headerType){
			case SIP_HEADER_TYPE_VIA:
			case SIP_HEADER_TYPE_FROM:
			case SIP_HEADER_TYPE_TO:
			case SIP_HEADER_TYPE_CALLID:
			case SIP_HEADER_TYPE_CSEQ:
			case SIP_HEADER_TYPE_RECORDROUTE: //if it exhists in the request, it is copied to response
				addHeader(header);	//FIXME: Other headers should be copies as well (?)
				break;
		}
	}
}

//TODO: This constructor needs rewriting (re-use from sipmessage)
SipResponse::SipResponse(string &resp): SipMessage(SipResponse::type, resp)
{

	if(resp.size() < 11){
#ifdef DEBUG_OUTPUT
		cerr << "SipResponse::SipResponse: message too short - throwing exception"<< endl;
#endif
		throw SipExceptionInvalidMessage("SipResponse too short");
	}
		//strlen(SIP/2.0)==7
	int afterws=7;
	while (resp[afterws]!='\0' && (resp[afterws]==' ' || resp[afterws]=='\t'))
		afterws++;
	massert(resp[afterws+0]>='0' && resp[afterws+0]<='9');
	massert(resp[afterws+1]>='0' && resp[afterws+1]<='9');
	massert(resp[afterws+2]>='0' && resp[afterws+2]<='9');
	
	status_code = (resp[afterws+0]-'0')*100 + (resp[afterws+1]-'0')*10 + resp[afterws+2]-'0';

	status_desc="";
	uint32_t i;
	for (i=12; resp[i]!='\r' && resp[i]!='\n'; i++){
		if(resp.size() == i){
#ifdef DEBUG_OUTPUT
		cerr << "SipResponse::SipResponse: message did not end correctly - throwing exception"<< endl;
#endif

			throw SipExceptionInvalidMessage("SipResponse malformed - could not find end of response description");
		}
		status_desc=status_desc+resp[i];
	}
}

string SipResponse::getString(){
	string rep = "SIP/2.0 "+itoa(status_code)+" "+status_desc+"\r\n";
	rep = rep + getHeadersAndContent();
	return rep;
}

int32_t SipResponse::getStatusCode(){
	return status_code;
}

string SipResponse::getStatusDesc(){
	return status_desc;
}

