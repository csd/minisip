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
 * 	SipCancel.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#include<config.h>

#include<libmsip/SipCancel.h>
#include<libmsip/SipInvite.h>
#include<assert.h>
#include<libmsip/SipHeaderVia.h>
#include<libmsip/SipHeaderFrom.h>
#include<libmsip/SipHeaderTo.h>
#include<libmsip/SipHeaderCSeq.h>
#include<libmsip/SipHeaderCallID.h>
#include<libmsip/SipHeaderMaxForwards.h>
#include<libmutil/dbg.h>

const int SipCancel::type=4;

SipCancel::SipCancel(string &resp):SipMessage(SipCancel::type, resp){
}

SipCancel::SipCancel(string branch, MRef<SipInvite*> inv, 
		string to_uri, 
		string from_uri, 
		string proxy, 
		bool local_called
		):SipMessage(branch, type)
{
	this->ipaddr=proxy;
	username = to_uri;

	MRef<SipHeaderValueFrom*> from;
	MRef<SipHeaderValueTo*> to;

	MRef<SipHeader*> mf = new SipHeader( new SipHeaderValueMaxForwards(70));
	addHeader(mf);
	MRef<SipHeader *> header;
	int noHeaders = inv->getNoHeaders();
	for (int32_t i=0; i< noHeaders; i++){
		header = inv->getHeaderNo(i);
		int type = header->getType();
		bool add=false;
		switch (type){
			case SIP_HEADER_TYPE_FROM:
				((SipHeaderValueFrom*)*(header->getHeaderValue(0)))->setTag(((SipHeaderValueFrom*)*(header->getHeaderValue(0)))->getTag());
				((SipHeaderValueFrom*)*(header->getHeaderValue(0)))->getUri().setUserId(from_uri);
				add=true;
				break;
			case SIP_HEADER_TYPE_TO:
				((SipHeaderValueTo*)*(header->getHeaderValue(0)))->setTag( ((SipHeaderValueTo*)*(header->getHeaderValue(0)))->getTag() );
				((SipHeaderValueTo*)*header)->getUri().setUserId(to_uri);
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
		}
		if (add){
			addHeader(header);
		}
	}
}

string SipCancel::getString(){

	string ret;
	if (username.find("sip:")==string::npos){
		ret = "CANCEL sip:";
	}else{
		ret = "CANCEL ";
	}
	ret = ret + username;
	if (username.find("@")==string::npos){
		ret = ret +"@"+ipaddr;
	}
	return ret + " SIP/2.0\r\n"+getHeadersAndContent();
}

