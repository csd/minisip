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
#include<libmutil/stringutils.h>
#include<libmutil/dbg.h>

#include<libmsip/SipHeaderVia.h>
#include<libmsip/SipHeaderFrom.h>
#include<libmsip/SipHeaderTo.h>
#include<libmsip/SipHeaderCallID.h>
#include<libmsip/SipHeaderCSeq.h>
#include<libmsip/SipHeaderMaxForwards.h>
#include<libmsip/SipHeaderUnsupported.h>

#include<libmsip/SipException.h>
#include<libmsip/SipUtils.h>

using namespace std;

const string SipResponse::type="RESPONSE";

SipResponse::SipResponse( int32_t status, 
		string status_desc_, 
			  MRef<SipRequest*> req): request( req )
{
	this->status_code=status;
	this->status_desc=status_desc_;

	setContent(NULL);

	MRef<SipHeaderValue*> mf = new SipHeaderValueMaxForwards(70);
	addHeader(new SipHeader(*mf));
	
	int noHeaders = req->getNoHeaders();
	for (int i=0 ; i < noHeaders; i++){			//FIX: deep copy
		MRef<SipHeader*> header = req->getHeaderNo(i);
		int headerType = header->getType();
		switch (headerType){
			case SIP_HEADER_TYPE_VIA:
			case SIP_HEADER_TYPE_FROM:
			case SIP_HEADER_TYPE_TO:
			case SIP_HEADER_TYPE_CALLID:
			case SIP_HEADER_TYPE_CSEQ:
			case SIP_HEADER_TYPE_RECORDROUTE: //if it exhists in the request, it is copied to response
				addHeader(header);
				break;
			default:
				/*don't copy any other header*/
				break;
		}
	}
}

//TODO: This constructor needs rewriting (re-use from sipmessage)
SipResponse::SipResponse(string &resp): SipMessage(resp)
{
	size_t len = resp.size();

	size_t i =0;

	//If stream transport we should allow whitespace before the start
	//of the message
	while (i<len &&(resp[i]==' ' || resp[i]=='\r' || resp[i]=='\n' || resp[i]=='\t')){
		i++;
	}
		//strlen(SIP/2.0)==7
	if (!( i+7<=len && resp.substr(i,7)=="SIP/2.0" ) ){
		throw SipExceptionInvalidMessage("SipResponse header error");
	}
	i+=7;

	size_t afterws=i;
	while ( afterws<len && resp[afterws]!='\0' && (resp[afterws]==' ' || resp[afterws]=='\t'))
		afterws++;
	
	if (afterws+3 >=len){
		throw SipExceptionInvalidMessage("SipResponse header error");
	}
	
	if (! (resp[afterws+0]>='0' && resp[afterws+0]<='9' &&
				resp[afterws+1]>='0' && resp[afterws+1]<='9' &&
				resp[afterws+2]>='0' && resp[afterws+2]<='9' ) ){
		throw SipExceptionInvalidMessage("SipResponse without status code");
	}
	
	status_code = (resp[afterws+0]-'0')*100 + (resp[afterws+1]-'0')*10 + resp[afterws+2]-'0';

	status_desc="";
	i=afterws+3;	//go past response code
	while( resp[i] == ' ' ){
	  i++;
	}

	for ( ; resp[i]!='\r' && resp[i]!='\n'; i++){
		if(len == i){
#ifdef DEBUG_OUTPUT
		mdbg("signaling/sip") << "SipResponse::SipResponse: message did not end correctly - throwing exception"<< endl;
#endif

			throw SipExceptionInvalidMessage("SipResponse malformed - could not find end of response description");
		}
		status_desc=status_desc+resp[i];
	}
}

string SipResponse::getString() const{
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

void SipResponse::addUnsupported(const std::list<string> &unsupported){
	list<string>::const_iterator i;
	list<string>::const_iterator last = unsupported.end();

	for( i = unsupported.begin(); i != last; i++ ){
		const string &ext = *i;

		SipHeaderValue *value = new SipHeaderValueUnsupported( ext );
		addHeader( new SipHeader( value ));
	}
}

MRef<SipRequest*> SipResponse::getRequest() const{
	return request;
}
