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
 * 	SipMessage.h
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/



#ifndef sipMessage_h
#define sipMessage_h

#include<libmsip/libmsip_config.h>

#include<libmutil/minilist.h>
#include<libmsip/SipHeader.h>
#include<libmsip/SipUri.h>
#include<libmsip/SipMessageContent.h>
#include<libmutil/MemObject.h>
#include<libmsip/SipMessageContentFactory.h>
#include<libmnetutil/Socket.h>

class SipHeaderValueContact;
class SipHeaderValueFrom;
class SipHeaderValueTo;
class SipHeaderValueVia;


MRef<SipMessageContent*> sipSipMessageContentFactory(const string & buf, const string & ContentType);

/**
 * Base class for the SIP packet classes.
 * Contains the fundamental fields for all SIP packets, methods
 * to compile packets, read from sockets and send to sockets.
 * 
 * @author Erik Eliasson, eliasson@it.kth.se
 */
class LIBMSIP_API SipMessage : public SipMessageContent{

	public:
		static const string anyType;
		
		virtual std::string getContentType(){ return "message/sipfrag"; };

		/**
		 * Registry of the registred SIP message content parsers.
		 * Adding support for a new content type means adding
		 * a factory to this collection.
		 */
		static SMCFCollection contentFactories;


	protected:
		/**
		 * Parent of all SIP messages that only sets branch and
		 * type. Headers and content must be added to the message
		 * to make it a valid SIP message. This is typically done
		 * in the constructor of the sub-class.
		 * @param branch	All SIP messages have a branch
		 * 			value (which transaction they
		 * 			belong to). This argument may be an
		 * 			empty string.
		 * @param type		Each SIP message has a type. It 
		 * 			is a string that for a method
		 * 			is the method string (for example
		 * 			"INVITE"), and for a response it
		 * 			is the string in SipResponse::type.
		 */
		SipMessage(string branch);



		
		/**
		 * Creates a SIP message from a buffer. This superclass
		 * parses the buffer and creates headers and content.
		 */
		SipMessage(int dummy, string &build_from);
	public:
		
		virtual ~SipMessage();

		static MRef<SipMessage*> createMessage(string &buf); 

		/**
			* There are two ways to determine what kind of SIP message
			* a SipMessage really is - dynamic_cast and getType.
			* getType does not require run-time type
			* information support (sometimes disabled, for
			* example on handheld devices - i.e. Visual Studio
			* for WinCE) and you don't have to test * for each 
			* type individualy.
			*
			* Note that the method returns a reference to an
			* internal member and that you therefore must
			* be careful to not use the reference after the
			* message has been deleted. The string is not
			* a reference to the message, and it will therefore
			* be garbage collected if there is no other
			* reference to it.
			* 
			* @return The type of SIP message as string. The
			*     different types are defined as the method of
			*     a request of the string SipResponse::type for
			*     a response.
			*/
		virtual const string& getType()=0;

		/**
		* Adds one header (specialization of SipHeader) to the SIP
		* message. The header added first to the SIP message will
		* also appear first in the string representation of the
		* message returned by getString().
		* @param header        Header to add.
		*/
		void addHeader(MRef<SipHeader*> header);


                /**
                 * Returns true if the message contains a SIP "Require" header with
                 * the extension given as argument to this method.
                 * @param extension     Extension to check for in the
                 *                      "Require" headers.
                 */
		bool requires(string extension);


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
		MRef<SipHeaderValueFrom*> getHeaderValueFrom();
		
		/**
		* @return The "To:" header.
		*/
		MRef<SipHeaderValueTo*> getHeaderValueTo();
		
		/**
		* @return The "Contact:" header.
		*/		
		MRef<SipHeaderValueContact*> getHeaderValueContact();
		
		/**
		* Sets the content of the SIP message. A SIP message can
		* only contain one content. This single content can be a
		* multipart content (see SipMIMEContent).
		* 
		* @param content       Content of the SIP message. This
		*                      affects the ContentLength and any
		*                      previous content will be removed.
		*/
		void setContent(MRef<SipMessageContent*> content);

		/**
		* @return Content of the SIP message
		*/
		MRef<SipMessageContent *> getContent();
		
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
		* header or if the branch is not set in the topmost via
		* header.
		*/
		string getFirstViaBranch();

		/**
		* @return The top most via header, or a null reference
		* if there is no via header in the message.
		*/
		MRef<SipHeaderValueVia*> getFirstVia();

		/**
		* @return The branch parameter in the last via header in
		* the message.
		*/
		string getLastViaBranch();
		
		/**
		* @return The last via header or a null reference
		* if there is no via header in the message.
		*/
		MRef<SipHeaderValueVia*> getLastVia();

		/**
		* @return Call ID in the SIP message.
		*/
		string getCallId();

		/**
		* @return The URI in the "From:" header.
		*/
		SipUri getFrom();
		
		/**
		* @return The URI in the "To:" header.
		*/
		SipUri getTo();
                
		/**
		 * Removes all via headers that may be in the message.
		 */
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

		
		friend ostream & operator<<(ostream &out, SipMessage &);
		std::string getDescription();
		
		/**
		*
		* @return Branch parameter associated with this message.
		* If it is non empty string it identifies which
		* transaction it belongs to.
		*/
		string getDestinationBranch();

		/**
		* @return Number of headers in this message. Notice that
		* this is not the number of header values that can be
		* higher.
		*/
		int getNoHeaders();

		/**
		 * @return Returns the i:th header in the message. See
		 * getHeaderValueNo() to get the i:th header value of a
		 * certain type.
		 */
		MRef<SipHeader*> getHeaderNo(int i);
		
		/**
		 * Returns the i:th header value of a specific type. Note
		 * that the i:th header value of a type may be in header
		 * [1..i] of that type since one header can have several
		 * header values.
		 */
		MRef<SipHeaderValue*> getHeaderValueNo(int type, int i);

		/**
		* Return a list with the route set, extracted from the 
		* Record-Route headers (in the same order as in the message,
		* top Record-Route first).
		* Return an empty list if no route-set or error.
		*/
		list<string> getRouteSet();

		/**
		 * Set the socket used by SipMessageTransport to send
		 * the message.
		 */
		void setSocket(MRef<Socket*> sock);

		/**
		 * Get the socket this message was sent to or received from.
		 */
		MRef<Socket*> getSocket();

		/**
		 * Searches the message for a "property" in
		 * WWW-Authenticate or Proxy-Authenticate headers.
		 * A property can be for example "nonce" or "realm".
		 * If both WWW-Authenticate headers are searched first.
		 * 
		 * @param prop	Property to find. For example "nonce".
		 * @return	First property found from top of message.
		 */
		string getAuthenticateProperty(string prop);

	protected:
		void setDestinationBranch(string b){branch = b;}

		minilist<MRef<SipHeader*> > *getHeaders(){return &headers;};

		minilist<MRef<SipHeader*> > headers;
		
		/**
		 * Parses one line of text to a SIP header and adds it to
		 * the message. If the message can be parsed depends if a
		 * "factory" is available for the header type (and if the
		 * string is a valid header of that type)
		 */
		bool addLine(string line);
		
	private: 
		MRef<SipMessageContent*> content;

		/**
		 * Gets the i:th header of a certain type. In most cases,
		 * users are 
		 */
		MRef<SipHeader*> getHeaderOfType(int t, int i=0);
		
		int parseHeaders(const string &buf, int startIndex);
		MRef<SipHeaderValueVia*> getViaHeader(bool first);
		string getViaHeaderBranch(bool first);
		string branch;

		MRef<Socket*> sock;
};

#endif
