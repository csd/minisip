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
 * 	SipInvite.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#include<config.h>

#include<stdio.h> //sprintf
#include<stdlib.h> //rand...
#include<libmsip/SipInvite.h>
#include<libmutil/vmd5.h>
//#include<sys/socket.h>
//#include<netinet/in.h>
#include<libmsip/SipMessage.h>
//#include"state_machines/DefaultCallHandler.h"
//#include"../minisip/SoundSender.h"
#include<libmutil/itoa.h>
#include<libmutil/dbg.h>

#include<libmsip/SipHeaderAcceptContact.h>
#include<libmsip/SipHeaderVia.h>
#include<libmsip/SipHeaderFrom.h>
#include<libmsip/SipHeaderTo.h>
#include<libmsip/SipHeaderCSeq.h>
#include<libmsip/SipHeaderCallID.h>
#include<libmsip/SipHeaderContact.h>
#include<libmsip/SipHeaderUserAgent.h>
#include<libmsip/SipHeaderContentType.h>
#include<libmsip/SipHeaderProxyAuthorization.h>
#include<libmsip/SipHeaderMaxForwards.h>
//#include<libmsip/CODECInterface.h>
#include<libmsip/SipMessageContent.h>

const int SipInvite::type=1;

SipInvite::SipInvite(string &build_from): SipMessage(SipInvite::type, build_from){

	//check if it is a P2T Invite packet
	MRef<SipHeaderValueAcceptContact*> acp;
	P2T=false;
	
	for (int32_t i=0; i< headers.size(); i++){
		if ((headers[i])->getType() == SIP_HEADER_TYPE_ACCEPTCONTACT){
			acp = MRef<SipHeaderValueAcceptContact*>((SipHeaderValueAcceptContact *)*(headers[i]->getHeaderValue(0)));
			
			if(acp->getFeaturetag()=="+sip.p2t=\"TRUE\"")
				P2T=true;
		}
	}	
}

SipInvite::SipInvite(const string &branch, 
		const string &call_id, 
		const string &tel_no, 
		const string &proxyAddr, 
		int32_t proxyPort, 
		const string &localAddr, 
		int32_t localSipPort, 
		const string &from_tel_no, 
		int32_t seq_no, 
//		int32_t local_media_port, 
//		vector<CODECInterface *> &codecs,
		const string &transport
		):SipMessage(branch, SipInvite::type)
{


	createHeadersAndContent(call_id, tel_no, proxyAddr, proxyPort, localAddr,
			localSipPort, from_tel_no, seq_no, "","","","",transport);
	
	
}


SipInvite::SipInvite(const string &branch, 
		const string &call_id, 
		const string &tel_no, 
		const string &proxyAddr, 
		int32_t proxyPort, 
		const string &localAddr, 
		int32_t localSipPort, 
		const string &from_tel_no, 
		int32_t seq_no, 
		const string &username, 
		const string &nonce, 
		const string &realm, 
		const string &password,
//		int32_t local_media_port, 
//		vector<CODECInterface *> &codecs,
		const string &transport
		):SipMessage(branch, SipInvite::type)
{
	
	createHeadersAndContent(call_id, tel_no, proxyAddr, proxyPort, localAddr, localSipPort, from_tel_no, seq_no, 
			username, nonce, realm, password, transport);

}

SipInvite::~SipInvite(){

}

void SipInvite::createHeadersAndContent(
		const string &call_id,
		const string &tel_no,
		const string &proxyAddr,
		int32_t proxyPort,
		const string &localAddr,
		int32_t localSipPort,
		const string &from_tel_no,
		int32_t seq_no,
		const string &username,
		const string &nonce,
		const string &realm,
		const string &password,
//		int32_t local_media_port,
//		vector<CODECInterface *> &codecs,
		const string &transport
//		const string &key_mgmt
		)
{
	SipURI uri(tel_no,proxyAddr,"phone",proxyPort);
	
	this->username = tel_no;
	this->ip=proxyAddr;
	this->user_type="phone";
	
	//SipHeaderVia *viap = new SipHeaderVia("UDP",localAddr, localSipPort);
	//add_header(viap);
	
	MRef<SipHeader*> fromp = new SipHeader( new SipHeaderValueFrom(from_tel_no, proxyAddr ) );
	addHeader(fromp);

	MRef<SipHeader*> top = new SipHeader( new SipHeaderValueTo(tel_no, proxyAddr) );
	addHeader(top);
	
	MRef<SipHeaderValueCallID*> callidp = new SipHeaderValueCallID() ;
	callidp->setId(call_id);
	addHeader(new SipHeader(*callidp) );
        
	if ( username.length()>0 || nonce.length()>0 || realm.length()>0 ){
		MRef<SipHeader*> authp = new SipHeader( new SipHeaderValueProxyAuthorization("INVITE",tel_no,realm, nonce, uri, username, password,"DIGEST") );
		addHeader(authp);
	}


	MRef<SipHeaderValueCSeq*> seqp = new SipHeaderValueCSeq();
	seqp->setMethod("INVITE");
	seqp->setCSeq(seq_no);
	addHeader(new SipHeader(*seqp));
	
	MRef<SipHeaderValue*> contactp = new SipHeaderValueContact(from_tel_no, localAddr, localSipPort,"phone",transport);
	addHeader(new SipHeader(contactp));
	
	MRef<SipHeaderValueUserAgent*> uap = new SipHeaderValueUserAgent();
	uap->setUserAgent("Minisip");
	addHeader(new SipHeader(*uap));
	
//        MRef<SipHeaderContentType*> contenttypep = new SipHeaderContentType();
        /*contenttypep->setContentType( sdpref->getContentType() ); */
//	addHeader(MRef<SipHeader*>(*contenttypep));

}


string SipInvite::getRemoteTelNo(){
	MRef<SipHeaderValueFrom*> fromp;
	for (int32_t i=0; i< headers.size(); i++)
		if ((headers[i])->getType() == SIP_HEADER_TYPE_FROM){
			fromp = MRef<SipHeaderValueFrom*>((SipHeaderValueFrom *)*(headers[i]->getHeaderValue(0)));
			return fromp->getUri().getUserId();
		}
	merr << "ERROR: Could not find user_id (tel. no.) in SipInvite"<< end;
	return "";
}

string SipInvite::getString(){
	string ret ="";
	if (username.length()>4 && username.substr(0,4)=="sip:")
		ret = "INVITE "+username;
	else
		ret = "INVITE sip:"+username;
	if (username.find("@")==string::npos)
		ret = ret+"@"+ip;

	ret += ";user="+user_type+" SIP/2.0\r\n" + getHeadersAndContent();

	return  ret;
}

/**
 * SipInvite::set_P2T()
 * if the INVITE message is a invite to a P2T Session, this function
 * adds the Accept-Contact header to the packet and sets the 
 * P2T flag equals true.
 *
 * @author Florian Maurer
 */
void SipInvite::set_P2T() {
	this->P2T=true;
	MRef<SipHeaderValueAcceptContact*> acp = new SipHeaderValueAcceptContact("+sip.p2t=\"TRUE\"",true,false);
	addHeader(new SipHeader(*acp) );
}


/**
 * SipInvite::is_P2T()
 * can be used to check if this is a Invite packet to a P2T Session
 * @return returns true if the packet is a invitation to a P2T Session
 * @author Florian Maurer
 */
bool SipInvite::is_P2T() {
	return P2T;
}
