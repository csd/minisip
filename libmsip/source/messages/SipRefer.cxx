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
 * 	SipRefer.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * 	Johan Bilien, jobi@via.ecp.fr
 * Purpose
 * 
*/

#include<config.h>

#include<libmsip/SipRefer.h>
#include<libmsip/SipInvite.h>
#include<assert.h>
#include<libmsip/SipHeaderVia.h>
#include<libmsip/SipHeaderFrom.h>
#include<libmsip/SipHeaderTo.h>
#include<libmsip/SipHeaderCSeq.h>
#include<libmsip/SipHeaderCallID.h>
#include<libmsip/SipHeaderMaxForwards.h>
#include<libmsip/SipHeaderReferTo.h>
#include<libmutil/dbg.h>

const int SipRefer::type=10;

SipRefer::SipRefer(string &resp):SipRequest(SipRefer::type, resp){
}

SipRefer::SipRefer(string branch, MRef<SipInvite*> inv, 
		string to_uri, 
		string from_uri, 
		string proxy,
		string referredUri,
		int cSeqNo
	):SipRequest(branch, type, "REFER")
{
	this->ipaddr=proxy;
	this->referredUri=referredUri;
	username = to_uri;
	setUri(to_uri);

	MRef<SipHeaderValueFrom*> from;
	MRef<SipHeaderValueTo*> to;

	MRef<SipHeader*> mf = new SipHeader( new SipHeaderValueMaxForwards(70));
	addHeader(mf);
	MRef<SipHeader *> header;
	int noHeaders = inv->getNoHeaders();
	for (int32_t i=0; i< noHeaders; i++){
		header = inv->getHeaderNo(i);
		int type = header->getType();
		bool add=false;
		switch (type){
			case SIP_HEADER_TYPE_FROM:
				// ??
//				((SipHeaderValueFrom*)*(header->getHeaderValue(0)))->setParameter("tag", ((SipHeaderValueFrom*)*(header->getHeaderValue(0)))->getParameter("tag"));
//				
				((SipHeaderValueFrom*)*(header->getHeaderValue(0)))->getUri().setUser(from_uri);
				add=true;
				break;
			case SIP_HEADER_TYPE_TO:
//				((SipHeaderValueTo*)*(header->getHeaderValue(0)))->setParameter("tag", ((SipHeaderValueTo*)*(header->getHeaderValue(0)))->getParameter("tag") );
//				((SipHeaderValueTo*)*(header->getHeaderValue(0)))->getUri().setUser(to_uri);
				add=true;
				break;
			case SIP_HEADER_TYPE_CALLID:
				add=true;
				break;
		}
		
		if (add){
			addHeader(header);
		}
	}

	/* Add the CSeq: header */
	MRef<SipHeaderValueCSeq *> cseqVal = new SipHeaderValueCSeq();
	cseqVal->setMethod("REFER");
	cseqVal->setCSeq(cSeqNo);
	header = new SipHeader( *cseqVal );
	addHeader(header);
	
	/* Add the Refer-To: header */
	MRef<SipHeaderValueReferTo *> rtVal = new SipHeaderValueReferTo();
	rtVal->setUri(referredUri);
	header = new SipHeader( *rtVal );
	addHeader(header);
}

string SipRefer::getReferredUri(){
        for (uint32_t i = 0; i< (uint32_t)headers.size(); i++)
                if ((headers[i])->getType() == SIP_HEADER_TYPE_REFERTO){
                        string referredUri = ((SipHeaderValueReferTo *)*(headers[i]->getHeaderValue(0)))->getUri();
                        return referredUri;
                }
        return "";
}
