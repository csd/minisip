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
#include<libmsip/SipHeaderAcceptContact.h>
#include<libmsip/SipHeaderFrom.h>
#include<libmsip/SipHeaderTo.h>
#include<libmsip/SipHeaderCallID.h>
#include<libmsip/SipHeaderCSeq.h>
#include<libmsip/SipHeaderMaxForwards.h>
#include<libmutil/itoa.h>

const int SipAck::type=5;

SipAck::SipAck(string &build_from):SipRequest(type, build_from){

}

SipAck::SipAck(string branch, 
		MRef<SipMessage*> pack, 
		string to_tel_no, 
		string proxy): 
		SipRequest(branch, type, "ACK"){
	this->username = to_tel_no;
	this->ipaddr = proxy;

	setUri(to_tel_no);

	MRef<SipHeader*> mf = new SipHeader(new SipHeaderValueMaxForwards(70));
	addHeader(mf);
	int noHeaders = pack->getNoHeaders();
	for (int32_t i=0; i< noHeaders; i++){			//FIX: deep copy?
		MRef<SipHeader *> header = pack->getHeaderNo(i);
		int headerType = header->getType();
		switch (headerType){
			case SIP_HEADER_TYPE_CSEQ:
				((SipHeaderValueCSeq*) *(header->getHeaderValue(0)))->setMethod("ACK");
			case SIP_HEADER_TYPE_FROM:
			case SIP_HEADER_TYPE_TO:
			case SIP_HEADER_TYPE_CALLID:
				addHeader(header);
				break;
		}
	}
}

void SipAck::set_Conf() {
	this->Conf=true;
	MRef<SipHeaderValueAcceptContact*> acp = new SipHeaderValueAcceptContact("+sip.conf=\"TRUE\"",true,false);
	addHeader(new SipHeader(*acp) );
}
bool SipAck::is_Conf() {
	return Conf;
}

