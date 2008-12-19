/*
 Copyright (C) 2004-2006 the Minisip Team
 
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
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */

/* Copyright (C) 2004 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

/* Name
 * 	SdpPacket.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#include<config.h>

#include<libminisip/signaling/sdp/SdpPacket.h>
#include<libmnetutil/NetworkException.h>

#include<libmutil/stringutils.h>

#include<libminisip/signaling/sdp/SdpHeaderV.h>
#include<libminisip/signaling/sdp/SdpHeaderO.h>
#include<libminisip/signaling/sdp/SdpHeaderS.h>
#include<libminisip/signaling/sdp/SdpHeaderC.h>
#include<libminisip/signaling/sdp/SdpHeaderT.h>
#include<libminisip/signaling/sdp/SdpHeaderM.h>
#include<libminisip/signaling/sdp/SdpHeaderA.h>
#include<libminisip/signaling/sdp/SdpHeaderI.h>

#include<iostream>
#include<sys/types.h>

using namespace std;

MRef<SipMessageContent*> sdpSipMessageContentFactory(const string &buf, const string &){
	return new SdpPacket(buf);
}


SdpPacket::SdpPacket(){
}

SdpPacket::SdpPacket(string build_from) /*: MObject("SdpPacket")*/{
	//content = initstr;
	MRef<SdpHeaderM *> lastM=NULL;
	MRef<SdpHeaderA *> attr=NULL;
	vector<string> lines = splitLines(build_from); 
	for (uint32_t i=0; i< lines.size(); i++){
		switch (lines[i][0]){
			case 'v':
				addHeader(MRef<SdpHeader*>(new SdpHeaderV(lines[i])));
				break;
			case 'o':
				addHeader(MRef<SdpHeader*>(new SdpHeaderO(lines[i])));
				break;
			case 's':
				addHeader(MRef<SdpHeader*>(new SdpHeaderS(lines[i])));
				break;
			case 'i':{
				MRef<SdpHeaderI*> in = new SdpHeaderI(lines[i]);
				if( lastM )
					lastM->setInformation(in);
				else
					addHeader(*in);
				break;
				}
			case 'c':{
				MRef<SdpHeaderC*> c = new SdpHeaderC(lines[i]);
				if( lastM )
					lastM->setConnection(c);
				else
					addHeader(*c);
				break;
				}
			case 't':
				addHeader(MRef<SdpHeader*>(new SdpHeaderT(lines[i])));
				break;
			case 'm':
				lastM=new SdpHeaderM(lines[i]);
				addHeader(*lastM);
				break;
			case 'a':
				attr=new SdpHeaderA(lines[i]);
				if(lastM){
                                        // media level attribute
					lastM->addAttribute(attr);
				}
                                else{
                                        // Session level attribute
                                        addHeader(*attr);
                                }
                                
				break;
			default:
				cerr << "ERROR: unknown SDP header: "<< lines[i]<< endl;
				break;
		}
	}
}

#if 0

SdpPacket::SdpPacket(/*IPAddress &ipaddr*/ string ipaddr, int32_t local_media_port, vector<CODECInterface *> &codecs)/*:MObject("SdpPacket")*/{
	string addrtype;
//	if (dynamic_cast<IP4Address *>(&ipaddr)){
		addrtype="IP4";
//	}else{
//		addrtype="IP6";
//	}
	MRef<SdpHeader*> v = new SdpHeaderV(0);
	addHeader(v);

	MRef<SdpHeader*> o = new SdpHeaderO("","3344","3344","IN", addrtype, ipaddr/*.get_string()*/ );
	addHeader(o);

	MRef<SdpHeader*> s = new SdpHeaderS("s=Minisip Session");
	addHeader(s);

	MRef<SdpHeader*> c = new SdpHeaderC("IN", addrtype,ipaddr/*.get_string()*/ );
	addHeader(c);

	MRef<SdpHeader*> t = new SdpHeaderT(0,0);
	addHeader(t);

	MRef<SdpHeaderM*> m = new SdpHeaderM("audio", local_media_port, 1, "RTP/AVP");
	for (unsigned i=0; i<codecs.size(); i++){
		m->addFormat(codecs[i]->getSdpMediaType());
	}
	addHeader(MRef<SdpHeader*>(*m));

	for (unsigned i=0; i<codecs.size(); i++){
		MRef<SdpHeaderA*> a = new SdpHeaderA("a=X");
		a->setAttributes("rtpmap:"+itoa(codecs[i]->getSdpMediaType())+" "+codecs[i]->getSdpMediaAttributes());
		addHeader(MRef<SdpHeader*>(*a));
	}
}

SdpPacket::SdpPacket(/*IPAddress &ipaddr*/ string ipAddr, int32_t local_media_port, vector<CODECInterface *> &codecs, string key_mgmt)/*:MObject("SdpPacket")*/{
	string addrtype;
//	if (dynamic_cast<IP4Address *>(&ipaddr)){
		addrtype="IP4";
//	}else{
//		addrtype="IP6";
//	}
	MRef<SdpHeader*> v = new SdpHeaderV(0);
	addHeader(v);

	MRef<SdpHeader*> o = new SdpHeaderO("","3344","3344","IN", addrtype, ipAddr );
	addHeader(o);

	MRef<SdpHeader*> s = new SdpHeaderS("s=Minisip Session");
	addHeader(s);

	MRef<SdpHeader*> c = new SdpHeaderC("IN", addrtype, ipAddr);
	addHeader(c);

	MRef<SdpHeader*> t = new SdpHeaderT(0,0);
	addHeader(t);

	MRef<SdpHeaderA*> ak = new SdpHeaderA("a=X");
	ak->setAttributes("key-mgmt:"+key_mgmt);
	addHeader(MRef<SdpHeader*>(*ak));

	MRef<SdpHeaderM*> m = new SdpHeaderM("audio", local_media_port, 1, "RTP/AVP");
	for (unsigned i=0; i<codecs.size(); i++){
		m->addFormat(codecs[i]->getSdpMediaType());
	}
	addHeader(MRef<SdpHeader*>(*m));

	for (unsigned i=0; i<codecs.size(); i++){
		MRef<SdpHeaderA*> a = new SdpHeaderA("a=X");
		a->setAttributes("rtpmap:"+itoa(codecs[i]->getSdpMediaType())+" "+codecs[i]->getSdpMediaAttributes());
		addHeader(MRef<SdpHeader*>(*a));
	}
}
#endif

void SdpPacket::addHeader(MRef<SdpHeader*> h){
	headers.push_back(h);
}

string SdpPacket::getString() const{
	string ret = "";
	list<MRef<SdpHeaderA*> >::iterator  iAttr;
	list<MRef<SdpHeaderA*> > attributes;
	for (int32_t prio=0; prio<15; prio++)
		for (unsigned i=0; i< headers.size(); i++)
			if (headers[i]->getPriority()==prio){
				ret+="\r\n"+headers[i]->getString();
				if (headers[i]->getType() == SDP_HEADER_TYPE_M){
					attributes = ((SdpHeaderM *)*(headers[i]))->getAttributes();
					for(iAttr = attributes.begin(); iAttr!=attributes.end(); iAttr++){
						ret+="\r\n"+(*iAttr)->getString();
					}
				}
			}
	
					
	return trim(ret)+"\r\n";
}



MRef<SdpHeaderC*> SdpPacket::getSessionLevelConnection(){
//	cerr << "starting parse of remote addr in SDP packet"<< endl;
	for (unsigned i=0; i<headers.size(); i++){
		if ( headers[i].isNull() )
			cerr <<"WARNING: sdp header is null"<< endl;
//		cerr << "testing header with prio "<< headers[i]->get_priority()<< endl;
		if ((headers[i])->getType() == SDP_HEADER_TYPE_C){
			MRef<SdpHeaderC*> cptr;
			cptr = MRef<SdpHeaderC*>((SdpHeaderC *)*(headers[i]));
			return cptr;
		}
	}

	return NULL;
}

string SdpPacket::getKeyMgmt(){
	string message;
	MRef<SdpHeaderA*> aptr;

	for( unsigned i=0; i<headers.size();i++ ){
		if( headers[i].isNull() )
			cerr << "WARNING: sdp header is null" << endl;
		if((headers[i])->getType() == SDP_HEADER_TYPE_A ){
			aptr = MRef<SdpHeaderA*>((SdpHeaderA *)*(headers[i]));
			if( aptr->getAttributeType() == "key-mgmt" )
				return aptr->getAttributeValue();
		}
	}

	return "";
}
	

string SdpPacket::getFormatMatch(SdpPacket &pack){
	MRef<SdpHeaderM*> mym;
	MRef<SdpHeaderM*> otherm;

	unsigned int i;
	
	for (i = 0 ; i< headers.size(); i++)
		if ((headers[i])->getType() == SDP_HEADER_TYPE_M)
			mym = MRef<SdpHeaderM*>((SdpHeaderM*)(*headers[i]));

	for (i=0; i<pack.headers.size(); i++)
		if ((headers[i])->getType() == SDP_HEADER_TYPE_M)
			otherm = MRef<SdpHeaderM*>((SdpHeaderM*)(*pack.headers[i]));
		
	if (mym.isNull() || otherm.isNull() ){
		cerr << "ERROR: BUG: SDP packet did not contain <m> header. Defaulting to PCMu"<< endl;
		return 0;
	}


	for (i=0; (int)i< mym->getNrFormats(); i++)
		for (int32_t j=0; j< otherm->getNrFormats(); j++)
			if (mym->getFormat(i)==otherm->getFormat(j))
				return mym->getFormat(i);
	cout << "ERROR: could not match any codec format - trying anyway with PCMu"<< endl;
	return 0;
	
}
#if 0
int32_t SdpPacket::getCodecMatch(vector<CODECInterface *>codecs){
	MRef<SdpHeaderM*> mym;

//	cerr << "_______________Starting CODEC match___________________"<< endl;
	for (unsigned i = 0 ; i< headers.size(); i++)
		if ((headers[i])->getType() == SDP_HEADER_TYPE_M)
			mym = MRef<SdpHeaderM*>((SdpHeaderM*)(*headers[i]));

	if (mym.isNull()){
		cerr << "ERROR: BUG: SDP packet did not contain <m> header. Defaulting to PCMu"<< endl;
		return 0;
	}
//	cerr << "Number of formats in mym "<< mym->get_n_formats()<< endl;

	for (int32_t i=0; i< mym->getNrFormats(); i++){
//		cerr << "Trying to find format "<< mym->get_format(i)<< endl;
		for (unsigned j=0; j< codecs.size(); j++){
//			cerr << "matching "<< mym->get_format(i)<<" to "<<codecs[j]->get_sdp_media_type()<< endl;;
			if (mym->getFormat(i)==codecs[j]->getSdpMediaType() )
				return mym->getFormat(i);
		}
	}
	cout << "ERROR: could not match any codec format - trying anyway with PCMu"<< endl;
	return 0;
	
}
#endif

string SdpPacket::getFirstMediaFormat(){
	MRef<SdpHeaderM*> mym;

	for (unsigned i = 0 ; i< headers.size(); i++)
		if ((headers[i])->getType() == SDP_HEADER_TYPE_M)
			mym = MRef<SdpHeaderM*>((SdpHeaderM*)(*headers[i]));

	if (mym.isNull() || mym->getNrFormats()<1){
		cerr << "ERROR: BUG: SDP packet did not contain <m> header. Defaulting to PCMu"<< endl;
		return 0;
	}

	return mym->getFormat(0);

}

bool SdpPacket::mediaFormatAvailable(string f){
	MRef<SdpHeaderM*> mym;

	unsigned int i;
	for (i = 0 ; i< headers.size(); i++)
		if ((headers[i])->getType() == SDP_HEADER_TYPE_M)
			mym = MRef<SdpHeaderM*>((SdpHeaderM*)(*headers[i]));

	if (mym.isNull() || mym->getNrFormats()<1){
		cerr << "ERROR: BUG: SDP packet did not contain <m> header. Defaulting to PCMu"<< endl;
		return 0;
	}

	for (i=0;  (int)i<mym->getNrFormats(); i++)
		if (mym->getFormat(i)==f)
			return true;
	return false;
}


void SdpPacket::setSessionLevelAttribute(string type, string value) {
		MRef<SdpHeaderA*> a = new SdpHeaderA("a=X");
		a->set_priority(6);
		a->setAttributes(type+":"+value);
		addHeader(MRef<SdpHeader*>(*a));		
}


string SdpPacket::getSessionLevelAttribute(string type) {
	MRef<SdpHeaderA*> a;
	string ret="";
	
	for (unsigned i = 0 ; i< headers.size(); i++){
		//search all headers for type A
		if ((headers[i])->getType() == SDP_HEADER_TYPE_A){
			a = MRef<SdpHeaderA*>((SdpHeaderA*)(*headers[i]));
			
			if(a->getAttributeType()==type){
				ret=a->getAttributeValue();
				break;
			}
		}
	}
	
	return ret;
}


std::vector<MRef<SdpHeader*> > SdpPacket::getHeaders(){
	return headers;
}
