/*
  Copyright (C) 2005 Mikael Magnusson, Erik Eliasson
  
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
 * Author(s): Mikael Magnusson <mikma@users.sourceforge.net>
 *            Erik Eliasson <eliasson@it.kth.se>
*/


/* Name
 * 	SipRequest.cxx
 * Author
 *      Mikael Magnusson, mikma@users.sourceforge.net
 *      Erik Eliasson, eliasson@it.kth.se 
 * Purpose
 *      Any SIP request request to libmsip
*/

#include<config.h>

#include<libmsip/SipStack.h>
#include<libmsip/SipRequest.h>
#include<libmsip/SipException.h>
#include"../SipCommandDispatcher.h"
#include<libmsip/SipMessageContentIM.h>
#include<libmsip/SipHeader.h>
#include<libmsip/SipHeaderRoute.h>
#include<libmsip/SipHeaderEvent.h>
#include<libmsip/SipHeaderFrom.h>
#include<libmsip/SipHeaderTo.h>
#include<libmsip/SipHeaderCallID.h>
#include<libmsip/SipHeaderCSeq.h>
#include<libmsip/SipHeaderMaxForwards.h>
#include<libmsip/SipHeaderUserAgent.h>
#include<libmsip/SipHeaderProxyAuthorization.h>
#include<libmsip/SipHeaderAccept.h>
#include<libmsip/SipHeaderContact.h>
#include<libmsip/SipHeaderReferTo.h>
#include<libmsip/SipHeaderSupported.h>

#include<stdio.h>

using namespace std;

MRef<SipRequest*> SipRequest::createSipMessageAck( MRef<SipRequest*> origReq,
		MRef<SipResponse*> resp,
		bool provisional)
{
	string method;
	if (provisional){
		method = "PRACK";
	}else{
		method = "ACK";
	}
	MRef<SipRequest*> req = new SipRequest(method, origReq->getUri() );

	req->addHeader(new SipHeader(new SipHeaderValueMaxForwards(70)));
	
	int noHeaders = origReq->getNoHeaders();
	for (int32_t i=0; i< noHeaders; i++){			//FIX: deep copy?
		MRef<SipHeader *> header = origReq->getHeaderNo(i);
		int headerType = header->getType();
		switch (headerType){
			case SIP_HEADER_TYPE_CSEQ:
				((SipHeaderValueCSeq*) *(header->getHeaderValue(0)))->setMethod(method);
			case SIP_HEADER_TYPE_FROM:
			case SIP_HEADER_TYPE_CALLID:
			case SIP_HEADER_TYPE_ROUTE:
			case SIP_HEADER_TYPE_AUTHORIZATION:
			case SIP_HEADER_TYPE_PROXYAUTHORIZATION:
				req->addHeader(header);
				break;
			default:
				break;
		}
	}

	req->addHeader( new SipHeader( *resp->getHeaderValueTo() ) );
	
	return req;
}


MRef<SipRequest*> SipRequest::createSipMessageCancel( MRef<SipRequest*> r )
{
	MRef<SipRequest*> req = new SipRequest("CANCEL", r->getUri());

	req->addHeader(new SipHeader( new SipHeaderValueMaxForwards(70)));
	
	MRef<SipHeader *> header;
	int noHeaders = r->getNoHeaders();
	for (int32_t i=0; i< noHeaders; i++){
		header = r->getHeaderNo(i);
		int type = header->getType();
		bool add=false;
		switch (type){
			case SIP_HEADER_TYPE_FROM:
				add=true;
				break;
			case SIP_HEADER_TYPE_TO:
				add=true;
				break;
			case SIP_HEADER_TYPE_CSEQ:
				((SipHeaderValueCSeq*)*(header->getHeaderValue(0)))->setCSeq(  ((SipHeaderValueCSeq *)*(header->getHeaderValue(0)))->getCSeq() );
				((SipHeaderValueCSeq*)*(header->getHeaderValue(0)))->setMethod("CANCEL");
				add=true;
				break;
			case SIP_HEADER_TYPE_CALLID:
				add=true;
				break;

			// CANCEL requests must have the same branch
			// parameter as the transaction it cancels.
			// If CANCEL packets are treated as any other
			// request, then they would be assigned a random
			// branch. When we create the CANCEL request (how)
			// we therefore copy the Via header to indicate
			// which transaction it cancels. The
			// transport layer must make sure to not add it again.
			case SIP_HEADER_TYPE_VIA:
			case SIP_HEADER_TYPE_AUTHORIZATION:
			case SIP_HEADER_TYPE_PROXYAUTHORIZATION:
				add=true;
				break;
			default:
				break;
		}
		if (add){
			req->addHeader(header);
		}
	}
	
	
	return req;
}


MRef<SipRequest*> SipRequest::createSipMessageIMMessage(const string& callId,
							const SipUri& toUri,
							const SipUri& fromUri,
							int32_t seqNo,
							const string& msg)
{
	MRef<SipRequest*> req = new SipRequest("MESSAGE", toUri);
	req->addDefaultHeaders(fromUri, toUri, seqNo, callId);
	req->addHeader(new SipHeader(new SipHeaderValueUserAgent(HEADER_USER_AGENT_DEFAULT)));
	req->setContent(new SipMessageContentIM(msg));
	return req;
}

static void addHeaders( MRef<SipRequest*> req,
		const SipUri &contact,
		MRef<SipStack*> stack
		)
{

	req->addHeader(new SipHeader(new SipHeaderValueContact(contact)));
	req->addHeader(new SipHeader(new SipHeaderValueUserAgent(HEADER_USER_AGENT_DEFAULT)));
	if (stack){
		req->addHeader(new SipHeader(new SipHeaderValueSupported(stack->getAllSupportedExtensionsStr())));
	}
}

MRef<SipRequest*> SipRequest::createSipMessageInvite( const string &call_id,
							const SipUri &toUri,
							const SipUri &fromUri,
							const SipUri &contact,
							int32_t seq_no,
							MRef<SipStack*> stack
                )
{
	MRef<SipRequest*> req = new SipRequest("INVITE", toUri);

	req->addDefaultHeaders( fromUri, toUri, seq_no, call_id );
	addHeaders(req, contact, stack);

	return req;
}

MRef<SipRequest*> SipRequest::createSipMessageNotify( const string& callId,
							const SipUri& toUri,
							const SipUri& fromUri,
							int32_t seqNo
							)
{
	MRef<SipRequest*> req = new SipRequest("NOTIFY", toUri);
	req->addDefaultHeaders(fromUri, toUri, seqNo,callId);
	req->addHeader(new SipHeader(new SipHeaderValueUserAgent(HEADER_USER_AGENT_DEFAULT)));
	req->addHeader(new SipHeader(new SipHeaderValueEvent("presence")));
	return req;
}

MRef<SipRequest*> SipRequest::createSipMessageRegister( const string &call_id,
							const SipUri &registrar,
							const SipUri &fromUri,
						       MRef<SipHeaderValueContact *> contactHdr,
						       int32_t seq_no)
{
	MRef<SipRequest*> req = new SipRequest("REGISTER", registrar);

	req->addDefaultHeaders(fromUri, fromUri, seq_no, call_id);

	req->addHeader(new SipHeader(*contactHdr));
	req->addHeader(new SipHeader(new SipHeaderValueUserAgent(HEADER_USER_AGENT_DEFAULT)));
	req->setContent(NULL);

	return req;
}


MRef<SipRequest*> SipRequest::createSipMessageSubscribe(const string &call_id,
							const SipUri &toUri,
							const SipUri &fromUri,
							const SipUri &contact,
							int32_t seq_no)
{
	MRef<SipRequest*> req = new SipRequest("SUBSCRIBE", toUri );

	req->addDefaultHeaders(fromUri, toUri, seq_no, call_id);
	req->addHeader(new SipHeader(new SipHeaderValueContact(contact)));
	req->addHeader(new SipHeader(new SipHeaderValueEvent("presence")));
	req->addHeader(new SipHeader(new SipHeaderValueAccept("application/xpidf+xml")));
						
	return req;
}

void SipRequest::addDefaultHeaders(const SipUri& fromUri,
		const SipUri& toUri,
		int seqNo,
		const string& callId)
{
	addHeader(new SipHeader(new SipHeaderValueFrom(fromUri)));
	addHeader(new SipHeader(new SipHeaderValueTo(toUri)));
	addHeader(new SipHeader(new SipHeaderValueCallID(callId)));
	addHeader(new SipHeader(new SipHeaderValueCSeq(method, seqNo)));
	addHeader(new SipHeader(new SipHeaderValueMaxForwards(70)));
}


SipRequest::SipRequest(const string &method_,
		       const SipUri &uri_) :
		method(method_),
		uri(uri_)
{
}

SipRequest::SipRequest(string &build_from): SipMessage(build_from){
	init(build_from);
}

void SipRequest::init(string &build_from){
	size_t start = 0;
	size_t pos;
	size_t pos2;
	size_t end = 0;
	//int length = build_from.length();
	string requestLine;

	// Skip white space
	start = build_from.find_first_not_of( " \r\n\t", start );
	if( (int)start == (int)string::npos ){
		throw SipExceptionInvalidMessage("SipRequest malformed - first line did not contain any non whitespace character");
	}

	end = build_from.find_first_of( "\r\n", start );
	if( (int)end == (int)string::npos ){
		throw SipExceptionInvalidMessage("SipRequest malformed - only one line");
	}

	requestLine = build_from.substr( start, end - start );
	start = 0;
	end = requestLine.length();

	// Parse method
	pos = requestLine.find( ' ', start );
	if( (int)pos == (int)string::npos ){
		throw SipExceptionInvalidMessage("SipRequest malformed - could not find method");
	}

	method = requestLine.substr( start, pos - start );

	// Parse version
	pos2 = requestLine.rfind( ' ', end - 1 );
	if( (int)pos2 == (int)string::npos ){
		throw SipExceptionInvalidMessage("SipRequest malformed - request line did not contain space between method and version");
	}

	string version = requestLine.substr( pos2 + 1, end - pos2 );

	if( version != "SIP/2.0" ){
		throw SipExceptionInvalidMessage("SipRequest malformed - unknown version");
	}	  

	pos2 = requestLine.find_last_not_of( ' ', pos2 );

	uri = requestLine.substr( pos + 1, pos2 - pos );
	if( !uri.isValid() ){
		throw SipExceptionInvalidMessage("SipRequest malformed - uri");
	}
}

SipRequest::~SipRequest(){
}

const string& SipRequest::getType(){
	return method;
}

string SipRequest::getString() const{
	return getMethod() + " " + getUri().getRequestUriString() + " SIP/2.0\r\n"
		+ getHeadersAndContent();
}


void SipRequest::setMethod(const string &m){
	this->method = m;
}

string SipRequest::getMethod() const{
	return method;
}

void SipRequest::setUri(const SipUri &u){
	this->uri = u;
}

const SipUri &SipRequest::getUri() const{
	return uri;
}

void SipRequest::addRoute(const string &route)
{
	MRef<SipHeaderValue*> routeValue = (SipHeaderValue*)new SipHeaderValueRoute( route );
	MRef<SipHeader*> routeHdr = new SipHeader( routeValue );

	addBefore(routeHdr);
}

void SipRequest::addRoute(const string &addr, int32_t port,
			  const string &transport)
{
	string u = "<sip:" + addr;

	if( port ){
		char buf[20];
		sprintf(buf, "%d", port);
		u = u + ":" + buf;
	}

	if( transport != "" ){
		u = u + ";transport=" + transport;
	}

	u = u + ";lr>";
	
	addRoute( u );
}

void SipRequest::addRoutes(const list<SipUri> &routes){
	list<SipUri>::const_reverse_iterator i;
	list<SipUri>::const_reverse_iterator first=routes.rend();

	for( i = routes.rbegin(); i != first; i++ ){
		const SipUri &route = *i;

		addRoute( route.getString() );
	}
}

MRef<SipHeaderValueRoute*> SipRequest::getFirstRoute()
{
	MRef<SipHeaderValueRoute*> route;
	MRef<SipHeader*> header = getHeaderOfType(SIP_HEADER_TYPE_ROUTE, 0);

	if( header ){
		route = MRef<SipHeaderValueRoute*>((SipHeaderValueRoute*)*(header->getHeaderValue(0)));
	}
	return route;
}

void SipRequest::removeFirstRoute(){
	MRef<SipHeader*> hdr = getHeaderOfType(SIP_HEADER_TYPE_ROUTE, 0);

	if( hdr->getNoValues() > 1 ){
		hdr->removeHeaderValue( 0 );
	}
	else{
		removeHeader( hdr );
	}
}
