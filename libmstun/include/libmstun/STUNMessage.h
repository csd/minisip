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

#ifndef STUNMESSAGE_H
#define STUNMESSAGE_H

#include<libmstun/config.h>

#include<list>
#include<libmstun/STUNAttributes.h>

/**
 * Defines the message header that is common to all STUN messages.
 * @author Erik Eliasson
*/
class LIBMSTUN_API MessageHeader{
	public:
		MessageHeader(int type);
		
		void setPayloadLength(int len);
		int getPayloadLength();
		void setType(int type);
		void setTransactionID(unsigned char *id);
		int getHeaderLength(){return 20;}

		int getData(unsigned char*buf);

		unsigned char transactionID[16];
	private:
		uint16_t messageType;
		uint16_t messageLength;

};

/**
 * Declares the super class of all STUN messages.
 * @author Erik Eliasson
*/
class LIBMSTUN_API STUNMessage{
	public:
		static const int BINDING_REQUEST;
		static const int BINDING_RESPONSE;
		static const int BINDING_ERROR_RESPONSE;
		static const int SHARED_SECRET_REQUEST;
		static const int SHARES_SECRET_RESPONSE;
		static const int SHARED_SECRET_ERROR_RESPONSE;

		STUNMessage(unsigned char*buf, int length);
		STUNMessage(int type);
		virtual ~STUNMessage();

		
//		static STUNMessage *parseMessage(unsigned char *data, int length);

		bool sameTransactionID(STUNMessage &mgs);

		void addAttribute(STUNAttribute *a);

		unsigned char* getMessageData(int &retLength); //the user is responsible for deleteing the data
		void sendMessage(int sock);

//		virtual string getDesc()=0;

		STUNAttribute *getAttribute(int type);
		
	protected:
		void parseAttributes(unsigned char *data, int length);

		MessageHeader header;
		std::list<STUNAttribute *> attributes;
	private:
};

#endif
