#include<config.h>

#include<libmsip/SipHeader.h>
#include<libmsip/SipUtils.h>
#include<libmsip/SipHeaderVia.h>
#include<libmsip/SipHeaderFrom.h>
#include<libmsip/SipHeaderTo.h>
#include<libmsip/SipHeaderCallID.h>
#include<libmsip/SipHeaderCSeq.h>
#include<libmsip/SipHeaderContact.h>
#include<libmsip/SipHeaderUserAgent.h>
#include<libmsip/SipHeaderRoute.h>
#include<libmsip/SipHeaderRecordRoute.h>
#include<libmsip/SipHeaderContentLength.h>
#include<libmsip/SipHeaderEvent.h>
#include<libmsip/SipHeaderAccept.h>
#include<libmsip/SipHeaderAcceptContact.h>
#include<libmsip/SipHeaderContentType.h>
#include<libmsip/SipHeaderMaxForwards.h>
#include<libmsip/SipHeaderUnsupported.h>


SipHeader::SipHeader(int type):type(type){

}

SipHeader::~SipHeader(){

}

MRef<SipHeader *> SipHeader::parseHeader(const string &line){

	if (SipUtils::startsWith(line,"Via:")){
		return new SipHeaderVia(line);
	}

	if (SipUtils::startsWith(line,"From:")){
		return new SipHeaderFrom(line);
	}

	if (SipUtils::startsWith(line,"To:")){
		return new SipHeaderTo(line);
	}

	if (SipUtils::startsWith(line,"Call-ID:")){
		return new SipHeaderCallID(line);
	}

	if (SipUtils::startsWith(line,"CSeq:")){
		return new SipHeaderCSeq(line);
	}

	if (SipUtils::startsWith(line,"Contact:")){
		return new SipHeaderContact(line);
	}

	if (SipUtils::startsWith(line,"User-Agent:")){
		return new SipHeaderUserAgent(line);
	}

	if (SipUtils::startsWith(line,"Record-Route:")){
		return new SipHeaderRecordRoute(line);
	}

	if (SipUtils::startsWith(line,"Route:")){
		return new SipHeaderRoute(line);
	}

	if (SipUtils::startsWith(line,"Content-Length:")){
		return new SipHeaderContentLength(line);
	}

	if (SipUtils::startsWith(line, "Event:")){
		return new SipHeaderEvent(line);
	}

	if (SipUtils::startsWith(line, "Accept:")){
		return new SipHeaderAccept(line);
	}

	if (SipUtils::startsWith(line, "Content-Type:")){
		return new SipHeaderContentType(line);
	}

	if (SipUtils::startsWith(line, "Max-Forwards:")){
		return new SipHeaderMaxForwards(line);
	}

	if (SipUtils::startsWith(line,"Accept-Contact:")){
		return new SipHeaderAcceptContact(line);
	}



#ifdef DEBUG_OUTPUT
	mdbg << "INFO: Unsupported header found and added using SipHeaderUnsupported ("<< line <<")"<< end;
#endif
	return new SipHeaderUnsupported(line);
}



