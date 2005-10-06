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
#include<libmsip/SipHeaderExpires.h>
#include<libmsip/SipHeaderAccept.h>

const int SipIMMessage::type=9;

SipIMMessage::SipIMMessage(string &build_from): SipMessage(SipIMMessage::type, build_from){
	
}

SipIMMessage::SipIMMessage(string branch, 
		    string call_id, 
		    MRef<SipIdentity*> toIdentity,
		    MRef<SipIdentity*> fromIdentity,
                    int32_t seq_no, string msg): 
                            SipMessage(branch, SipIMMessage::type),
			    fromIdentity(fromIdentity)
{
	
	toUser = toIdentity->sipUsername;
	toDomain = toIdentity->sipDomain;
	
	MRef<SipHeaderValue*> fromp = new SipHeaderValueFrom( fromIdentity->sipUsername, fromIdentity->sipDomain );
	addHeader(new SipHeader(*fromp));

	MRef<SipHeaderValue*> top = new SipHeaderValueTo(toUser, toDomain);
	addHeader(new SipHeader(*top));
	
	MRef<SipHeaderValue*> mf = new SipHeaderValueMaxForwards(70);
	addHeader(new SipHeader(*mf));

	MRef<SipHeaderValueCallID*> callidp = new SipHeaderValueCallID();
	callidp->setId(call_id);
	addHeader(new SipHeader(*callidp));
	
	MRef<SipHeaderValueCSeq*> seqp = new SipHeaderValueCSeq();
	seqp->setMethod("MESSAGE");
	seqp->setCSeq(seq_no);
	addHeader(new SipHeader(*seqp));
	
	
	MRef<SipHeaderValueUserAgent*> uap = new SipHeaderValueUserAgent();
	uap->setUserAgent(HEADER_USER_AGENT_DEFAULT);
	addHeader(new SipHeader(*uap));

	setContent(new SipMessageContentIM(msg));
}

SipIMMessage::~SipIMMessage(){

}

string SipIMMessage::getString(){
	string ret ="";
	
	//FIXME sanitize the request uri ... if we used a SipURI object, this would not be needed
	string username; //hide the class::username ... carefull
	size_t pos;
	username = this->toUser;
	
	pos = username.find('<');
	if( pos != string::npos ) {
		username.erase( 0, pos + 1 ); //erase the part in front of the '<'
		pos = username.find('>');
		username.erase( pos );
	}

	if (username.length()>4 && username.substr(0,4)=="sip:")
		ret = "MESSAGE "+username;
	else
		ret = "MESSAGE sip:"+username;
	if (username.find("@")==string::npos)
		ret = ret+"@"+toDomain;

	ret += " SIP/2.0\r\n" + getHeadersAndContent();

	return  ret;
}

int SipIMMessage::getExpiresTimeout(){
	assert(dynamic_cast<SipHeaderValueExpires*>( *getHeaderValueNo( SIP_HEADER_TYPE_EXPIRES, 0 ) ));
	
	MRef<SipHeaderValueExpires*> exp = (SipHeaderValueExpires*)*getHeaderValueNo( SIP_HEADER_TYPE_EXPIRES, 0 );
	
	if (exp){
		return exp->getTimeout();
	}else{
		return -1;
	}
}

