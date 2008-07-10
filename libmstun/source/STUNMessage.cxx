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

#include<libmstun/STUNMessage.h>

#ifdef _MSC_VER
#	include<winsock2.h>
#	include<io.h>
#else
#	include<unistd.h>
#	ifdef HAVE_NETINET_IN_H
#		include<netinet/in.h>
#	endif
#endif

#include<assert.h>

#include<iostream>
#include<stdlib.h>

using namespace std;

MessageHeader::MessageHeader(int type): messageType(type), messageLength(0){
	for (int i=0; i< 16; i++)
		transactionID[i] = rand()%256;
}

int MessageHeader::getData(unsigned char *buf){
	uint16_t *p = (uint16_t *)buf;
	p[0]=htons(messageType);
	p[1]=htons(messageLength);
	for (int i=0; i<16; i++)
		buf[4+i]=transactionID[i];
	
	return getHeaderLength();
}

void MessageHeader::setPayloadLength(int len){
	messageLength=len;
}

int MessageHeader::getPayloadLength(){
	return messageLength;
}

void MessageHeader::setType(int type){
	messageType = type;
}

void MessageHeader::setTransactionID(unsigned char *id){
	for (int i=0; i< 16; i++)
		transactionID[i] = id[i];
}


//////////////////////////////////
//

const int STUNMessage::BINDING_REQUEST=0x0001;
const int STUNMessage::BINDING_RESPONSE=0x0101;
const int STUNMessage::BINDING_ERROR_RESPONSE=0x0111;
const int STUNMessage::SHARED_SECRET_REQUEST=0x0002;
const int STUNMessage::SHARES_SECRET_RESPONSE=0x0102;
const int STUNMessage::SHARED_SECRET_ERROR_RESPONSE=0x0112;


STUNMessage::STUNMessage(unsigned char *data, int length): header(0)
{
	uint16_t *sptr = (uint16_t*)data;
	header.setPayloadLength(ntohs(sptr[1]));	
	header.setTransactionID(&data[4]);
	uint16_t type = ntohs(*sptr);
	header.setType(type);
	
	parseAttributes(&data[20], header.getPayloadLength());
}

STUNMessage::STUNMessage(int type):header(type){
//	header.setType(type);	
}

STUNMessage::~STUNMessage(){

}



bool STUNMessage::sameTransactionID(STUNMessage &msg){
	for (int i=0; i<16; i++)
		if (header.transactionID[i]!=msg.header.transactionID[i])
			return false;
	return true;
}

void STUNMessage::addAttribute(STUNAttribute *a){
	attributes.push_back(a);
}

void STUNMessage::parseAttributes(unsigned char *data, int length){
	int nleft = length;
	int index=0;
	do{
		int attrLen=0;
		STUNAttribute *attr = STUNAttribute::parseAttribute(&data[index], nleft, attrLen);
//		cerr << "Parsed Attribute: "<<attr->getDesc()<<endl;
		if (attr != NULL) addAttribute(attr);
		nleft-=attrLen;
		index+=attrLen;
//		cerr << "nleft="<<nleft << endl;
	}while(nleft>0);
	assert(nleft==0);
}

unsigned char* STUNMessage::getMessageData(int &retLength){
	int length=0;
	length+=header.getHeaderLength();
	list<STUNAttribute*>::iterator i;
	for (i=attributes.begin(); i!=attributes.end(); i++)
		length+=2+2+(*i)->getValueLength(); //T+L+V
	
	header.setPayloadLength(length-header.getHeaderLength());
	
	unsigned char *rawData = new unsigned char[length];

	int index = header.getData(rawData);

	for (i=attributes.begin(); i!=attributes.end(); i++)
		index+= (*i)->getMessageDataTLV(&rawData[index]);
	
	assert(index==length);
	retLength=length;
	return rawData;
}

void STUNMessage::sendMessage(int fd){
	int retLength;
	unsigned char *data = getMessageData(retLength);
	
#ifdef _MSC_VER
	::_write(fd,data, (unsigned int)retLength);
#else
	write(fd,data, retLength);
#endif
	delete []data;
}

STUNAttribute *STUNMessage::getAttribute(int type){
	for (list<STUNAttribute*>::iterator i=attributes.begin(); i!=attributes.end(); i++)
		if ((*i)->getType() == type)
			return *i;
	return NULL;

}


