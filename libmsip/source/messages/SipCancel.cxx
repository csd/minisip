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
		//string localIp, 
		//string localSipPort, 
		bool local_called
		):SipMessage(branch, type)
{
	this->ipaddr=proxy;
	username = to_uri;

	//SipHeaderVia *viap = new SipHeaderVia("UDP",localIp, atoi(localSipPort.c_str()));
	//add_header(viap);
	
	MRef<SipHeaderFrom*> from;
	MRef<SipHeaderTo*> to;

	MRef<SipHeader*> mf = new SipHeaderMaxForwards(70);
	addHeader(mf);
	MRef<SipHeader *> hdr;
	int noHeaders = inv->getNoHeaders();
	for (uint32_t i=0; i< noHeaders; i++){
		hdr = inv->getHeader(i);
		int type = hdr->getType();
		if (type == SIP_HEADER_TYPE_VIA){
		//				add_header(inv->headers[i]);
		}
		if (type == SIP_HEADER_TYPE_FROM){
			from = MRef<SipHeaderFrom*>((SipHeaderFrom*)*hdr /*inv->getHeader(i)*//*headers[i]*/);
		//	from->set_tag("");
			from->setTag( ((SipHeaderFrom*)(*hdr))->getTag() );
			from->getUri().setUserId(from_uri);
			addHeader(hdr);
		}
		if (type == SIP_HEADER_TYPE_TO){
			to = MRef<SipHeaderTo*>((SipHeaderTo*)*hdr);
		//	to->set_tag("");
			to->setTag( ((SipHeaderTo*)(*hdr))->getTag() );
			to->getUri().setUserId(to_uri);
			addHeader(hdr);

		}
		if (type == SIP_HEADER_TYPE_CSEQ){
			MRef<SipHeaderCSeq*> seq = new SipHeaderCSeq();
			seq->setCSeq( ((SipHeaderCSeq *)*hdr)->getCSeq() );
			seq->setMethod("CANCEL");
			addHeader(MRef<SipHeader*>(*seq) );
//			merr <<"TMPDEBUG: seq no in CANCEL is parsed from inv to be "<<seq->get_cseq()<< end;
		}
		if (type == SIP_HEADER_TYPE_CALLID){
			addHeader(hdr);
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

