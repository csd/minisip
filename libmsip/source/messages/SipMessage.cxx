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
 * 	SipMessage.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#include<config.h>

#include<stdio.h>
#include<libmsip/SipMessage.h>
#include<assert.h>
#include<errno.h>
#include<ctype.h>

#include<libmsip/SipHeaderContentLength.h>
#include<libmsip/SipHeaderVia.h>
#include<libmsip/SipHeaderFrom.h>
#include<libmsip/SipHeaderTo.h>
#include<libmsip/SipHeaderCSeq.h>
#include<libmsip/SipHeaderCallID.h>
#include<libmsip/SipHeaderContentLength.h>
#include<libmsip/SipHeaderUserAgent.h>
#include<libmsip/SipHeaderContact.h>
#include<libmsip/SipHeaderEvent.h>
#include<libmsip/SipHeaderAccept.h>
#include<libmsip/SipHeaderRecordRoute.h>
#include<libmsip/SipHeaderRoute.h>
#include<libmsip/SipHeaderUnsupported.h>
#include<libmsip/SipHeaderContentType.h>
#include<libmsip/SipHeaderMaxForwards.h>
#include<libmsip/SipHeaderWarning.h>

#include<libmsip/SipResponse.h>
#include<libmsip/SipAck.h>
#include<libmsip/SipCancel.h>
#include<libmsip/SipBye.h>
#include<libmsip/SipSubscribe.h>
#include<libmsip/SipNotify.h>
#include<libmsip/SipIMMessage.h>
#include<libmsip/SipUtils.h>

#include<libmsip/SipMessageContentIM.h>

#include<libmsip/SipException.h>

#include<libmutil/trim.h>
#include<libmutil/dbg.h>
#include<libmutil/itoa.h>

#ifdef DEBUG_OUTPUT
#include<libmsip/SipResponse.h>
#endif

string SipMessage::getDescription(){
        string ret;
//	char *str[11]={"UNKNOWN","INVITE", "REGISTER","BYE","CANCEL","ACK","NOTIFY","SUBSCRIBE","RESPONSE", "MESSAGE","TYPEUNKNOWN-FIXME"};
//	ret= str[type];
	ret = getTypeString();
	if (type==SipResponse::type)
		ret +="_"+itoa(((SipResponse*)(this))->getStatusCode());
	return ret;
}

SipMessage::~SipMessage(){

}

ostream & operator<<(ostream &out, SipMessage &p){
        out << p.getDescription();
	return out;
}


SipMessage::SipMessage(string b, int type):branch(b),type(type){
	content=NULL;
}

void SipMessage::addHeader(MRef<SipHeader*> header){
#ifdef MINISIP_MEMDEBUG
	header.setUser("SipMessage");
#endif
	headers.push_back(header);
//	headers.validate();
}

int SipMessage::getType(){
	return type;
}

MRef<SipHeader*> SipMessage::getHeaderNo(int i){
	return headers[i];
}

int SipMessage::getNoHeaders(){
	return headers.size();
}

int32_t SipMessage::getContentLength(){
	for (int32_t i=0; i< headers.size(); i++){
		MRef<SipHeaderContentLength*> len;
		if ((headers[i])->getType() == SIP_HEADER_TYPE_CONTENTLENGTH){
			len = MRef<SipHeaderContentLength*>((SipHeaderContentLength*)*headers[i]);
			return len->getContentLength();
		}
	}
	return 0;
}

string SipMessage::getHeadersAndContent(){
	string req="";
	int32_t clen=-1;

//	headers.validate();
	for (int32_t i=0; i< headers.size(); i++){
		req=req+headers[i]->getString()+"\r\n";
		if ((headers[i])->getType() == SIP_HEADER_TYPE_CONTENTLENGTH){
			clen=0;
		}
	}
	if (clen<0){
		if ( !content.isNull())
			clen=content->getString().length();
		else
			clen=0;
		SipHeaderContentLength content_length(clen);
		req=req+content_length.getString()+"\r\n";
	}
	req=req+"\r\n";
	
	
	if ( !content.isNull()){
		req=req + content->getString();
	}
	return req;
}

SipMessage::SipMessage(int type, string &build_from): type(type)
{
	//socket=NULL;
	content=NULL;
#ifdef MINISIP_MEMDEBUG
	content.setUser("SipMessage");
#endif
	uint32_t i;
	string header;
	for (i=0; build_from[i]!='\r' && build_from[i]!='\n'; i++){
		if(i==build_from.size())
			throw new SipExceptionInvalidMessage();
		header = header + build_from[i];

	}
	
	bool done=false;
	int32_t nr_n, nr_r;
	do{
		if (build_from[i]=='\r' || build_from[i]=='\n'){
			nr_n=nr_r=0;
			while ((i!=build_from.size()) && (build_from[i]=='\r' || build_from[i]=='\n')){
				if (build_from[i]=='\r')
					nr_r++;
				if (build_from[i]=='\n')
					nr_n++;
				i++;
			}	
			done = (nr_n>1) || (nr_r >1);
		}
		if (!done){
			string line="";
			while (build_from[i] != '\r' && build_from[i] != '\n'){
				line = line + build_from[i];
				i++;
				if(i==build_from.size()){
					throw new SipExceptionInvalidMessage();}
			}
			if (!addLine(line)){
#ifdef DEBUG_OUTPUT
				mdbg << "Info: Could not copy line to new Message: " << line << " (unknown)" << end;
#endif
			}
		}		
	}while (!done);
	
	string content="";
	
	nr_n=nr_r=0;
	done=false;

	if (getContentLength()>0){
		if(i+getContentLength()>build_from.size())
		{	
			throw new SipExceptionInvalidMessage();}
		for (int32_t j=0; j<getContentLength(); j++){
			content+=build_from[i+j];
		}
		MRef<SipMessageContent*> sdpOrIm;
		if (type==SipIMMessage::type){
			sdpOrIm = new SipMessageContentIM(trim(content));
		}else{
			sdpOrIm = new SdpPacket(trim(content));
		}
                //MRef<SipMessageContent*> sdp(new SdpPacket(trim(content)));
#ifdef MINISIP_MEMDEBUG
		sdp.setUser("SipMessage");
#endif
		setContent( sdpOrIm );
	}else{
		setContent(NULL);
	}
	build_from.erase(0,i+getContentLength());

	branch = getLastViaBranch();
}



bool SipMessage::addLine(string line){
	MRef<SipHeader*> hdr = SipHeader::parseHeader(line);
#ifdef MINISIP_MEMDEBUG
	hdr.setUser("SipMessage");
#endif
	addHeader(hdr);

	string ln = line;	//Hack to get realm an nonce... FIXME
	if (getType()==SipResponse::type && (SipUtils::startsWith(ln,"Proxy-Authenticate:") || SipUtils::startsWith(ln,"WWW-Authenticate")) ){

		unsigned r_pos = ln.find("realm=");
		unsigned n_pos = ln.find("nonce=");
		if (r_pos == string::npos || n_pos ==string::npos)
			merr << "ERROR: could not extract nonce and realm in line: " << ln << end;
		int32_t r_end = ln.find("\"",r_pos+7);
		int32_t n_end = ln.find("\"",n_pos+7);
		(static_cast<SipResponse*>(this))->setNonce(ln.substr(n_pos+7, n_end-(n_pos+7)));
		(static_cast<SipResponse*>(this))->setRealm( ln.substr(r_pos+7, r_end-(r_pos+7)) );


	}
	
	return true;
}


void SipMessage::setContent(MRef<SipMessageContent*> content){
//void SipMessage::setContent(MRef<SdpPacket*> content){
#ifdef MINISIP_MEMDEBUG
	if (content.getUser()=="")
		content.setUser("SipMessage");
#endif
	this->content=content;

	if( content ){
		string contentType = content->getContentType();
		if( contentType != "" ){
			MRef<SipHeaderContentType*> contenttypep = new SipHeaderContentType();
			contenttypep->setContentType( contentType );
			addHeader(MRef<SipHeader*>(*contenttypep));
		}
	}
}

//MRef<SipMessageContent*> SipMessage::getContent(){
MRef<SipMessageContent*> SipMessage::getContent(){
//#ifdef MINISIP_MEMDEBUG
//	MRef<SdpMessage*> nouser(*content);
//	nouser.setUser("(ret from SipMessage)");
//	return nouser;
//#else
//#endif
	
#ifdef MINISIP_MEMDEBUG
	content.setUser("(ret from SipMessage)");
		
#endif
	return content;
}

string SipMessage::getCallId(){
	for (int32_t i=0; i< headers.size(); i++){
		MRef<SipHeaderCallID*> id;
		if ((headers[i])->getType() == SIP_HEADER_TYPE_CALLID){
			id = MRef<SipHeaderCallID*>((SipHeaderCallID*)*headers[i]);
			return id->getId();
		}
	}
	return "";
}

int32_t  SipMessage::getCSeq(){
	for (int32_t i=0; i< headers.size(); i++){
		MRef<SipHeaderCSeq*> seq;
		if ((headers[i])->getType() == SIP_HEADER_TYPE_CSEQ){
			seq = MRef<SipHeaderCSeq*>((SipHeaderCSeq *)*headers[i]);
			return seq->getCSeq();
		}
	}
	merr << "ERROR: Could not find command sequence number in sip Message."<< end;
	return -1;
}

string SipMessage::getViaHeaderBranch(bool first){
	string b;
	for (int32_t i=0; i< headers.size(); i++){
		MRef<SipHeaderVia*> via;
		if ((headers[i])->getType() == SIP_HEADER_TYPE_VIA){
			via = MRef<SipHeaderVia*>((SipHeaderVia*)*headers[i]);
			b = via->getBranch();
			if (first)
				return b;
		}
	}
	return b;
}

string SipMessage::getFirstViaBranch(){
	return getViaHeaderBranch(true);
}

string SipMessage::getLastViaBranch(){
	return getViaHeaderBranch(false);
}

string SipMessage::getDestinationBranch(){
//#ifdef DEBUG_OUTPUT
//	assert(type!=SipResponse::type);
//#endif
	return branch;
}

string SipMessage::getCSeqMethod(){
	for (int32_t i=0; i < headers.size(); i++){
		MRef<SipHeaderCSeq*> seq;
		if ((headers[i])->getType() == SIP_HEADER_TYPE_CSEQ){
			seq = MRef<SipHeaderCSeq*>((SipHeaderCSeq *)(*headers[i]));
			return seq->getMethod();
		}
	
	}
	merr << "ERROR: Could not find command sequence method in sip Message."<< end;
	return "";
}

SipURI SipMessage::getFrom(){
	SipURI ret("","");
	MRef<SipHeaderFrom*> hto = getHeaderFrom();
	if (hto)
		ret = hto->getUri();

	return ret;
/*
	for (int32_t i=0; i< headers.size(); i++){
		MRef<SipHeaderFrom*> from;
		if ((headers[i])->getType() == SIP_HEADER_TYPE_FROM){
			from = MRef<SipHeaderFrom*>((SipHeaderFrom *)(*headers[i]));
			return from->getUri();
		}
	}
	return SipURI("","");
*/
}

SipURI SipMessage::getTo(){
	SipURI ret("","");
	MRef<SipHeaderTo*> hto = getHeaderTo();
	if (hto)
		ret = hto->getUri();

	return ret;
/*
	for (int32_t i=0; i< headers.size(); i++){
		MRef<SipHeaderTo*> to;
		if ((headers[i])->getType() == SIP_HEADER_TYPE_FROM){
			to = MRef<SipHeaderTo*>((SipHeaderTo *)(*headers[i]));
			return to->getUri();
		}
	}
	return SipURI("","");
*/
}

void SipMessage::removeAllViaHeaders(){
	bool done=false;
	int n=0;
	while (!done){
		done=true;

//		for (vector<MRef<SipHeader*> >::iterator i=headers.begin(); i!=headers.end(); i++){
		for (int i=0; i<headers.size(); i++){
			if ((headers[i])->getType()==SIP_HEADER_TYPE_VIA){
				headers.remove(i);
				done=false;
				n++;
//				i=headers.begin();
				i--;
				break;
			}
			
		}
	}
//	mout << "removeAllViaHeaders:: remove "<< n << " via headers"<< end;
}

MRef<SipHeaderFrom*> SipMessage::getHeaderFrom(){

	return MRef<SipHeaderFrom*> ((SipHeaderFrom*)*(getHeaderOfType(SIP_HEADER_TYPE_FROM)));

/*
	MRef<SipHeaderFrom*> from;
	for (int32_t i=0; i< headers.size(); i++){
		if ((headers[i])->getType() == SIP_HEADER_TYPE_FROM){
			from = MRef<SipHeaderFrom*>((SipHeaderFrom *)(*headers[i]));
			return from;
		}
	}
	return from; 
*/
	
}

MRef<SipHeaderTo*> SipMessage::getHeaderTo(){

	return MRef<SipHeaderTo*>((SipHeaderTo*)*(getHeaderOfType(SIP_HEADER_TYPE_TO)));

/*
	MRef<SipHeaderTo*> to;
	for (int32_t i=0; i< headers.size(); i++){
		if ((headers[i])->getType() == SIP_HEADER_TYPE_TO){
			to = MRef<SipHeaderTo*>((SipHeaderTo *)(*headers[i]));
			return to;
		}
	}
	return to; 
*/
}

MRef<SipHeader *> SipMessage::getHeaderOfType(int t){
	for (int32_t i=0; i< headers.size(); i++){
		if ((headers[i])->getType() == t){
			return headers[i];
		}
	}
	MRef<SipHeader*> nullhdr;
	return nullhdr; 

	
}

list<string> SipMessage::getRouteSet(){
	list<string> ret;
	for (uint32_t i = 0; i< (uint32_t)headers.size(); i++)
		if ((headers[i])->getType() == SIP_HEADER_TYPE_RECORDROUTE){
			string route = ((SipHeaderRecordRoute *)*(headers[i]))->getRoute();
			int i=0;
			string part;
			while (i<(int)route.size()){
				if (route[i]==','){
					ret.push_back(trim(part));
					part="";
				}else{
					part = part + route[i];
				}
				
			       	i++;
			}
			ret.push_back(trim(part));
		}
	return ret;
}


string SipMessage::getWarningMessage(){
	for (uint32_t i = 0; i< (uint32_t)headers.size(); i++)
		if ((headers[i])->getType() == SIP_HEADER_TYPE_WARNING){
			string warning = ((SipHeaderWarning *)*(headers[i]))->getWarning();
			return warning;
		}
	return "";
}
