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
		string transport,
		int expires
	) : SipRequest(branch, SipRegister::type, "REGISTER"), domain(domainarg)
{
	SipURI fromUri;
	fromUri.setParams(from_tel_no, domain, "", 0);
	MRef<SipHeaderValue*> fromp = new SipHeaderValueFrom(fromUri);
	addHeader(new SipHeader(*fromp));
	
	MRef<SipHeaderValue*> top = new SipHeaderValueTo(fromUri);
	addHeader(new SipHeader(*top));
	
	MRef<SipHeaderValue*> mf= new SipHeaderValueMaxForwards(70);
	addHeader(new SipHeader(*mf));
	
	MRef<SipHeaderValueCallID*> callidp = new SipHeaderValueCallID(call_id);
	addHeader(new SipHeader(*callidp));
	
	MRef<SipHeaderValueCSeq*> seqp = new SipHeaderValueCSeq("REGISTER", seq_no);
	addHeader(new SipHeader(*seqp));
	
	MRef<SipHeaderValue*> contactp = new SipHeaderValueContact(from_tel_no, 
			localIp, 
			sip_listen_port,
			"",
			transport
			); //EE: append ";expires=3600" ?!
	
	//add P2T featuretag
	MRef<SipHeaderValueContact*>contactp_casted = MRef<SipHeaderValueContact*>((SipHeaderValueContact*) *contactp);
	//contactp_casted->setFeatureTag("+sip.confjoin=\"TRUE\"");
	contactp_casted->setFeatureTag("+sip.p2t=\"true\"");		
	contactp_casted->setExpires(expires);
	addHeader(new SipHeader(*contactp));
	
	MRef<SipHeaderValueUserAgent*> uap = new SipHeaderValueUserAgent(HEADER_USER_AGENT_DEFAULT);
	addHeader(new SipHeader(*uap));

	setUri("sip:" + domain);
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
		string password,
		int expires
	): SipRequest(branch, SipRegister::type, "REGISTER"), domain(domainarg)
{
	SipURI uri;
	uri.setParams("", localIp,"", sip_listen_port);
	 
	SipURI fromUri;
	fromUri.setParams(from_tel_no, domain, "", 0);
	MRef<SipHeaderValue*> fromp = new SipHeaderValueFrom(fromUri);
	addHeader(new SipHeader(fromp));
	
	MRef<SipHeaderValue*> top = new SipHeaderValueTo(fromUri);
	addHeader(new SipHeader(top));
	
	MRef<SipHeaderValue*> mf = new SipHeaderValueMaxForwards(70);
	addHeader(new SipHeader(*mf));
	
	MRef<SipHeaderValueCallID*> callidp = new SipHeaderValueCallID(call_id);
	addHeader(new SipHeader(*callidp));
	
	MRef<SipHeaderValueCSeq*> seqp = new SipHeaderValueCSeq("REGISTER", seq_no);
	addHeader(new SipHeader(*seqp));
	
	MRef<SipHeaderValue*> contactp = new SipHeaderValueContact(from_tel_no, localIp, sip_listen_port,"",transport, expires);
	addHeader(new SipHeader(*contactp));
	
	MRef<SipHeaderValueUserAgent*> uap = new SipHeaderValueUserAgent(HEADER_USER_AGENT_DEFAULT);
	addHeader(new SipHeader(*uap));
	
        MRef<SipHeaderValue*> authp = 
		new SipHeaderValueAuthorization(
				"REGISTER",
				from_tel_no, 
				realm, 
				nonce, 
				uri, 
				auth_id, 
				password,
				"Digest");
	addHeader(new SipHeader(*authp));
	
	setContent(NULL);

	setUri("sip:" + domain);
}

SipRegister::~SipRegister(){

}

