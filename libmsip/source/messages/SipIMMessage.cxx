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
 * 	SipIMMessage.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#include<config.h>


#include<libmsip/SipIMMessage.h>
#include<libmsip/SipMessageContentIM.h>
#include<libmutil/itoa.h>

#include<libmsip/SipHeaderVia.h>
#include<libmsip/SipHeaderFrom.h>
#include<libmsip/SipHeaderTo.h>
#include<libmsip/SipHeaderCSeq.h>
#include<libmsip/SipHeaderCallID.h>
#include<libmsip/SipHeaderUserAgent.h>
#include<libmsip/SipHeaderContentType.h>
#include<libmsip/SipHeaderMaxForwards.h>
#include<libmsip/SipHeaderProxyAuthorization.h>
#include<libmsip/SipHeaderEvent.h>
#include<libmsip/SipHeaderAccept.h>

const int SipIMMessage::type=9;

SipIMMessage::SipIMMessage(string &build_from): SipMessage(SipIMMessage::type, build_from){
	
}

SipIMMessage::SipIMMessage(string branch, 
		    string call_id, 
		    MRef<SipIdentity*> toIdentity,
		    MRef<SipIdentity*> fromIdentity,
                    int local_port, 
                    int32_t seq_no, string msg): 
                            SipMessage(branch, SipIMMessage::type),
			    fromIdentity(fromIdentity)
{
	
	toUser = toIdentity->sipUsername;
	toDomain = toIdentity->sipDomain;
	
//	this->user = tel_no;
//	ip = proxyString;

	//	ip=proxy_addr.getString();
	
	MRef<SipHeader*> fromp = new SipHeaderFrom(/*from_resource*/ fromIdentity->sipUsername, /*proxyString*/ fromIdentity->sipDomain );
	addHeader(fromp);

	MRef<SipHeader*> top = new SipHeaderTo(toUser, toDomain);
	addHeader(top);
	
	MRef<SipHeader*> mf = new SipHeaderMaxForwards(70);
	addHeader(mf);

	MRef<SipHeaderCallID*> callidp = new SipHeaderCallID();
	callidp->setId(call_id);
	addHeader(MRef<SipHeader*>(*callidp));
	
	MRef<SipHeaderCSeq*> seqp = new SipHeaderCSeq();
	seqp->setMethod("MESSAGE");
	seqp->setCSeq(seq_no);
	addHeader(MRef<SipHeader*>(*seqp));
	
	
	MRef<SipHeaderUserAgent*> uap = new SipHeaderUserAgent();
	uap->setUserAgent("Minisip");
	addHeader(MRef<SipHeader*>(*uap));

	setContent(new SipMessageContentIM(msg));

//	MRef<SipHeaderContentType*> contenttypep = new SipHeaderContentType();
//	contenttypep->setContentType("text/plain");
//	addHeader(MRef<SipHeader*>(*contenttypep));
}

SipIMMessage::~SipIMMessage(){

}

string SipIMMessage::getString(){
	return  "MESSAGE sip:"+toUser+"@"+toDomain+" SIP/2.0\r\n" + getHeadersAndContent();

}

