/*
  Copyright (C) 2005, 2004 Erik Eliasson, Johan Bilien
  Copyright (C) 2006 Mikael Magnusson
  
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
 *          Mikael Magnusson <mikma@users.sourceforge.net>
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
#include<errno.h>
#include<ctype.h>

#include<libmnetutil/Socket.h>
#include<libmsip/SipMessageContentFactory.h>
#include<libmsip/SipHeaderContentLength.h>
#include<libmsip/SipHeaderVia.h>
#include<libmsip/SipHeaderFrom.h>
#include<libmsip/SipHeaderTo.h>
#include<libmsip/SipHeaderCSeq.h>
#include<libmsip/SipHeaderCallID.h>
#include<libmsip/SipHeaderContentLength.h>
#include<libmsip/SipHeaderContact.h>
#include<libmsip/SipHeaderRecordRoute.h>
#include<libmsip/SipHeaderRoute.h>
#include<libmsip/SipHeaderRequire.h>
#include<libmsip/SipHeaderSupported.h>
#include<libmsip/SipHeaderContentType.h>
#include<libmsip/SipHeaderWWWAuthenticate.h>
#include<libmsip/SipHeaderWarning.h>

#include<libmsip/SipRequest.h>
#include<libmsip/SipResponse.h>
#include<libmsip/SipUtils.h>

#include<libmsip/SipMessageContentIM.h>

#include<libmsip/SipException.h>

#include<libmutil/stringutils.h>
#include<libmutil/dbg.h>
#include<libmutil/Timestamp.h>

using namespace std;

const string SipMessage::anyType="";

#if defined(_MSC_VER) && !defined(_WIN32_WCE)
	template class __declspec(dllexport) MRef<SipMessage*>;
#endif

MRef<SipMessageContent*> sipSipMessageContentFactory(const string & buf, const string &){
	string tmp = buf;
	return (*SipMessage::createMessage(tmp));
}

SMCFCollection SipMessage::contentFactories=SMCFCollection();

string SipMessage::getDescription(){
        string ret;
	ret = getType();
	if (ret=="RESPONSE")
		ret +="_"+itoa(((SipResponse*)(this))->getStatusCode());
	return ret;
}

SipMessage::~SipMessage(){

}

MRef<SipMessage*> SipMessage::createMessage(string &data){
	
	size_t n = data.size();

	if (n>3   &&    (data[0]=='S'||data[0]=='s') &&
			(data[1]=='I'||data[1]=='i') &&
			(data[2]=='P'||data[2]=='p' )){
		return MRef<SipMessage*>(new SipResponse(data));
	}else{
		return new SipRequest(data);

	}
	return NULL;
}

ostream & operator<<(ostream &out, SipMessage &p){
        out << p.getDescription();
	return out;
}



SipMessage::SipMessage(string b):branch(b){
	content=NULL;
}


void SipMessage::addHeader(MRef<SipHeader*> header){
	if( header.isNull() ) {
		merr << "ERROR: trying to add null header to message!"<<end;
		return;
	}
	headers.push_back(header);
}

MRef<SipHeader*> SipMessage::getHeaderNo(int i){
	if (i>=headers.size()){
		MRef<SipHeader*> nullhdr;
		return nullhdr;
	}
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
	int endBuf = (int)buf.size();
	//This filters the sipfrag messages we receive ... like in NOTIFY ... which most of the times come without any header
	if( startIndex + 4 >= endBuf ) {
		#ifdef DEBUG_OUTPUT
		mdbg << "SipMessage::parseHeaders: Info: SipMessage without headers ... only request line" << end;
		#endif
		return i;
	}
	do{
		if (i+2<=endBuf && buf[i]=='\n' && buf[i+1]=='\n') {	// i points to first after header
			return i+2;				// return pointer to start of content
		}
		if (i+4<=endBuf && buf[i]=='\r' && buf[i+1]=='\n' 
				&& buf[i+2]=='\r' && buf[i+3]=='\n' ){	// i points to first after header
			return i+4;				// return pointer to start of content
		}
		if (i+4<=endBuf && buf[i]=='\n' && buf[i+1]=='\r' 
				&& buf[i+2]=='\n' && buf[i+3]=='\r' ){	// i points to first after header
			return i+4;				// return pointer to start of content
		}
		int eoh = SipUtils::findEndOfHeader(buf, i);	// i will be adjusted to start of header
		string header = buf.substr(i, eoh-i+1);
// 		merr << "SipMessage::parseHeaders: parsing line = ##" << header 
// 			<< "## [end=" << endBuf << "; i="<< i 
// 			<< "; eoh=" << eoh << "; length=" 
// 			<< eoh-i+1 << "]" << end;
		if( header == "" ) {
			#ifdef DEBUG_OUTPUT
			mdbg << "SipMessage::parseHeaders: Info: Could not copy line to new Message: (empty line)" << end;
			#endif
		} else if (!addLine(header)){
			#ifdef DEBUG_OUTPUT
			mdbg << "SipMessage::parseHeaders: Info: Could not copy line to new Message: " << header << " (unknown)" << end;
			#endif
		}
		i=eoh+1;
	}while (i<endBuf);
	
	return i;
}

SipMessage::SipMessage(int, string &buildFrom)
{
	uint32_t i;

	//string header;
	for (i=0; buildFrom[i]!='\r' && buildFrom[i]!='\n'; i++){
		if(i==buildFrom.size()){
#ifdef DEBUG_OUTPUT
			cerr << "SipMessage::SipMessage: Size is too short - throwing exception"<< endl;
#endif
			throw SipExceptionInvalidMessage("SIP Message too short");
		}
		//header = header + buildFrom[i];

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
			string contentType = ((SipHeaderValueString*)*(h->getHeaderValue(0) ))->getString();
//			string b = (SipHeaderValueContentType*)*(h->getHeaderValue(0) )->getParameter("boundary");
//cerr <<"boundary="<< b <<endl;
			SipMessageContentFactoryFuncPtr contentFactory = contentFactories.getFactory( contentType );
			if (contentFactory){
				MRef<SipMessageContent*> smcref = contentFactory(content, contentType + "; boundary=boun=_dry");
				setContent(smcref);
			}else{ //TODO: Better error handling
				merr << "WARNING: No SipMessageContentFactory found for content type "<<contentType <<end;
			}
			
		}else{ //TODO: Better error handling
			merr << "WARNING: Sip message has content, but no content type! Content ignored."<< end;
		}
	}
	
	branch = getFirstViaBranch();
}




bool SipMessage::addLine(string line){
	//ts.save("SipMessage-creating header start");
	MRef<SipHeader*> hdr = SipHeader::parseHeader(line);
	//ts.save("SipMessage-creating header end");
	if( hdr.isNull() ) //do not add if null
		return false;
	addHeader(hdr);
	return true;
}


void SipMessage::setContent(MRef<SipMessageContent*> content){
	this->content=content;
	if( content ){
		string contentType = content->getContentType();
		if( contentType != "" ){
			MRef<SipHeaderValueContentType*> contenttypep = new SipHeaderValueContentType( contentType );
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
			return id->getString();
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
 	mdbg << "ERROR: Could not find command sequence number in sip Message."<< end;
	return -1;
}

string SipMessage::getViaHeaderBranch(bool first){
	string b;
	MRef<SipHeaderValueVia*> via = getViaHeader(first);

	if( !via.isNull() ){
		b = via->getParameter("branch");
	}
	
	return b;
}

MRef<SipHeaderValueVia*> SipMessage::getViaHeader(bool first){
	MRef<SipHeaderValueVia*> via;
	
	for (int32_t i=0; i< headers.size(); i++){
		if ((headers[i])->getType() == SIP_HEADER_TYPE_VIA){
			via = MRef<SipHeaderValueVia*>((SipHeaderValueVia*)*(headers[i]->getHeaderValue(0)));
			if (first)
				return via;
		}
	}
	return via;
}


MRef<SipHeaderValueVia*> SipMessage::getFirstVia(){
	return getViaHeader(true);
}

MRef<SipHeaderValueVia*> SipMessage::getLastVia(){
	return getViaHeader(false);
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
	mdbg << "ERROR: Could not find command sequence method in sip Message."<< end;
	return "";
}

SipUri SipMessage::getFrom(){
	SipUri ret;
	MRef<SipHeaderValueFrom*> hfrom = getHeaderValueFrom();
	if (hfrom)
		ret = hfrom->getUri();

	return ret;
}

SipUri SipMessage::getTo(){
	SipUri ret;
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
	MRef<SipHeader*> from = getHeaderOfType( SIP_HEADER_TYPE_FROM );

	if( from ){
		return MRef<SipHeaderValueFrom*>( (SipHeaderValueFrom*)*(from->getHeaderValue(0)) );
	}

	return NULL;
}

MRef<SipHeaderValueTo*> SipMessage::getHeaderValueTo(){
	MRef<SipHeader*> to = getHeaderOfType( SIP_HEADER_TYPE_TO );

	if( to ){
		return MRef<SipHeaderValueTo*>( (SipHeaderValueTo*)*(to->getHeaderValue(0)) );
	}

	return NULL;
}

MRef<SipHeaderValueContact*> SipMessage::getHeaderValueContact(){
	MRef<SipHeader *> h = getHeaderOfType( SIP_HEADER_TYPE_CONTACT );

	if( h ){
		return MRef<SipHeaderValueContact*>( 
			(SipHeaderValueContact*)*(h->getHeaderValue(0) ) );
	}
	return NULL;
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

string SipMessage::getWarningMessage(){
	for (uint32_t i = 0; i< (uint32_t)headers.size(); i++)
		if ((headers[i])->getType() == SIP_HEADER_TYPE_WARNING){
			string warning = ((SipHeaderValueWarning *)*(headers[i]->getHeaderValue(0)))->getWarning();
			return warning;
		}
	return "";
}

list<string> SipMessage::getRouteSet() {
	list<string> set;

	//merr << "CESC: SipMessage::getRouteSet() " << end;
	for( int i=0; i<headers.size(); i++ ) {
		if( headers[i]->getType() == SIP_HEADER_TYPE_RECORDROUTE ) {
			for( int j=0; j<headers[i]->getNoValues(); j++ ) {
				MRef<SipHeaderValueRecordRoute *> rr = (MRef<SipHeaderValueRecordRoute *>) ((SipHeaderValueRecordRoute *)*headers[i]->getHeaderValue(j));
				//merr << "CESC: SipMessage: Record-Route: (" << i << "," << j << ") : " << rr->getStringWithParameters() << end;
				set.push_back( rr->getStringWithParameters() );	
			}
			//merr << "CESC: SipMessage: Record-Route: " << headers[i]->getString() << end;
		} 
	}
	return set;
}


void SipMessage::setSocket(MRef<Socket*> sock)
{
	this->sock = sock;
}

MRef<Socket*> SipMessage::getSocket()
{
	return sock;
}

static string unquote(const string &value){
	if (value.size()>=2 && value[0]=='\"' && value[value.size()-1]=='\"'){
		return trim(value.substr(1,value.size()-2));
	}

	return value;
}

string SipMessage::getAuthenticateProperty(string prop){
        MRef<SipHeaderValue*> hdr;
        int i=0;

        do{
                hdr=getHeaderValueNo(SIP_HEADER_TYPE_WWWAUTHENTICATE, i++);
                if (hdr){
                        MRef<SipHeaderValueWWWAuthenticate*> whdr = (SipHeaderValueWWWAuthenticate*)*hdr;
                        if (whdr->hasParameter(prop))
				return unquote(whdr->getParameter(prop));
                }
        }while(hdr);

        i=0;
        do{
                hdr=getHeaderValueNo(SIP_HEADER_TYPE_PROXYAUTHENTICATE, i++);
                if (hdr){
                        MRef<SipHeaderValueProxyAuthenticate*> phdr = (SipHeaderValueProxyAuthenticate*)*hdr;
                        if (phdr->hasParameter(prop))
				return unquote(phdr->getParameter(prop));
                }
        }while(hdr);


        return "";
}


MRef<SipHeaderValueProxyAuthenticate*> SipMessage::getHeaderValueProxyAuthenticate(int i){
        MRef<SipHeaderValue*> hdr;

	hdr=getHeaderValueNo(SIP_HEADER_TYPE_PROXYAUTHENTICATE, i);
	if (hdr){
		MRef<SipHeaderValueProxyAuthenticate*> phdr = (SipHeaderValueProxyAuthenticate*)*hdr;
		return phdr;
	}
	return NULL;
}

MRef<SipHeaderValueWWWAuthenticate*> SipMessage::getHeaderValueWWWAuthenticate(int i){
        MRef<SipHeaderValue*> hdr;

	hdr=getHeaderValueNo(SIP_HEADER_TYPE_WWWAUTHENTICATE, i);
	if (hdr){
		MRef<SipHeaderValueWWWAuthenticate*> whdr = (SipHeaderValueWWWAuthenticate*)*hdr;
		return whdr;
	}

	return NULL;
}

bool SipMessage::requires(string extension){
	MRef<SipHeaderValue*> hval;
	bool ret=false;
	int i=0;
	bool done=false;

	do{
		hval = getHeaderValueNo(SIP_HEADER_TYPE_REQUIRE, i);
		if (hval){
			string e= ((SipHeaderValueRequire*)*hval)->getString();
			if (e==extension){
				ret=true;
				done=true;
			}
		}
		i++;
	}while(!done && hval);

	return ret;
}


bool SipMessage::supported(string extension){
	MRef<SipHeaderValue*> hval;
	bool ret=false;
	int i=0;
	bool done=false;

	do{
		hval = getHeaderValueNo(SIP_HEADER_TYPE_SUPPORTED, i);
		if (hval){
			string e= ((SipHeaderValueSupported*)*hval)->getString();
			if (e==extension){
				ret=true;
				done=true;
			}
		}
		i++;
	}while(!done && hval);

	return ret;
}


