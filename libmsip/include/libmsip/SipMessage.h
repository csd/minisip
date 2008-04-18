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
#include<libmutil/SipUri.h>
#include<libmsip/SipMessageContent.h>
#include<libmutil/MemObject.h>
#include<libmsip/SipMessageContentFactory.h>
#include<libmnetutil/Socket.h>

class SipHeaderValueContact;
class SipHeaderValueFrom;
class SipHeaderValueTo;
class SipHeaderValueVia;
class SipHeaderValueProxyAuthenticate;
class SipHeaderValueWWWAuthenticate;


MRef<SipMessageContent*> sipSipMessageContentFactory(const std::string & buf, const std::string & ContentType);

/**
 * Base class for the SIP packet classes.
 * Contains the fundamental fields for all SIP packets, methods
 * to compile packets, read from sockets and send to sockets.
 * 
 * @author Erik Eliasson, eliasson@it.kth.se
 */
class LIBMSIP_API SipMessage : public SipMessageContent{

	public:
		static const std::string anyType;
		
		virtual std::string getContentType() const { return "message/sipfrag"; };

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
		 */
		SipMessage();



		
		/**
		 * Creates a SIP message from a buffer. This superclass
		 * parses the buffer and creates headers and content.
		 */
		SipMessage(std::string &build_from);
	public:
		
		virtual ~SipMessage();

		static MRef<SipMessage*> createMessage(std::string &buf); 

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
		virtual const std::string& getType()=0;

		/**
		* Adds one header (specialization of SipHeader) to the SIP
		* message. The header added first to the SIP message will
		* also appear first in the string representation of the
		* message returned by getString().
		* @param header        Header to add.
		*/
		void addHeader(MRef<SipHeader*> header);

		/**
		 * Adds header h just before any other header of the same
		 * type to the message.
		 */

		void addBefore(MRef<SipHeader*> h);

                /**
                 * Returns true if the message contains a SIP "Require" header with
                 * the extension given as argument to this method.
                 * @param extension     Extension to check for in the
                 *                      "Require" headers.
                 */
		bool requires(std::string extension);

                /**
                 * Returns a list of required extensions listed in
		 * "Require" headers.
		 * @return list of required extensions.
                 */
		std::list<std::string> getRequired();

		/**
		 * Returns true if the message contains a SIP "Supported" header with
		 * the extension given as argument to this method.
		 * @param extension     Extension to check for in the
		 *                      "Supported" headers.
		 */
		bool supported(std::string extension);



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
		std::string getCSeqMethod();
		
		/**
		* @return The top most via header, or a null reference
		* if there is no via header in the message.
		*/
		MRef<SipHeaderValueVia*> getFirstVia();

		void removeFirstVia();

		/**
		* @return Call ID in the SIP message.
		*/
		std::string getCallId();

		/**
		* @return The URI in the "From:" header.
		*/
		SipUri getFrom();
		
		/**
		* @return The URI in the "To:" header.
		*/
		SipUri getTo();
                
		/**
		* @return The complete message as a string.
		*/
		virtual std::string getString() const =0;
		
		/**
		* @return The headers plus the content as a string. This
		* is the complete SIP package minus the first line.
		*/
		virtual std::string getHeadersAndContent() const;

		/**
		* @return The warning message contained in Warning: header
		*/
		std::string getWarningMessage();

		
		friend std::ostream & operator<<(std::ostream &out, SipMessage &);
		std::string getDescription();
		
		/**
		*
		* @return Branch parameter associated with this message.
		* If it is non empty string it identifies which
		* transaction it belongs to.
		*/
		std::string getBranch();

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
		std::list<std::string> getRouteSet();

		/**
		 * Set the socket used by SipLayerTransport to send
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
		std::string getAuthenticateProperty(std::string prop);

		/**
		 * @return Returns the i:th Proxy-Authenticate header,
		 * or NULL if it doesn't exist.
		 */
		MRef<SipHeaderValueProxyAuthenticate*> getHeaderValueProxyAuthenticate(int i);

		/**
		 * @return Returns the i:th WWW-Authenticate header,
		 * or NULL if it doesn't exist.
		 */
		MRef<SipHeaderValueWWWAuthenticate*> getHeaderValueWWWAuthenticate(int i);


		/**
		 * Removes a header value from the packet. If it is the
		 * only value in the header then the entire header is
		 * removed as well.
		 */
		void removeHeaderValue(MRef<SipHeaderValue*>);

	protected:

		/**
		 * Parses one line of text to a SIP header and adds it to
		 * the message. If the message can be parsed depends if a
		 * "factory" is available for the header type (and if the
		 * string is a valid header of that type)
		 */
		bool addLine(std::string line);
		
		
		void removeHeader(MRef<SipHeader*> header);

		/**
		 * Gets the i:th header of a certain type. In most cases,
		 * users want to access header values, and then they should
		 * use getHeaderValueNo() instead.
		 */
		MRef<SipHeader*> getHeaderOfType(int t, int i=0);


	private: 
		minilist<MRef<SipHeader*> > headers;
		MRef<SipMessageContent*> content;

		int parseHeaders(const std::string &buf, int startIndex);

		MRef<Socket*> sock;
};

#endif
