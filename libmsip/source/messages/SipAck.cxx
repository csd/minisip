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
 * 	SipAck.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#include<config.h>

#include<libmsip/SipAck.h>
#include<assert.h>
#include<libmsip/SipHeaderVia.h>
#include<libmsip/SipHeaderFrom.h>
#include<libmsip/SipHeaderTo.h>
#include<libmsip/SipHeaderCallID.h>
#include<libmsip/SipHeaderCSeq.h>
#include<libmsip/SipHeaderMaxForwards.h>
#include<libmutil/itoa.h>

const int SipAck::type=5;

SipAck::SipAck(string &build_from):SipMessage(type, build_from){

}

SipAck::SipAck(string branch, MRef<SipMessage*> pack, string to_tel_no, string proxy): SipMessage(branch, type){

	this->username = to_tel_no;
	//this->port = atoi(local_sip_port.c_str());
	this->ipaddr = proxy/*.get_string()*/;
	
	MRef<SipHeader*> mf = new SipHeaderMaxForwards(70);
	addHeader(mf);

//	SipMessage* msg = *pack;

	for (uint32_t i=0; i< pack->getNoHeaders()/*headers.size()*/; i++){			//FIX: deep copy
		if ((pack->getHeader(i)/*headers[i]*/)->getType() == SIP_HEADER_TYPE_FROM){
			addHeader(pack->getHeader(i)/*headers[i]*/);
		}
		if ((pack->getHeader(i)/*headers[i]*/)->getType() == SIP_HEADER_TYPE_TO)
			addHeader(pack->getHeader(i)/*headers[i]*/);
		if ((pack->getHeader(i)/*headers[i]*/)->getType() == SIP_HEADER_TYPE_CALLID)
			addHeader(pack->getHeader(i)/*headers[i]*/);
		if ((pack->getHeader(i)/*headers[i]*/)->getType() == SIP_HEADER_TYPE_CSEQ){
			MRef<SipHeaderCSeq*> seq = MRef<SipHeaderCSeq*>((SipHeaderCSeq *)(*pack->getHeader(i)/*headers[i]*/));
			seq->setMethod("ACK");
			addHeader(MRef<SipHeader*>(*seq));
		}
//		if (dynamic_cast<SipHeaderVia *>(pack.headers[i])!=NULL){
//			add_header(pack.headers[i]);
//		}
	}
};

string SipAck::getString(){
	string ret;
	if (username.find("sip:")==string::npos)
		ret = "ACK sip:";
	else
		ret = "ACK ";

	if (username.find("@")==string::npos){
		ret = ret + username + "@"+ipaddr;
	}else{
		ret = ret + username;
	}
	
	ret = ret + /*":" + itoa(port) +*/ ";user=phone SIP/2.0\r\n";
	ret = ret + getHeadersAndContent();
	return ret;
}



