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
 * 	SipMessage.h
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/



#ifndef sipMessage_h
#define sipMessage_h


#include<libmutil/minilist.h>
#include<libmsip/SipHeader.h>
#include<libmsip/SipHeaderFrom.h>
#include<libmsip/SipHeaderTo.h>
#include<libmsip/SipURI.h>
#include<libmsip/SipMessageContent.h>
#include<sys/types.h>
#include<libmutil/MemObject.h>
#include<libmsip/SdpPacket.h>


/**
 * Base class for the SIP packet classes.
 * Contains the fundamental fields for all SIP packets, methods
 * to compile packets, read from sockets and send to sockets.
 * 
 * @author Erik Eliasson, eliasson@it.kth.se
 */
class SipMessage : public MObject{

	public:

		SipMessage(string branch, int type);
		SipMessage(int type, string &build_from);
		virtual ~SipMessage();

                /**
                 * There are two ways to determine what kind of SIP message
                 * a SipMessage really is - dynamic_cast and getType.
                 * getType is ~10 times faster and does not require to test
                 * for each type individualy.
                 * @return The type of SIP message as integer. The
                 *     different types are defined (static const) in the 
                 *     respective sub-classes, for example SipBye::type.
                 */
		int getType();
		string getTypeString(){
			char *types[11]={"TYPE0","INVITE","REGISTER","BYE","CANCEL","ACK","NOTIFY","SUBSCRIBE","RESPONSE","MESSAGE","TYPE10"};
			if (getType()>10)
				return "UNDEFINED";
			return string(types[getType()]);
		}
		
                /**
                 * Adds one header (specialization of SipHeader) to the SIP
                 * message. 
                 * @param header        Header to add.
                 */
		virtual void addHeader(MRef<SipHeader*> header);

                /**
                 * @return Size in bytes of the content of the SIP message.
                 * This is the number in the "ContentLength" header.
                 */
		int32_t getContentLength();
		
                
                //----------------------------------------------------
                // The following nine methods are used to simplify
                // using SIP Messages by extracting commonly used
                // parts.
		
                /**
                 * @return The "From:" header.
                 */
		MRef<SipHeaderFrom*> getHeaderFrom();
		
                /**
                 * @return The "To:" header.
                 */
                MRef<SipHeaderTo*> getHeaderTo();
		
                /**
                 * @param content       Content of the SIP message. This
                 *                      affects the ContentLength and any
                 *                      previous content will be removed.
                 */
		//void setContent(MRef<SdpPacket*> content);
		void setContent(MRef<SipMessageContent*> content);

                /**
                 * @return Content of the SIP message
                 */
		MRef<SipMessageContent*> getContent();
		
                /**
                 * @return The command sequence identifier (integer part).
                 */
		int32_t getCSeq();

                /**
                 * @return The command sequence identifier (method part).
                 */
		string getCSeqMethod();
		
                /**
                 * @return The branch parameter associated with the top
		 * most via header, or an empty string if there is no via
		 * header or if the branch is not set in the topmost via..
                 */
		string getFirstViaBranch();
		string getLastViaBranch();
		

                /**
                 * @return Call ID in the SIP message.
                 */
		string getCallId();

                /**
                 * @return The URI in the "From:" header.
                 */
		SipURI getFrom();
		
                /**
                 * @return The URI in the "To:" header.
                 */
                SipURI getTo();
                

		void removeAllViaHeaders();
		
                /**
                 * @return The complete message as a string.
                 */
		virtual string getString()=0;
		
                /**
                 * @return The headers plus the content as a string. This
                 * is the complete SIP package minus the first line.
                 */
                virtual string getHeadersAndContent();

		/**
		 * @return The warning message contained in Warning: header
		 */
		string getWarningMessage();

                
///		void setSocket(Socket* sock){socket = sock;};
///		Socket* getSocket(){return socket;};

		list<string> getRouteSet();


//		vector<MRef<SipHeader*> > headers;
		
		friend ostream & operator<<(ostream &out, SipMessage &);
                std::string getDescription();
		

		int getNoHeaders();
		MRef<SipHeader*> getHeaderNo(int i);
		MRef<SipHeader*> getHeaderOfType(int t);
		

		
		void setDestinationBranch(string b){branch = b;}
		
		string getDestinationBranch();
		

	protected:

//		vector<MRef<SipHeader*> > getHeaders(){return headers;};
		minilist<MRef<SipHeader*> > *getHeaders(){return &headers;};


		minilist<MRef<SipHeader*> > headers;
		
		bool addLine(string line);
		MRef<SipMessageContent*> content;
		//MRef<SdpPacket*> content;
		// Socket on which the packet was received
///		Socket* socket;
	private: 
		string getViaHeaderBranch(bool first);
		string branch;
		int type;
};

#endif
