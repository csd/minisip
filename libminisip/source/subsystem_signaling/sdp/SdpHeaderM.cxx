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
 * 	SdpHeaderM.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#include<config.h>

#include<libminisip/signaling/sdp/SdpHeaderM.h>
#include<libmutil/stringutils.h>

#ifdef DEBUG_OUTPUT
#include<iostream>
#endif

using namespace std;

#include<libminisip/signaling/sdp/SdpHeaderA.h>
#include<stdlib.h>

SdpHeaderM::SdpHeaderM(string buildFrom) : SdpHeader(SDP_HEADER_TYPE_M, 8){

	size_t len = buildFrom.length();
	if (buildFrom.substr(0,2)!="m="){
#ifdef DEBUG_OUTPUT
		cerr << "ERROR: Origin sdp header is not starting with <m=>"<< endl;
#endif
	}
	size_t i=2;
	while (buildFrom[i]==' ' && i<len)
		i++;
	
	media="";
	while (buildFrom[i]!=' ' && i<len)
		media+=buildFrom[i++];

	while (buildFrom[i]==' ' && i<len)
		i++;

	string portstr="";
	while (buildFrom[i]!=' ' && i<len)
		portstr+=buildFrom[i++];
	
	int32_t np=0;
	for (unsigned j=0; j<portstr.length(); j++)
		if (portstr[j]=='/')
			np=j;
	if (np>0){
		port = atoi(portstr.substr(0,np).c_str());
		nPorts= atoi(portstr.substr(np+1,portstr.length()-(np+1)-1).c_str());
	}else{
		port = atoi(portstr.c_str());
		nPorts=1;
	}
	
	while (buildFrom[i]==' ' && i<len)
		i++;
	
	transport="";
	while (buildFrom[i]!=' ' && i<len)
		transport+=buildFrom[i++];

	bool done=false;
	while (!done){
		while (buildFrom[i]==' '  && i<len ){
			i++;
		}

		string f="";
		while (buildFrom[i]!=' ' && i<len )
			f+=buildFrom[i++];
		if (f.length()>0){
			formats.push_back( f );
		}
		
		if ( i>=len )
			done=true;
	}
}

SdpHeaderM::SdpHeaderM(string media_, 
			int32_t port_, 
			int32_t n_ports, 
			string transp) 
				: SdpHeader(SDP_HEADER_TYPE_M, 8)
{
	this->media=media_;
	this->port=port_;
	this->nPorts=n_ports;
	this->transport=transp;
}

SdpHeaderM::SdpHeaderM(const SdpHeaderM &src): SdpHeader( SDP_HEADER_TYPE_M, 8 ){
	*this = src;
}

SdpHeaderM &SdpHeaderM::operator=(const SdpHeaderM &src){
	set_priority( src.getPriority() );
	media = src.media;
	port = src.port;
	nPorts = src.nPorts;
	transport = src.transport;
	formats = src.formats;
	attributes.clear();

	list<MRef<SdpHeaderA *> >::const_iterator i;

	for(i=src.attributes.begin(); i!=src.attributes.end(); i++){
		addAttribute( new SdpHeaderA( ***i ) );
	}

	return *this;
}

SdpHeaderM::~SdpHeaderM(){

}

string SdpHeaderM::getMedia(){
	return media;
}

void SdpHeaderM::setMedia(string m){
	this->media=m;
}

int32_t SdpHeaderM::getPort(){
//	cerr << "Returning port"<< port << endl;
	return port;
}

void SdpHeaderM::setPort(int32_t p){
	this->port=p;
}

int32_t SdpHeaderM::getNrPorts(){
	return nPorts;
}

void SdpHeaderM::setNrPorts(int32_t n){
	this->nPorts=n;
}

string SdpHeaderM::getTransport(){
	return transport;
}

void SdpHeaderM::setTransport(string t){
	this->transport=t;
}

void SdpHeaderM::addFormat(string f){
	formats.push_back(f);;
}

int32_t SdpHeaderM::getNrFormats(){
	return (int32_t)formats.size();
}

string SdpHeaderM::getFormat(int32_t i){
	return formats[i];
}

string SdpHeaderM::getString(){
	string ret="m="+media+" ";
	
	if (nPorts>1)
		ret+=port+"/"+itoa(nPorts);
	else
		ret+=itoa(port);
	
	ret+=" "+transport;

	for (unsigned i=0; i< formats.size(); i++)
		ret+=" "+formats[i];

	if( connection )
		ret += "\r\n" + connection->getString();
		
	if( information )
		ret += "\r\n" + information->getString();		

	return ret;
}

void SdpHeaderM::addAttribute(MRef<SdpHeaderA*> a){
	attributes.push_back(a);
}

string SdpHeaderM::getAttribute(string key, uint32_t n){
	list<MRef<SdpHeaderA *> >::iterator i;
	uint32_t nAttr = 0;

	for(i=attributes.begin(); i!=attributes.end(); i++){
		if((*i)->getAttributeType() == key){
			if(nAttr == n){
				return (*i)->getAttributeValue();
			}
			nAttr++;
		}
	}
	return "";
}

string SdpHeaderM::getRtpMap(std::string format){
	int i=0;
	string attrib;
	string value = "rtpmap";

	while((attrib = getAttribute(value, i)) != ""){
		size_t firstSpace = attrib.find(" ");
// 		cerr << "SdpHeaderM::getRtpMap - value retrieved = " << attrib << "; substr = " << attrib.substr(0, firstSpace) << endl;
		if( attrib.substr(0, firstSpace) == format ){
			return attrib.substr(firstSpace+1, attrib.size());
		}
		i++;
	}
	return "";
}

string SdpHeaderM::getFmtpParam(string format){
	int i=0;
	string attrib;
	string value = "fmtp";

	while((attrib = getAttribute(value, i)) != ""){
		size_t firstSpace = attrib.find(" ");
// 		cerr << "SdpHeaderM::getFmtpParam - value retrieved = " << attrib << "; substr = " << attrib.substr(0, firstSpace) << endl;
		if( attrib.substr(0, firstSpace) == format ){
			return attrib.substr(firstSpace+1, attrib.size());
		}
		i++;
	}
	return "";
}

list<MRef<SdpHeaderA *> > SdpHeaderM::getAttributes(){
	return attributes;
}


void SdpHeaderM::setConnection( MRef<SdpHeaderC*> c ){
	connection = c;
}

MRef<SdpHeaderC*> SdpHeaderM::getConnection(){
	return connection;
}
void SdpHeaderM::setInformation(MRef<SdpHeaderI*> i) {
	information = i;
}

MRef<SdpHeaderI*> SdpHeaderM::getInformation() {
	return information;
}
