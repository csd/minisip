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
 * 	SipResponse.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#include<config.h>

#include<libmsip/SipResponse.h>
#include<assert.h>
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
				addHeader(header);		//FIXME: Other headers should be copies as well XXX
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
		throw new SipExceptionInvalidMessage();
	}
//	setContent(NULL);
		//strlen(SIP/2.0)==7
	int afterws=7;
	while (resp[afterws]!='\0' && (resp[afterws]==' ' || resp[afterws]=='\t'))
		afterws++;
	assert(resp[afterws+0]>='0' && resp[afterws+0]<='9');
	assert(resp[afterws+1]>='0' && resp[afterws+1]<='9');
	assert(resp[afterws+2]>='0' && resp[afterws+2]<='9');
	
	status_code = (resp[afterws+0]-'0')*100 + (resp[afterws+1]-'0')*10 + resp[afterws+2]-'0';

	status_desc="";
	uint32_t i;
	for (i=12; resp[i]!='\r' && resp[i]!='\n'; i++){
		if(resp.size() == i){
#ifdef DEBUG_OUTPUT
		cerr << "SipResponse::SipResponse: message did not end correctly - throwing exception"<< endl;
#endif

			throw new SipExceptionInvalidMessage();
		}
		status_desc=status_desc+resp[i];
	}
	
/*
	bool done=false;
	int32_t nr_n, nr_r;
	
	string line;
	do{
		if (resp[i]=='\r' || resp[i]=='\n'){
			nr_n=nr_r=0;
			while (resp[i]=='\r' || resp[i]=='\n'){
				if (resp[i]=='\r')
					nr_r++;
				if (resp[i]=='\n')
					nr_n++;
				i++;
				if(resp.size() == i){
					break;
				}
			}	
			done = (nr_n>1) || (nr_r >1);
		}
		if (!done){
			line="";
			
			while ( (resp[i] != '\r') && (resp[i] != '\n') ){
				line = line + string(string("")+resp[i]);
				i++;
				if(resp.size() == i){
					throw new SipExceptionInvalidMessage();
				}
			}
			string ln = line;
			if (!addLine(ln)){
#ifdef DEBUG_OUTPUT
				//merr << "Info: Could not copy line to new Message: " << line << " (unknown)" << end;
#endif	
				if (SipUtils::startsWith(ln,"Proxy-Authenticate:") || SipUtils::startsWith(ln,"WWW-Authenticate")){
					unsigned r_pos = ln.find("realm=");
					unsigned n_pos = ln.find("nonce=");
					if (r_pos == string::npos || n_pos ==string::npos)
						merr << "ERROR: could not extract nonce and realm in line: " << ln << end;
					int32_t r_end = ln.find("\"",r_pos+7);
					int32_t n_end = ln.find("\"",n_pos+7);
					nonce = ln.substr(n_pos+7, n_end-(n_pos+7));
					realm = ln.substr(r_pos+7, r_end-(r_pos+7));
				}
			}
			
		}		
	}while (!done);
	string sdp;
	
	while ((resp[i]=='\r' || resp[i]=='\n') && !((unsigned)i>=resp.length())){
		i++;
	}

	if (getContentLength() > 0){
		if (getContentLength() > (int)resp.length() - (int)i)
			throw new SipExceptionInvalidMessage();
		else
			sdp = resp.substr(i, i+getContentLength());
			setContent(MRef<SipMessageContent*>(new SdpPacket(sdp)));
	}
	resp.erase(0, i+getContentLength());
	
	setDestinationBranch( getLastViaBranch() );
*/
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

/*
MRef<SdpPacket*> SipResponse::getSdp(){
	return MRef<SdpPacket*>((SdpPacket*)*content);
}
*/
