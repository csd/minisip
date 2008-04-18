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
#include<libmsip/SipMessageContentUnknown.h>
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
	size_t start = 0;

	while ( start<n && isWS(data[start]))
		start++;

	if (n>3   &&    (data[start+0]=='S'||data[start+0]=='s') &&
			(data[start+1]=='I'||data[start+1]=='i') &&
			(data[start+2]=='P'||data[start+2]=='p' )){
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



SipMessage::SipMessage(){
}


void SipMessage::addHeader(MRef<SipHeader*> header){
	if( header.isNull() ) {
		merr << "ERROR: trying to add null header to message!"<<endl;
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
	MRef<SipHeaderValue*> cl = getHeaderValueNo( SIP_HEADER_TYPE_CONTENTLENGTH, 0 );
	if (cl){
		return ((SipHeaderValueContentLength*)*cl)->getContentLength();
	}else{
		return 0;
	}
}

string SipMessage::getHeadersAndContent() const{
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
		mdbg("signaling/sip") << "SipMessage::parseHeaders: Info: SipMessage without headers ... only request line" << endl;
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
			mdbg("signaling/sip") << "SipMessage::parseHeaders: Info: Could not copy line to new Message: (empty line)" << endl;
			#endif
		} else if (!addLine(header)){
			#ifdef DEBUG_OUTPUT
			mdbg("signaling/sip") << "SipMessage::parseHeaders: Info: Could not copy line to new Message: " << header << " (unknown)" << endl;
			#endif
		}
		i=eoh+1;
	}while (i<endBuf);
	
	return i;
}

SipMessage::SipMessage(string &buildFrom)
{
	uint32_t i;

	uint32_t start=0;
	uint32_t blen=buildFrom.size();

	//skip whitespace (can be there if received over reliable transport)
	while (start<blen && isWS(buildFrom[start]))
		start++;

	//Skip first line (they are parsed by sub-class)
	for (i=start; buildFrom[i]!='\r' && buildFrom[i]!='\n'; i++){
		if(i==blen){
#ifdef DEBUG_OUTPUT
			cerr << "SipMessage::SipMessage: Size is too short - throwing exception"<< endl;
#endif
			throw SipExceptionInvalidMessage("SIP Message too short");
		}
	}
	
	int contentStart = parseHeaders(buildFrom, i);
	int clen = getContentLength();
	if (clen>0){
		string contentbuf=buildFrom.substr(contentStart, clen);
		if ((int)contentbuf.length() != clen){
			cerr << "WARNING: Length of content was shorter than expected (" << clen <<"!="<<(int)contentbuf.length()<<")"<<endl;
		}
		MRef<SipHeader*> h = getHeaderOfType(SIP_HEADER_TYPE_CONTENTTYPE);
		if (h){	
			string contentType = ((SipHeaderValueString*)*(h->getHeaderValue(0) ))->getString();
//			string b = (SipHeaderValueContentType*)*(h->getHeaderValue(0) )->getParameter("boundary");
//cerr <<"boundary="<< b <<endl;
			SipMessageContentFactoryFuncPtr contentFactory = contentFactories.getFactory( contentType );
			if (contentFactory){
				MRef<SipMessageContent*> smcref = contentFactory(contentbuf, contentType );
				setContent( smcref );
			}else{
				setContent( new SipMessageContentUnknown( contentbuf, contentType ));
			}
			
		}else{
			merr << "WARNING: unknown content type"<<endl;
			setContent( new SipMessageContentUnknown( contentbuf, "unknown" ));
		}
	}
}




bool SipMessage::addLine(string line){
	MRef<SipHeader*> hdr = SipHeader::parseHeader(line);
	if( hdr.isNull() )
		return false;
	addHeader(hdr);
	return true;
}

void SipMessage::setContent(MRef<SipMessageContent*> c){
	this->content=c;
	if( content ){
		string contentType = content->getContentType();
		if( contentType != "" ){
			MRef<SipHeaderValue*> hdr = getHeaderValueNo(SIP_HEADER_TYPE_CONTENTTYPE, 0);

			if( hdr ){
				MRef<SipHeaderValueContentType*> contentTypeHdr = (SipHeaderValueContentType*)*hdr;
				contentTypeHdr->setString(contentType);
			} else{
				MRef<SipHeaderValue*> contenttypep = new SipHeaderValueContentType( contentType );
				addHeader( new SipHeader(contenttypep) );
			}
		}
	}
}

MRef<SipMessageContent*> SipMessage::getContent(){
	return content;
}

string SipMessage::getCallId(){
	MRef<SipHeaderValue*> id = getHeaderValueNo( SIP_HEADER_TYPE_CALLID, 0 );
	if (id)
		return ((SipHeaderValue*)*id)->getString();
	else
		return "";
}

int32_t  SipMessage::getCSeq(){
	MRef<SipHeaderValue*> seq = getHeaderValueNo( SIP_HEADER_TYPE_CSEQ, 0 );
	if (seq){
		return ((SipHeaderValueCSeq*)*seq)->getCSeq();
	}else{
		mdbg("signaling/sip") << "ERROR: Could not find command sequence number in sip Message."<< endl;
		return -1;
	}
}

MRef<SipHeaderValueVia*> SipMessage::getFirstVia(){
	MRef<SipHeaderValue*> via = getHeaderValueNo( SIP_HEADER_TYPE_VIA, 0 );
	if (via)
		return (SipHeaderValueVia*) *via;
	else
		return NULL;
}

void SipMessage::removeFirstVia(){
	MRef<SipHeader*> hdr = getHeaderOfType( SIP_HEADER_TYPE_VIA, 0 );
	if( hdr->getNoValues() > 1 ){
		hdr->removeHeaderValue( 0 );
	} else{
		removeHeader( hdr );
	}
}

void SipMessage::removeHeaderValue(MRef<SipHeaderValue*> hval){
	int hi=0;
	MRef<SipHeader*> hdr;
	for (; hdr=getHeaderOfType( hval->getType(), hi ) ; hi++ ){
		for (int vi=0; vi<hdr->getNoValues(); vi++){
			if (hval== hdr->getHeaderValue(vi) ){
				if (hdr->getNoValues()>1){
					hdr->removeHeaderValue(vi);
				}else{
					removeHeader(hdr);
				}
			}
			
		}
	
	}
}

string SipMessage::getBranch(){
	MRef<SipHeaderValue*> firstVia = getHeaderValueNo( SIP_HEADER_TYPE_VIA, 0 );
	if (firstVia){
		return firstVia->getParameter("branch");
	}else
		return "";
}

string SipMessage::getCSeqMethod(){
	MRef<SipHeaderValue*> seq = getHeaderValueNo( SIP_HEADER_TYPE_CSEQ, 0 );
	if (seq){
		return ((SipHeaderValueCSeq*)*seq)->getMethod();
	}else{
		mdbg("signaling/sip") << "ERROR: Could not find command sequence method in sip Message."<< endl;
		return "";
	}
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

void SipMessage::removeHeader(MRef<SipHeader*> header){
	headers.remove( header );
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
		return (SipHeaderValueContact*)*(h->getHeaderValue(0) );
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

	MRef<SipHeaderValue*> warn = getHeaderValueNo( SIP_HEADER_TYPE_WARNING, 0 );
	if (warn)
		return ((SipHeaderValueWarning*)*warn)->getWarning();
	else{
		return "";
	}
}

list<string> SipMessage::getRouteSet() {
	list<string> set;
	int n=0;
	MRef<SipHeaderValue*> rr;
	while ( ! (rr=getHeaderValueNo(SIP_HEADER_TYPE_RECORDROUTE,n++)).isNull() ){
		set.push_back( ((SipHeaderValueRecordRoute*)*rr)->getStringWithParameters() );
	}

	return set;
}


void SipMessage::setSocket(MRef<Socket*> s)
{
	this->sock = s;
}

MRef<Socket*> SipMessage::getSocket()
{
	return sock;
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


list<string> SipMessage::getRequired() {
	list<string> set;
	int n=0;
	MRef<SipHeaderValue*> rr;
	while ( ! (rr=getHeaderValueNo(SIP_HEADER_TYPE_REQUIRE,n++)).isNull() ){
		set.push_back( ((SipHeaderValueRequire*)*rr)->getString() );
	}

	return set;
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


void SipMessage::addBefore(MRef<SipHeader*> h){
	int htype = h->getType();
	int i;
	int pos = 0;
	
	for( i = 0; i < headers.size(); i++ ) {
		if( headers[i]->getType() == htype) {
			pos = i;
			break;
		}
	}

	headers.insert( pos, h );	
}


