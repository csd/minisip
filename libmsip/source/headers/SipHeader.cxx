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
#include<libmsip/SipHeaderWarning.h>

#include<libmutil/split_in_lines.h>

#include<vector>


SipHeader::SipHeader(MRef<SipHeaderValue*> val): headerName(val->headerName){
//	cerr << "Header name is "<< flush << headerName<< endl;
	type = val->getType();
	headerValues.push_back(val);
}

SipHeader::~SipHeader(){

}

string SipHeader::getString(){
	string ret = headerName +": ";
	bool first=true;
	for (int i=0; i< headerValues.size(); i++){
		if (!first){
			ret += ",";
		}else{
			first=false;
		}
		ret+=headerValues[i]->getString();
	}
	return ret;
}

void SipHeader::addHeaderValue(MRef<SipHeaderValue*> v){
	headerValues.push_back(v);
}

static string getHeader(const string &line,int &endi){
	string ret;
	int i;
	for (i=0;   i<line.size() && line[i]!=' ' && line[i]!='\t' && line[i]!=':'   ;  i++){
		ret+=line[i];
	}
	while (line[i]==' ' || line[i]=='\t' || line[i]==':'){
		i++;
	}
	endi=i;
	return ret;
}

MRef<SipHeader *> SipHeader::parseHeader(const string &line){
	int hdrstart=0;
	string hdr = getHeader(line,hdrstart);
	string valueline = line.substr(hdrstart);
	
//	cerr << "hdr parsed to <"<< hdr << ">"<< endl;
//	cerr << "valueline parsed to <"<< valueline<<">"<< endl;
	
	vector<string> values = split(valueline,true, ',');
	MRef<SipHeader*> h;
	if (SipUtils::startsWith(hdr,"Via")){
		 h= new SipHeader(new SipHeaderValueVia(values[0]));
		 for (int i=1;i<values.size();i++){
		 	h->addHeaderValue(new SipHeaderValueVia(values[i]));
		 }
		 return h;
	}

	if (SipUtils::startsWith(hdr,"From")){
		 h= new SipHeader(new SipHeaderValueFrom(values[0]));
		 for (int i=1;i<values.size();i++){
		 	h->addHeaderValue(new SipHeaderValueFrom(values[i]));
		 }
		 return h;

//		return new SipHeader(new SipHeaderValueFrom(line));
	}

	if (SipUtils::startsWith(hdr,"To")){
		 h= new SipHeader(new SipHeaderValueTo(values[0]));
		 for (int i=1;i<values.size();i++){
		 	h->addHeaderValue(new SipHeaderValueTo(values[i]));
		 }
		 return h;

//		return new SipHeader(new SipHeaderValueTo(line));
	}

	if (SipUtils::startsWith(hdr,"Call-ID")){
		 h= new SipHeader(new SipHeaderValueCallID(values[0]));
		 for (int i=1;i<values.size();i++){
		 	h->addHeaderValue(new SipHeaderValueCallID(values[i]));
		 }
		 return h;

//		return new SipHeader(new SipHeaderValueCallID(line));
	}

	if (SipUtils::startsWith(hdr,"CSeq")){
		 h= new SipHeader(new SipHeaderValueCSeq(values[0]));
		 for (int i=1;i<values.size();i++){
		 	h->addHeaderValue(new SipHeaderValueCSeq(values[i]));
		 }
		 return h;

//		return new SipHeader(new SipHeaderValueCSeq(line));
	}

	if (SipUtils::startsWith(hdr,"Contact")){
		 h= new SipHeader(new SipHeaderValueContact(values[0]));
		 for (int i=1;i<values.size();i++){
		 	h->addHeaderValue(new SipHeaderValueContact(values[i]));
		 }
		 return h;

//		return new SipHeader(new SipHeaderValueContact(line) );
	}

	if (SipUtils::startsWith(hdr,"User-Agent")){
		 h= new SipHeader(new SipHeaderValueUserAgent(values[0]));
		 for (int i=1;i<values.size();i++){
		 	h->addHeaderValue(new SipHeaderValueUserAgent(values[i]));
		 }
		 return h;

//		return new SipHeader(new SipHeaderValueUserAgent(line));
	}

	if (SipUtils::startsWith(hdr,"Record-Route")){
		 h= new SipHeader(new SipHeaderValueRecordRoute(values[0]));
		 for (int i=1;i<values.size();i++){
		 	h->addHeaderValue(new SipHeaderValueRecordRoute(values[i]));
		 }
		 return h;

//		return new SipHeader(new SipHeaderValueRecordRoute(line));
	}

	if (SipUtils::startsWith(hdr,"Route")){
		 h= new SipHeader(new SipHeaderValueRoute(values[0]));
		 for (int i=1;i<values.size();i++){
		 	h->addHeaderValue(new SipHeaderValueRoute(values[i]));
		 }
		 return h;

//		return new SipHeader(new SipHeaderValueRoute(line));
	}

	if (SipUtils::startsWith(hdr,"Content-Length")){
		 h= new SipHeader(new SipHeaderValueContentLength(values[0]));
		 for (int i=1;i<values.size();i++){
		 	h->addHeaderValue(new SipHeaderValueContentLength(values[i]));
		 }
		 return h;

//		return new SipHeader(new SipHeaderValueContentLength(line));
	}

	if (SipUtils::startsWith(hdr, "Event")){
		 h= new SipHeader(new SipHeaderValueEvent(values[0]));
		 for (int i=1;i<values.size();i++){
		 	h->addHeaderValue(new SipHeaderValueEvent(values[i]));
		 }
		 return h;

//		return new SipHeader(new SipHeaderValueEvent(line));
	}

	if (SipUtils::startsWith(hdr, "Accept")){
		 h= new SipHeader(new SipHeaderValueAccept(values[0]));
		 for (int i=1;i<values.size();i++){
		 	h->addHeaderValue(new SipHeaderValueAccept(values[i]));
		 }
		 return h;

//		return new SipHeader(new SipHeaderValueAccept(line));
	}

	if (SipUtils::startsWith(hdr, "Content-Type")){
		 h= new SipHeader(new SipHeaderValueContentType(values[0]));
		 for (int i=1;i<values.size();i++){
		 	h->addHeaderValue(new SipHeaderValueContentType(values[i]));
		 }
		 return h;

//		return new SipHeader(new SipHeaderValueContentType(line));
	}

	if (SipUtils::startsWith(hdr, "Max-Forwards")){
		 h= new SipHeader(new SipHeaderValueMaxForwards(values[0]));
		 for (int i=1;i<values.size();i++){
		 	h->addHeaderValue(new SipHeaderValueMaxForwards(values[i]));
		 }
		 return h;

//		return new SipHeader(new SipHeaderValueMaxForwards(line));
	}

	if (SipUtils::startsWith(hdr,"Accept-Contact")){
		 h= new SipHeader(new SipHeaderValueAcceptContact(values[0]));
		 for (int i=1;i<values.size();i++){
		 	h->addHeaderValue(new SipHeaderValueAcceptContact(values[i]));
		 }
		 return h;

//		return new SipHeader(new SipHeaderValueAcceptContact(line));
	}

	if (SipUtils::startsWith(hdr,"Warning")){
		 h= new SipHeader(new SipHeaderValueWarning(values[0]));
		 for (int i=1;i<values.size();i++){
		 	h->addHeaderValue(new SipHeaderValueWarning(values[i]));
		 }
		 return h;

//		return new SipHeader(new SipHeaderValueWarning(line));
	}


#ifdef DEBUG_OUTPUT
	mdbg << "INFO: Unsupported header found and added using SipHeaderUnsupported ("<< line <<")"<< end;
#endif
	h= new SipHeader(new SipHeaderValueUnsupported(values[0]));
	for (int i=1;i<values.size();i++){
		h->addHeaderValue(new SipHeaderValueUnsupported(values[i]));
	}
	return h;
}



SipHeaderValue::SipHeaderValue(int t, const string &hname):headerName(hname),type(t){

}

