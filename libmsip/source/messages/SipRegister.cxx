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
 * 	SipRegister.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#include<config.h>

#include"stdio.h" //sprintf
#include<libmsip/SipRegister.h>
#include<libmutil/vmd5.h>
#include<libmsip/SipMessage.h>

#include<libmsip/SipHeaderVia.h>
#include<libmsip/SipHeaderFrom.h>
#include<libmsip/SipHeaderTo.h>
#include<libmsip/SipHeaderCSeq.h>
#include<libmsip/SipHeaderCallID.h>
#include<libmsip/SipHeaderContact.h>
#include<libmsip/SipHeaderUserAgent.h>
#include<libmsip/SipHeaderContentType.h>
#include<libmsip/SipHeaderAuthorization.h>
#include<libmsip/SipHeaderMaxForwards.h>

const int SipRegister::type=2;

SipRegister::SipRegister(string branch,
		string call_id, 
		string domainarg, 
		string localIp, 
		int32_t sip_listen_port, 
		string from_tel_no, 
		int32_t seq_no,
		string transport
		) : SipMessage(branch, SipRegister::type), domain(domainarg)
{

	//SipHeaderVia *viap = new SipHeaderVia("UDP",localIp, atoi(sip_listen_port.c_str()));
	//add_header(viap);
	
	MRef<SipHeader*> fromp = new SipHeaderFrom(from_tel_no, domain);
	addHeader(fromp);
	
	MRef<SipHeader*> top = new SipHeaderTo(from_tel_no, domain);
	addHeader(top);
	
	MRef<SipHeader*> mf= new SipHeaderMaxForwards(70);
	addHeader(mf);
	
	MRef<SipHeaderCallID*> callidp = new SipHeaderCallID();
	callidp->setId(call_id);
	addHeader(MRef<SipHeader*>(*callidp));
	
	MRef<SipHeaderCSeq*> seqp = new SipHeaderCSeq();
	seqp->setMethod("REGISTER");
	seqp->setCSeq(seq_no);
	addHeader(MRef<SipHeader*>(*seqp));
	
	MRef<SipHeader*> contactp = new SipHeaderContact(from_tel_no, 
			localIp, 
			sip_listen_port,
			"phone",
			transport
			); //EE: append ";expires=3600" ?!
	
	//add P2T featuretag
	MRef<SipHeaderContact*>contactp_casted = MRef<SipHeaderContact*>((SipHeaderContact*) *contactp);
	contactp_casted->setFeatureTag("+sip.p2t=\"true\";");
			
	addHeader(contactp);
	
	MRef<SipHeaderUserAgent*> uap = new SipHeaderUserAgent();
	uap->setUserAgent("Minisip");
	addHeader(MRef<SipHeader*>(*uap));
}

SipRegister::SipRegister(string branch,
		string call_id, 
		string domainarg, 
		string localIp, 
		int32_t sip_listen_port, 
		string from_tel_no,
	    	int32_t seq_no, 
		string transport,
		string auth_id, 
		string realm, 
		string nonce, 
		string password
		): SipMessage(branch, SipRegister::type), domain(domainarg)
{

	SipURI uri("", localIp,"phone", sip_listen_port);
	 
	//SipHeaderVia *viap = new SipHeaderVia("UDP",localIp, atoi(sip_listen_port.c_str()));
	//add_header(viap);
	
	MRef<SipHeader*> fromp = new SipHeaderFrom(from_tel_no, domain);
	addHeader(fromp);
	
	MRef<SipHeader*> top = new SipHeaderTo(from_tel_no, domain);
	addHeader(top);
	
	MRef<SipHeader*> mf = new SipHeaderMaxForwards(70);
	addHeader(mf);
	
	MRef<SipHeaderCallID*> callidp = new SipHeaderCallID();
	callidp->setId(call_id);
	addHeader(MRef<SipHeader*>(*callidp));
	
	MRef<SipHeaderCSeq*> seqp = new SipHeaderCSeq();
	seqp->setMethod("REGISTER");
	seqp->setCSeq(seq_no);
	addHeader(MRef<SipHeader*>(*seqp));
	
	MRef<SipHeader*> contactp = new SipHeaderContact(from_tel_no, localIp, sip_listen_port,"phone",transport);
	addHeader(contactp);
	
	MRef<SipHeaderUserAgent*> uap = new SipHeaderUserAgent();
	uap->setUserAgent("Minisip");
	addHeader(MRef<SipHeader*>(*uap));
	
        MRef<SipHeader*> authp = 
		new SipHeaderAuthorization(
				"REGISTER",
				from_tel_no, 
				realm, 
				nonce, 
				uri, 
				auth_id, 
				password,
				"Digest");
	addHeader(authp);
	
	setContent(NULL);
}

SipRegister::~SipRegister(){

}

string SipRegister::getString(){
	return "REGISTER sip:"+domain+" SIP/2.0\r\n"+getHeadersAndContent();
}

