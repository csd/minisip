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
 * 	SdpHeaderM.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#include<config.h>

#include"SdpHeaderM.h"
#include<libmutil/itoa.h>

#ifdef DEBUG_OUTPUT
#include<iostream>
#endif

using namespace std;

#include"SdpHeaderA.h"

SdpHeaderM::SdpHeaderM(string buildFrom) : SdpHeader(SDP_HEADER_TYPE_M, 8){

	if (buildFrom.substr(0,2)!="m="){
#ifdef DEBUG_OUTPUT
		cerr << "ERROR: Origin sdp header is not starting with <m=>"<< endl;
#endif
	}
	unsigned i=2;
	while (buildFrom[i]==' ')
		i++;
	
	media="";
	while (buildFrom[i]!=' ')
		media+=buildFrom[i++];

	while (buildFrom[i]==' ')
		i++;

	string portstr="";
	while (buildFrom[i]!=' ')
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
	
	while (buildFrom[i]==' ')
		i++;
	
	transport="";
	while (buildFrom[i]!=' ')
		transport+=buildFrom[i++];

	bool done=false;
	while (!done){
		while (buildFrom[i]==' '  && !(i>=buildFrom.length())){
			i++;
		}

		string f="";
		while (buildFrom[i]!=' ' && !(i>=buildFrom.length()))
			f+=buildFrom[i++];
		if (f.length()>0){
			formats.push_back(atoi(f.c_str()));
		}
		
		if (i>=buildFrom.length())
			done=true;
	}
}

SdpHeaderM::SdpHeaderM(string media, 
			int32_t port, 
			int32_t n_ports, 
			string transport) 
				: SdpHeader(SDP_HEADER_TYPE_M, 8)
{
	this->media=media;
	this->port=port;
	this->nPorts=n_ports;
	this->transport=transport;
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

void SdpHeaderM::setPort(int32_t port){
	this->port=port;
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

void SdpHeaderM::addFormat(int32_t f){
	formats.push_back(f);;
}

int32_t SdpHeaderM::getNrFormats(){
	return formats.size();
}

int32_t SdpHeaderM::getFormat(int32_t i){
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
		ret+=" "+itoa(formats[i]);

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

string SdpHeaderM::getRtpMap(uint32_t format){
	int i=0;
	string rtpmap;

	while((rtpmap = getAttribute("rtpmap",i)) != ""){
		size_t firstSpace = rtpmap.find(" ");
		if( rtpmap.substr(0, firstSpace) == itoa(format) ){
			return rtpmap.substr(firstSpace+1, rtpmap.size());
		}
		i++;
	}
	return "";
}

list<MRef<SdpHeaderA *> > SdpHeaderM::getAttributes(){
	return attributes;
}
