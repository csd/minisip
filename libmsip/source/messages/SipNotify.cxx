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
 * 	SipNotify.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#include<config.h>


#include<stdio.h> //sprintf
#include<stdlib.h> //rand...
#include<libmsip/SipNotify.h>
#include<libmutil/vmd5.h>
//#include<sys/socket.h>
//#include<netinet/in.h>
#include<libmsip/SipMessage.h>
//#include"../minisip/SoundSender.h"
#include<libmutil/itoa.h>

#include<libmsip/SipHeaderVia.h>
#include<libmsip/SipHeaderFrom.h>
#include<libmsip/SipHeaderTo.h>
#include<libmsip/SipHeaderCSeq.h>
#include<libmsip/SipHeaderCallID.h>
#include<libmsip/SipHeaderContact.h>
#include<libmsip/SipHeaderUserAgent.h>
#include<libmsip/SipHeaderContentType.h>
#include<libmsip/SipHeaderMaxForwards.h>
#include<libmsip/SipHeaderProxyAuthorization.h>
#include<libmsip/SipHeaderEvent.h>
#include<libmsip/SipHeaderAccept.h>

//#include"state_machines/DefaultCallHandler.h"

const int SipNotify::type=6;

SipNotify::SipNotify(string &build_from): SipMessage(/*"SipNotify",*/ SipNotify::type, build_from){
	
}

SipNotify::SipNotify(string branch, 
		    string call_id, 
		    MRef<SipIdentity*> toIdentity,
		    MRef<SipIdentity*> fromId,
                    int local_port, 
                    int32_t seq_no 
                    ): 
                            SipMessage(branch, SipNotify::type), fromIdentity(fromId)
{
	toUser = toIdentity->sipUsername;
	toDomain = toIdentity->sipDomain;
	
	MRef<SipHeader*> fromp = new SipHeaderFrom(fromIdentity->sipUsername, fromIdentity->sipDomain );
	addHeader(fromp);

	MRef<SipHeader*> top = new SipHeaderTo(toIdentity->sipUsername, toIdentity->sipDomain);
	addHeader(top);
	
	MRef<SipHeader*> mf = new SipHeaderMaxForwards(70);
	addHeader(mf);

	MRef<SipHeaderCallID*> callidp = new SipHeaderCallID();
	callidp->setId(call_id);
	addHeader(MRef<SipHeader*>(*callidp));
	
	MRef<SipHeaderCSeq*> seqp = new SipHeaderCSeq();
	seqp->setMethod("NOTIFY");
	seqp->setCSeq(seq_no);
	addHeader(MRef<SipHeader*>(*seqp));
	
/*
	MRef<SipHeader*> contactp = new SipHeaderContact(from_resource, 
                                                        local_addr.getString(), 
                                                        local_port,
                                                        "phone",
                                                        "UDP");
	addHeader(contactp);
*/
	
	MRef<SipHeaderUserAgent*> uap = new SipHeaderUserAgent();
	uap->setUserAgent("Minisip");
	addHeader(MRef<SipHeader*>(*uap));

	MRef<SipHeaderEvent*> ep = new SipHeaderEvent();
	ep->setEvent("presence");	
	addHeader(MRef<SipHeader*>(*ep));
}

SipNotify::~SipNotify(){

}

string SipNotify::getString(){
	return  "NOTIFY sip:"+toUser+"@"+toDomain+" SIP/2.0\r\n" + getHeadersAndContent();

}

