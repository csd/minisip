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
 * 	SipSubscribe.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#include<config.h>

#include<stdio.h> //sprintf
#include<stdlib.h> //rand...
#include<libmsip/SipSubscribe.h>
#include<libmutil/vmd5.h>
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
#include<libmsip/SipHeaderProxyAuthorization.h>
#include<libmsip/SipHeaderEvent.h>
#include<libmsip/SipHeaderAccept.h>

//#include"state_machines/DefaultCallHandler.h"

const int SipSubscribe::type=7;

SipSubscribe::SipSubscribe(string &build_from): SipMessage(/*"SipSubscribe",*/ SipSubscribe::type, build_from){
	
}

SipSubscribe::SipSubscribe(string branch,
		string call_id, 
		string resource, 
		IPAddress &proxy_addr, 
	//	IPAddress &local_addr, 
		string from_tel_no, 
		int32_t seq_no, 
		int32_t local_media_port): SipMessage(branch, SipSubscribe::type)
{
	this->resource = resource;
	ip=proxy_addr.getString();
	user_type="phone";
	
	//SipHeaderVia *viap = new SipHeaderVia("UDP",local_addr.get_string(),local_addr.get_port());
	//add_header(viap);
	
	MRef<SipHeader*> fromp = new SipHeaderFrom(from_tel_no, proxy_addr.getString() );
	addHeader(fromp);
	

	MRef<SipHeader*> top = new SipHeaderTo(resource, proxy_addr.getString());
	addHeader(top);
	
	MRef<SipHeaderCallID*> callidp = new SipHeaderCallID();
	callidp->setId(call_id);
	addHeader(MRef<SipHeader*>(*callidp));
	
	MRef<SipHeaderCSeq*> seqp = new SipHeaderCSeq();
	seqp->setMethod("SUBSCRIBE");
	seqp->setCSeq(seq_no);
	addHeader(MRef<SipHeader*>(*seqp));
	
	/*SipHeaderContact *contactp = new SipHeaderContact(
				from_tel_no, 
				local_addr.get_string(), 
				local_addr.get_port(),
				"phone",
				"UDP");
	add_header(contactp);
	*/
	MRef<SipHeaderUserAgent*> uap = new SipHeaderUserAgent();
	uap->setUserAgent("Minisip");
	addHeader(MRef<SipHeader*>(*uap));

	MRef<SipHeaderEvent*> ep = new SipHeaderEvent();
	ep->setEvent("presence");	
	addHeader(MRef<SipHeader*>(*ep));

	MRef<SipHeaderAccept*> ap = new SipHeaderAccept();
	ap->setAccept("application/xpidf+xml");
	addHeader(MRef<SipHeader*>(*ap));
}

SipSubscribe::~SipSubscribe(){

}

string SipSubscribe::getString(){
	return  "SUBSCRIBE sip:"+resource+"@"+ip+";user="+user_type+" SIP/2.0\r\n" + getHeadersAndContent();
}

