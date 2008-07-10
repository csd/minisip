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

#include<config.h>

#include<assert.h>
#include<iostream>
#include<libmstun/STUNAttributes.h>
#include<stdlib.h>
#include<string.h>

#ifdef WIN32
#include<winsock2.h>
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include<netinet/in.h>
#include<netdb.h>
#endif

using namespace std;

const int STUNAttribute::MAPPED_ADDRESS=0x0001;
const int STUNAttribute::RESPONSE_ADDRESS=0x0002;
const int STUNAttribute::CHANGE_REQUEST=0x0003;
const int STUNAttribute::SOURCE_ADDRESS=0x0004;
const int STUNAttribute::CHANGED_ADDRESS=0x0005;
const int STUNAttribute::USERNAME=0x0006;
const int STUNAttribute::PASSWORD=0x0007;
const int STUNAttribute::MESSAGE_INTEGRITY=0x0008;
const int STUNAttribute::ERROR_CODE=0x0009;
const int STUNAttribute::UNKNOWN_ATTRIBUTES=0x000a;
const int STUNAttribute::REFLECTED_FROM=0x000b;


#define CHANGE_IP_MASK 0x04
#define CHANGE_PORT_MASK 0x02

										
STUNAttribute::STUNAttribute(int t): type(t){
	
}

STUNAttribute::~STUNAttribute(){
	assert(1==0);
}

int STUNAttribute::getType(){
	return type;
}

int STUNAttribute::getMessageDataTLV(unsigned char *buf){
	uint16_t *ptr= (uint16_t *)buf;
	ptr[0]=htons(type);
	ptr[1]=htons(getValueLength());
	getValue(&buf[4]);
	return 2+2+getValueLength();
}

STUNAttribute *STUNAttribute::parseAttribute(unsigned char *data, int /*maxLength*/, int &retParsedLength){//TODO: use maxLength
  
  uint16_t *shortptr = (uint16_t *)data;
  int type   = ntohs(shortptr[0]);
  int length = ntohs(shortptr[1]);
  STUNAttribute *ret=NULL;
  switch (type){
  case STUNAttribute::MAPPED_ADDRESS:
    ret = new STUNAttributeMappedAddress(length,&data[4]);
    break;
  case STUNAttribute::SOURCE_ADDRESS:
    ret = new STUNAttributeSourceAddress(length, &data[4]);
    break;
  case STUNAttribute::RESPONSE_ADDRESS:
    ret = new STUNAttributeResponseAddress(length,&data[4]);
    break;
  case STUNAttribute::CHANGED_ADDRESS:
    ret = new STUNAttributeChangedAddress(length,&data[4]);
    break;
  case STUNAttribute::CHANGE_REQUEST:
    ret = new STUNAttributeChangeRequest(&data[4], length);
    break;
  case STUNAttribute::USERNAME:
    ret = new STUNAttributeUsername(length,&data[4]);
    break;
  case STUNAttribute::PASSWORD:
    ret = new STUNAttributePassword(length,&data[4]);
    break;
  case STUNAttribute::ERROR_CODE:
    ret = new STUNAttributeErrorCode(length,&data[4]);
    break;
  default:
    cerr << "UNKNOWN ATTRIBUTE: "<< type << endl;
    
    if (type < 0x7fff) { // attribute is mandatory to understand
      assert(1==0 /*UNKNOWN ATTRIBUTE*/);
    } else {             // attribute is not mandatory to understand, ignore
      retParsedLength = 2+2+length; // TODO: check that there is length to parse 
      return NULL;
    }
    break;
  }
  assert(ret!=NULL);
  retParsedLength = 2+2+ret->getValueLength();
  //  cerr << retParsedLength<<endl;
  return ret;
}

////////////////////////////////////
//

STUNAttributeAddress::STUNAttributeAddress(int type, uint16_t port, char *addr)
		: STUNAttribute(type)
{
	assert(addr!=NULL);
	
	struct hostent *hp= gethostbyname(addr);
	
	if (!hp){
		cerr << "Could not resolve host "<< addr << endl;
		//TODO: throw exception
		exit(1);
	}
	this->address = ntohl( * ((uint32_t*)hp->h_addr_list[0]) );
	
	this->port = port;
	
}

STUNAttributeAddress::STUNAttributeAddress(
	int type,
	unsigned char* data, 
	int length)
		: STUNAttribute(type)
{
	assert(length==8); //FIXME: throw exception	
	family = data[1];
	uint16_t *shortPtr = (uint16_t*)data;
	port = ntohs(shortPtr[1]);
	uint32_t *ptr = (uint32_t*)data;
	address = ntohl(ptr[1]);
}

int STUNAttributeAddress::getValueLength(){
	return 8;
}

int STUNAttributeAddress::getValue(unsigned char* buf){
	buf[0]=0;
	buf[1]=family;
	uint16_t *shortPtr = (uint16_t*)buf;
	shortPtr[1] = htons(port);
	uint32_t *ptr = (uint32_t*)buf;
	ptr[1]=htonl(address);
	return getValueLength();
}

uint32_t STUNAttributeAddress::getBinaryIP(){
	return address;
}

uint16_t STUNAttributeAddress::getPort(){
	return port;
}

std::string STUNAttributeAddress::getDesc(){
	//char tmp[200];
	uint32_t nip = htonl(address);
	//inet_ntop(AF_INET,&nip,tmp,200);
	std::string ip = std::string("") + itoa((nip >> 24)&0xFF)+"." + itoa((nip >>16)&0xFF)+"." + itoa((nip>>8)&0xFF)+"." + itoa(nip&0xFF);
	//string ip(tmp);
	return std::string("Type: ") + itoa(getType()) + std::string("; Family: ") + itoa(family) + std::string("; port: ") + itoa(port)+"; address: " + ip;
}


//////////////////////////////////
//
//

//STUNAttributeAddress::STUNAttributeAddress(int type, uint16_t port, char *addr)

STUNAttributeMappedAddress::STUNAttributeMappedAddress(char *addr, uint16_t port)
		:STUNAttributeAddress(STUNAttribute::MAPPED_ADDRESS, port, addr)
{

}
		
STUNAttributeMappedAddress::STUNAttributeMappedAddress(
	int length,
	unsigned char* data):
		 STUNAttributeAddress(STUNAttribute::MAPPED_ADDRESS, data, length)
{

}

//////////////////////////////////
//

STUNAttributeResponseAddress::STUNAttributeResponseAddress(char *addr, uint16_t port):
		 STUNAttributeAddress(STUNAttribute::RESPONSE_ADDRESS, port, addr)
{

}

STUNAttributeResponseAddress::STUNAttributeResponseAddress(
	int length,
	unsigned char* data):
		 STUNAttributeAddress(STUNAttribute::RESPONSE_ADDRESS, data, length)
{

}

//////////////////////////////////
//

STUNAttributeChangedAddress::STUNAttributeChangedAddress(char *addr, uint16_t port):
		 STUNAttributeAddress(STUNAttribute::CHANGED_ADDRESS, port, addr)
{

}

STUNAttributeChangedAddress::STUNAttributeChangedAddress(
	int length,
	unsigned char* data):
		 STUNAttributeAddress(STUNAttribute::CHANGED_ADDRESS, data, length)
{

}

//////////////////////////////////
//

STUNAttributeSourceAddress::STUNAttributeSourceAddress(char *addr, uint16_t port):
		 STUNAttributeAddress(STUNAttribute::SOURCE_ADDRESS, port, addr)
{

}

STUNAttributeSourceAddress::STUNAttributeSourceAddress(
	int length,
	unsigned char* data):
		 STUNAttributeAddress(STUNAttribute::SOURCE_ADDRESS, data, length)
{

}

////////////////////////////////
//



STUNAttributeChangeRequest::STUNAttributeChangeRequest(unsigned char *data, int /*length*/): //TODO: use length parameter
		STUNAttribute(STUNAttribute::CHANGE_REQUEST)
{
	uint32_t flags = ntohl( *((uint32_t *)data) );
	changeIP = (flags & CHANGE_IP_MASK) != 0;
	changePort = (flags & CHANGE_PORT_MASK) != 0;
}

STUNAttributeChangeRequest::STUNAttributeChangeRequest(bool changeIP, bool changePort): 
		STUNAttribute(STUNAttribute::CHANGE_REQUEST), 
		changeIP(changeIP),
		changePort(changePort)
{
}

int STUNAttributeChangeRequest::getValue(unsigned char *buf){
	uint32_t *uiptr = (uint32_t *)buf;
	*uiptr=0;
	if (changeIP)
		(*uiptr)|=CHANGE_IP_MASK;
	if (changePort)
		(*uiptr)|=CHANGE_PORT_MASK;
		
	(*uiptr)=htonl(*uiptr);
	
	return getValueLength();
}

int STUNAttributeChangeRequest::getValueLength(){
	return 4;
}

//////////

STUNAttributeString::STUNAttributeString(int type, char *str, int strlen)
		:STUNAttribute(type)
{
	this->str = new char[strlen];
	this->length=strlen;
	memcpy(this->str, str, strlen);
}

STUNAttributeString::STUNAttributeString(int type,int length, unsigned char*data)
		:STUNAttribute(type)
{
	assert(length%4==0);
	this->length = length;
	this->str = new char[length];
	memcpy(this->str, data, length);
}
	
	
STUNAttributeString::~STUNAttributeString(){
	delete []str;
}

int STUNAttributeString::getValue(unsigned char *buf){
	memcpy(buf,str, length);
	return getValueLength();
}

int STUNAttributeString::getValueLength(){
	return length;
}

///////////////////

STUNAttributeUsername::STUNAttributeUsername(char *username, int length)
		:STUNAttributeString(STUNAttribute::USERNAME, username, length)
{
	
}

STUNAttributeUsername::STUNAttributeUsername(int length, unsigned char *data)
		:STUNAttributeString(STUNAttribute::USERNAME, length, data)
{
	
}

/////////////////


STUNAttributePassword::STUNAttributePassword(char *password, int length)
		:STUNAttributeString(STUNAttribute::PASSWORD, password, length)
{
	
}

STUNAttributePassword::STUNAttributePassword(int length, unsigned char *data)
		:STUNAttributeString(STUNAttribute::PASSWORD, length, data)
{
	
}

///////////////////

STUNAttributeErrorCode::STUNAttributeErrorCode(char *msg, int errorCode)
		:STUNAttribute(STUNAttribute::ERROR_CODE)
{
	this->errorCode = errorCode;
	
	messageLength = (int)strlen(msg);
	if (messageLength%4!=0)				//add padding length
		messageLength+= 4-messageLength%4;
	message = new char[messageLength];

	memcpy(message,msg, strlen(msg));
	
	for (int i=(int)strlen(msg); i<messageLength; i++)	//pad with spaces
		message[i]=' ';
}

STUNAttributeErrorCode::STUNAttributeErrorCode(int length, unsigned char *data)
		:STUNAttribute(STUNAttribute::ERROR_CODE)
{
	uint32_t *uip = (uint32_t *)data;
	int minor = (*uip) & 0xFF;
	int major = ((*uip) & 0x0700) >> 8;
	errorCode = major*100 + minor;
	if (length>4){
		message = new char[length-4];
		messageLength= length-4;
		memcpy(message, &uip[1], length-4);
	}else{
		messageLength=0;
		message=NULL;
	}
	
}

STUNAttributeErrorCode::~STUNAttributeErrorCode(){
	if (message!=NULL)
		delete []message;
}

int STUNAttributeErrorCode::getValue(unsigned char *buf){
	uint32_t *uip = (uint32_t *)buf;
	*uip = (errorCode/100 << 8) | errorCode % 100;
	memcpy(&buf[4], message, messageLength);
	return messageLength; //XXX: Is this correct?
}

int STUNAttributeErrorCode::getValueLength(){
	return 4+messageLength;
}


STUNAttributeReflectedFrom::STUNAttributeReflectedFrom(char *addr, uint16_t port)
		:STUNAttributeAddress(STUNAttribute::REFLECTED_FROM, port, addr)
{

}
		
STUNAttributeReflectedFrom::STUNAttributeReflectedFrom(
	int length,
	unsigned char* data):
		 STUNAttributeAddress(STUNAttribute::REFLECTED_FROM, data, length)
{

}

/////////////////////////

STUNAttributeUnknownAttributes::STUNAttributeUnknownAttributes(uint16_t *attr, int n_attribs)
		:STUNAttribute(STUNAttribute::UNKNOWN_ATTRIBUTES), nAttributes(n_attribs)
{
	if (nAttributes>0){
		attributes = new uint16_t[nAttributes];
		memcpy(attributes,attr,nAttributes*sizeof(uint16_t));
	}else 
		attributes=NULL;

}

STUNAttributeUnknownAttributes::~STUNAttributeUnknownAttributes(){
	if (attributes!=NULL)
		delete attributes;
}
		
STUNAttributeUnknownAttributes::STUNAttributeUnknownAttributes(
	int length,
	unsigned char* data):
		 STUNAttribute(STUNAttribute::UNKNOWN_ATTRIBUTES)
{
	nAttributes=length/2;
	
	if (nAttributes>0){
		attributes = new uint16_t[nAttributes];
		memcpy(attributes,data,nAttributes*sizeof(uint16_t));
	}else 
		attributes=NULL;
}


int STUNAttributeUnknownAttributes::getValue(unsigned char *buf){
	memcpy(buf, attributes, nAttributes * sizeof(uint16_t));
	return nAttributes * sizeof(uint16_t); //XXX: Is this correct?
}

int STUNAttributeUnknownAttributes::getValueLength(){
	return nAttributes*sizeof(uint16_t);
}

