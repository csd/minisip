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

#include<libmsip/SipMessageContentFactory.h>
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
#include<libmsip/SipRefer.h>
#include<libmsip/SipIMMessage.h>
#include<libmsip/SipUtils.h>

#include<libmsip/SipMessageContentIM.h>

#include<libmsip/SipException.h>

#include<libmutil/trim.h>
#include<libmutil/dbg.h>
#include<libmutil/itoa.h>
#include<libmsip/SipResponse.h>

#ifdef _MSC_VER
template class __declspec(dllexport) MRef<SipMessage*>;
#endif

MRef<SipMessageContent*> sipSipMessageContentFactory(const string & buf, const string & ContentType){
	string tmp = buf;
	return (*SipMessage::createMessage(tmp));
}

SMCFCollection SipMessage::contentFactories=SMCFCollection();

string SipMessage::getDescription(){
        string ret;
	ret = getTypeString();
	if (type==SipResponse::type)
		ret +="_"+itoa(((SipResponse*)(this))->getStatusCode());
	return ret;
}

SipMessage::~SipMessage(){

}

MRef<SipMessage*> SipMessage::createMessage(string &data){
	
	int n = data.size();

	if (n>3   &&    (data[0]=='S'||data[0]=='s') &&
			(data[1]=='I'||data[1]=='i') &&
			(data[2]=='P'||data[2]=='p' )){
		return MRef<SipMessage*>(new SipResponse(data));
	}else{
		if (n> 7 && data.substr(0, 7) == "MESSAGE"){
			return MRef<SipMessage*>(new SipIMMessage(data));
		}
		if (n> 6 && data.substr(0, 6) == "CANCEL"){
			return MRef<SipMessage*>(new SipCancel(data));
		}
		if (n> 3 && data.substr(0, 3)=="BYE"){
			return MRef<SipMessage*>(new SipBye(data));
		}
		if (n> 6 && data.substr(0, 6)=="INVITE"){
			return MRef<SipMessage*>(new SipInvite(data));
		}
		if (n> 3 && data.substr(0, 3)=="ACK"){
			return MRef<SipMessage*>(new SipAck(data));
		}
		if (n> 9 && data.substr(0, 9)=="SUBSCRIBE"){
			return MRef<SipMessage*>(new SipSubscribe(data));
		}
		if (n> 6 && data.substr(0, 6)=="NOTIFY"){
			return MRef<SipMessage*>(new SipNotify(data));
		}
		if (n> 5 && data.substr(0, 5)=="REFER"){
			return MRef<SipMessage*>(new SipRefer(data));
		}

		throw new SipExceptionInvalidStart;
	}
	return NULL;
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
		MRef<SipHeaderValueContentLength*> len;
		if ((headers[i])->getType() == SIP_HEADER_TYPE_CONTENTLENGTH){
			len = MRef<SipHeaderValueContentLength*>((SipHeaderValueContentLength*)*(headers[i]->getHeaderValue(0)));
			return len->getContentLength();
		}
	}
	return 0;
}

string SipMessage::getHeadersAndContent(){
	string req="";
	int32_t clen=-1;

	for (int32_t i=0; i< headers.size(); i++){
		req=req+headers[i]->getString()+"\r\n";
		if ((headers[i])->getType() == SIP_HEADER_TYPE_CONTENTLENGTH){
			clen=0;
		}
	}
	if (clen<0){
		if ( !content.isNull())
			clen=(int)content->getString().length();
		else
			clen=0;
		SipHeader content_length(new SipHeaderValueContentLength(clen));
		req=req+content_length.getString()+"\r\n";
	}
	req=req+"\r\n";
	
	
	if ( !content.isNull()){
		req=req + content->getString();
	}
	return req;
}

/**
 * @return Index where the content of the message starts (if any)
 */
int SipMessage::parseHeaders(const string &buf, int startIndex){
	int i=startIndex;
	int end = (int)buf.size();
	do{
		if (i+2<=end && buf[i]=='\n' && buf[i+1]=='\n')	// i points to first after header
			return i+2;				// return pointer to start of content

		if (i+4<=end && buf[i]=='\r' && buf[i+1]=='\n' 
				&& buf[i+2]=='\r' && buf[i+3]=='\n' )	// i points to first after header
			return i+4;				// return pointer to start of content
		if (i+4<=end && buf[i]=='\n' && buf[i+1]=='\r' 
				&& buf[i+2]=='\n' && buf[i+3]=='\r' )	// i points to first after header
			return i+4;				// return pointer to start of content
	
		int eoh = SipUtils::findEndOfHeader(buf, i);	// i will be adjusted to start of header
		string header = buf.substr(i, eoh-i+1);

		if (!addLine(header)){
#ifdef DEBUG_OUTPUT
			mdbg << "Info: Could not copy line to new Message: " << header << " (unknown)" << end;
#endif
		}
		i=eoh+1;
	}while (i<end);
	return i;
}

SipMessage::SipMessage(int type, string &buildFrom): type(type)
{
	uint32_t i;
	string header;
	for (i=0; buildFrom[i]!='\r' && buildFrom[i]!='\n'; i++){
		if(i==buildFrom.size()){
#ifdef DEBUG_OUTPUT
			cerr << "SipMessage::SipMessage: Size is too short - throwing exception"<< endl;
#endif
			throw new SipExceptionInvalidMessage();
		}
		header = header + buildFrom[i];

	}
	
	int contentStart = parseHeaders(buildFrom, i);
	int clen = getContentLength();
	if (clen>0){
		string content=buildFrom.substr(contentStart, clen);
		if ((int)content.length() != clen){
			cerr << "WARNING: Length of content was shorter than expected (" << clen <<"!="<<(int)content.length()<<")"<<endl;
		}
		MRef<SipHeader*> h = getHeaderOfType(SIP_HEADER_TYPE_CONTENTTYPE);
		if (h){	
			MRef<SipMessageContent*> smcref;
			string contentType = ((SipHeaderValueContentType*)*(h->getHeaderValue(0)))->getContentType();
			SipMessageContentFactoryFuncPtr contentFactory = contentFactories.getFactory( contentType);
			if (contentFactory){
				MRef<SipMessageContent*> smcref = contentFactory(content, contentType);
				setContent(smcref);
			}else{ //TODO: Better error handling
				merr << "WARNING: No SipMessageContentFactory found for content type "<<contentType <<end;
			}
			
		}else{ //TODO: Better error handling
			merr << "WARNING: Sip message has content, but no content type! Content ignored."<< end;
		}
	}
	
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
		size_t r_pos = ln.find("realm=");
		size_t n_pos = ln.find("nonce=");
		if (r_pos == string::npos || n_pos ==string::npos){
			merr << "ERROR: could not extract nonce and realm in line: " << ln << end;
		}
		size_t r_end = ln.find("\"",r_pos+7);
		size_t n_end = ln.find("\"",n_pos+7);
		string sub = ln.substr(n_pos+7, n_end-(n_pos+7));
		setNonce(sub);
		sub = ln.substr(r_pos+7, r_end-(r_pos+7));
		setRealm( sub );
	}
	return true;
}


void SipMessage::setContent(MRef<SipMessageContent*> content){
#ifdef MINISIP_MEMDEBUG
	if (content.getUser()=="")
		content.setUser("SipMessage");
#endif
	this->content=content;

	if( content ){
		string contentType = content->getContentType();
		if( contentType != "" ){
			MRef<SipHeaderValueContentType*> contenttypep = new SipHeaderValueContentType();
			contenttypep->setContentType( contentType );
			addHeader(new SipHeader(*contenttypep));
		}
	}
}

MRef<SipMessageContent*> SipMessage::getContent(){
	return content;
}

string SipMessage::getCallId(){
	for (int32_t i=0; i< headers.size(); i++){
		MRef<SipHeaderValueCallID*> id;
		if ((headers[i])->getType() == SIP_HEADER_TYPE_CALLID){
			id = MRef<SipHeaderValueCallID*>((SipHeaderValueCallID*)*(headers[i]->getHeaderValue(0)));
			return id->getId();
		}
	}
	return "";
}

int32_t  SipMessage::getCSeq(){
	for (int32_t i=0; i< headers.size(); i++){
		MRef<SipHeaderValueCSeq*> seq;
		if ((headers[i])->getType() == SIP_HEADER_TYPE_CSEQ){
			seq = MRef<SipHeaderValueCSeq*>((SipHeaderValueCSeq *)*(headers[i]->getHeaderValue(0)));
			return seq->getCSeq();
		}
	}
	merr << "ERROR: Could not find command sequence number in sip Message."<< end;
	return -1;
}

string SipMessage::getViaHeaderBranch(bool first){
	string b;
	for (int32_t i=0; i< headers.size(); i++){
		MRef<SipHeaderValueVia*> via;
		if ((headers[i])->getType() == SIP_HEADER_TYPE_VIA){
			via = MRef<SipHeaderValueVia*>((SipHeaderValueVia*)*(headers[i]->getHeaderValue(0)));
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
	return branch;
}

string SipMessage::getCSeqMethod(){
	for (int32_t i=0; i < headers.size(); i++){
		MRef<SipHeaderValueCSeq*> seq;
		if ((headers[i])->getType() == SIP_HEADER_TYPE_CSEQ){
			seq = MRef<SipHeaderValueCSeq*>((SipHeaderValueCSeq *)*(headers[i]->getHeaderValue(0)));
			return seq->getMethod();
		}
	
	}
	merr << "ERROR: Could not find command sequence method in sip Message."<< end;
	return "";
}

SipURI SipMessage::getFrom(){
	SipURI ret("","");
	MRef<SipHeaderValueFrom*> hfrom = getHeaderValueFrom();
	if (hfrom)
		ret = hfrom->getUri();

	return ret;
}

SipURI SipMessage::getTo(){
	SipURI ret("","");
	MRef<SipHeaderValueTo*> hto = getHeaderValueTo();
	if (hto)
		ret = hto->getUri();

	return ret;
}

void SipMessage::removeAllViaHeaders(){
	bool done=false;
	int n=0;
	while (!done){
		done=true;

		for (int i=0; i<headers.size(); i++){
			if ((headers[i])->getType()==SIP_HEADER_TYPE_VIA){
				headers.remove(i);
				done=false;
				n++;
				i--;
				break;
			}
			
		}
	}
}

MRef<SipHeaderValueFrom*> SipMessage::getHeaderValueFrom(){

	return MRef<SipHeaderValueFrom*> ( (SipHeaderValueFrom*)*( getHeaderOfType(SIP_HEADER_TYPE_FROM)->getHeaderValue(0)) );

}

MRef<SipHeaderValueTo*> SipMessage::getHeaderValueTo(){

	return MRef<SipHeaderValueTo*>( (SipHeaderValueTo*)*(getHeaderOfType(SIP_HEADER_TYPE_TO)->getHeaderValue(0)));

}

MRef<SipHeader *> SipMessage::getHeaderOfType(int t, int i){
	
	for (int32_t j=0; j< headers.size(); j++){
		if ((headers[j])->getType() == t){
			if (i==0)
				return headers[j];
			else
				i--;
		}
	}
	MRef<SipHeader*> nullhdr;
	return nullhdr; 
}


MRef<SipHeaderValue*> SipMessage::getHeaderValueNo(int type, int i){
	int headerindex=0;
	int valueindex=0;
	do{
		MRef<SipHeader *> h = getHeaderOfType(type, headerindex);
		if (h){
			int nval = h->getNoValues();
			if (i < valueindex+nval){	//the value we want is in this header
				return h->getHeaderValue(i-valueindex);
			}
			valueindex += nval;
			headerindex++;
		}else{
			MRef<SipHeaderValue*> nullhdr;
			return nullhdr;
		}
	}while(true);

}



/*
list<string> SipMessage::getRouteSet(){
	list<string> ret;
	for (uint32_t i = 0; i< (uint32_t)headers.size(); i++)
		if ((headers[i])->getType() == SIP_HEADER_TYPE_RECORDROUTE){
			string route = ((SipHeaderValueRecordRoute *)*(headers[i]->getHeaderValue(0)))->getRoute();
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
*/

string SipMessage::getWarningMessage(){
	for (uint32_t i = 0; i< (uint32_t)headers.size(); i++)
		if ((headers[i])->getType() == SIP_HEADER_TYPE_WARNING){
			string warning = ((SipHeaderValueWarning *)*(headers[i]->getHeaderValue(0)))->getWarning();
			return warning;
		}
	return "";
}


string SipMessage::getRealm(){
        return realm;
}

void SipMessage::setRealm(string r){
        realm = r;
}

string SipMessage::getNonce(){
        return nonce;
}

void SipMessage::setNonce(string n){
        nonce=n;
}
