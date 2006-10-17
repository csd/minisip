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

#ifndef STUNATTRIBUTES_H
#define STUNATTRIBUTES_H

#include<libmstun/config.h>

#include<libmutil/stringutils.h>


#include <sys/types.h>

#ifdef WIN32
# include<winsock2.h>
#endif

#include<string>

/**
 * Base class for  all STUN attributes.
 * @author Erik Eliasson
*/
class LIBMSTUN_API STUNAttribute{
	public:
		static const int MAPPED_ADDRESS;
		static const int RESPONSE_ADDRESS;
		static const int CHANGE_REQUEST;
		static const int SOURCE_ADDRESS;
		static const int CHANGED_ADDRESS;
		static const int USERNAME;
		static const int PASSWORD;
		static const int MESSAGE_INTEGRITY;	//TODO: implement generation and parsing
		static const int ERROR_CODE;		//TODO: implement generation and parsing
		static const int UNKNOWN_ATTRIBUTES;	//TODO: implement generation and parsing
		static const int REFLECTED_FROM;	//TODO: implement generation and parsing

		
		STUNAttribute(int type);
		virtual ~STUNAttribute();

		/**
		 * Returns what attribute it is. It MUST be one of the
		 * public static constants defined in this class.
		*/
		virtual int getType();

		/**
		 * All subclasses must implement this method that
		 * returns the length of the value of the attribute.
		 * @return	Number of octets in the value part
		 * 		of the attribute.
		*/
		virtual int getValueLength()=0;

		/**
		 * All attributes must implement this method that
		 * returns a string describing the attribute. This
		 * method is mainly used while developing/debugging.
		*/
		virtual std::string getDesc()=0;

		/**
		 * Writes the complete attribute to a buffer (it 
		 * is in TLV form),
		 * @param buf	Buffer to where the attribute will
		 * 		be written.
		 * @return	Number of octets written to the 
		 * 		buffer.
		*/
		virtual int getMessageDataTLV(unsigned char* buf);

		/**
		 * Creates a STUNAttribute from raw binary data.
		 * @param data	Raw binary data from which the 
		 * 		attribute will be created.
		 * @param maxLength Maximum index in the buffer the
		 * 		parsing may access.
		 * @param retParsedLength (OUT) Number of bytes
		 * 		that was found to belong to the
		 * 		parsed attribute.
		 * @return 	A new STUNAttribute parsed from the
		 * 		raw data in the data buffer or NULL
		 *              if recieved an attribute that is
		 *              not understood and is not mandatory 
		 *              to understand
		*/
		static STUNAttribute *parseAttribute(
				unsigned char *data, 
				int maxLength, 
				int &retParsedLength //return parameter - how long the parsed attr is
				);
	protected:

		/**
		 * All subclasses must implement this method that 
		 * writes the attribute value data to a buffer.
		 * @param buf	Buffer where the value data will be written.
		 * @return 	The number of bytes written to the buffer.
		*/
		virtual int getValue(unsigned char *buf)=0;

	private:
		/** The type of the message. This value must be one of the
		 *  constants defined in this class.
		*/
		uint16_t type;
};


/**
 * Base class for all address attributes.
 * @author Erik Eliasson
*/
class LIBMSTUN_API STUNAttributeAddress : public STUNAttribute{
	public:
		/**
		 * Creates a address attribute of a given type from
		 * a port and a address string.
		 * @param type		Type of the message. It must be one
		 * 			of the constants defined in 
		 * 			STUNAttribute.
		 * @param port		Each address attribute has a port.
		 * @param addr		String describing an IP4 address.
		*/
		STUNAttributeAddress(int type, uint16_t port, char *addr);

		/**
		 * Creates/parses an address attribute from a raw data buffer.
		 * @param type		Type of the message. This value is
		 * 			given by the subclass.
		 * @param data		Raw data buffer from which the 
		 * 			address attribute will be parsed.
		 * @param length	Length of the raw data buffer.
		*/
		STUNAttributeAddress(int type, unsigned char *data, int length);

		/**
		 * String description of the attribute as required by the 
		 * superclass.
		 * @return	A string describing the address attribute.
		*/
		virtual std::string getDesc();
		/**
		 * Accesses the binary representation of the IP4 address.
		 * @return The binary IP4 address CONVERTED TO HOST BYTE ORDER.
		*/
		uint32_t getBinaryIP();

		/**
		 * Gives access to the port.
		 * @return The port part of the address attribute.
		*/
		uint16_t getPort();
	protected:

		/**
		 * Writes the value part of the attribute to a buffer
		 * as required by the superclass.
		 * @param buf		Buffer where the value part
		 * 			of the attribute will be 
		 * 			written.
		 * @return		The number of bytes written to
		 * 			the buffer.
		*/
		virtual int getValue(unsigned char *buf);

		/**
		 * @return Returns the number of bytes in the value
		 * part of the address attribute.
		*/
		virtual int getValueLength();
		
	private:
		//Family must be 1 (IP4) according to RFC3489
		unsigned char family;
		uint16_t port;
		uint32_t address;
};

/**
 * Defines the Mapped Address attribute.
 * @author Erik Eliasson
*/
class LIBMSTUN_API STUNAttributeMappedAddress : public STUNAttributeAddress{
	public:
		/**
		 * Creates a Mapped Address attribute from an 
		 * address/port pair.
		*/
		STUNAttributeMappedAddress(char *addr, uint16_t port);

		/**
		 * Parses a MappedAddress attribute from a raw data buffer.
		*/
		STUNAttributeMappedAddress(int length, unsigned char *data);
};

/**
 * Defines the Response Address attribute
 * @author Erik Eliasson
*/
class LIBMSTUN_API STUNAttributeResponseAddress : public STUNAttributeAddress{
	public:
		/**
		 * Creates a Response Address attribute from a
		 * address/port pair.
		*/
		STUNAttributeResponseAddress(char *addr, uint16_t port);

		/**
		 * Parses a Response Address attribute from a
		 * raw data buffer.
		*/
		STUNAttributeResponseAddress(int length, unsigned char *data);
};

/**
 * Defines the Changed Address attribute
 * @author Erik Eliasson
*/
class LIBMSTUN_API STUNAttributeChangedAddress : public STUNAttributeAddress{
	public:
		/**
		 * Creates a Changed Address attribute from an
		 * address/port pair
		*/
		STUNAttributeChangedAddress(char *addr, uint16_t port);

		/**
		 * Parses a Changed Address attribute from a
		 * raw data buffer.
		*/
		STUNAttributeChangedAddress(int length, unsigned char *data);
};

/**
 * Defines the Source Address attribute
 * @author Erik Eliasson
*/
class LIBMSTUN_API STUNAttributeSourceAddress : public STUNAttributeAddress{
	public:
		/**
		 * Creates a Source Address attribute from an
		 * address/port pair.
		*/
		STUNAttributeSourceAddress(char *addr, uint16_t port);

		/**
		 * Parses a Source Address attribute from a
		 * raw data buffer
		*/
		STUNAttributeSourceAddress(int length, unsigned char *data);
};

/*
 * Defines the Change Request attribute
 * @author Erik Eliasson
*/
class LIBMSTUN_API STUNAttributeChangeRequest : public STUNAttribute{
	public:
		/**
		 * Creates a Change Request attribute
		 * @param changeIP	Specifies whether the STUN server
		 * 			should use its alternative IP or not.
		 * @param changePort	Specifies whether the STUN server
		 * 			should use its alternative port or not.
		*/
		STUNAttributeChangeRequest(bool changeIP, bool changePort);
		
		/**
		 * Parses a Change Request attribute from a raw data
		 * buffer.
		*/
		STUNAttributeChangeRequest(unsigned char *data, int length);

		/**
		 * Implements a method returning a string describing the
		 * attribute as required by the superclass STUNAttribute
		 * @return	String describing the attribute.
		*/
		virtual std::string getDesc(){
			return std::string("type: CHANGE REQUEST; changeIP: ") + itoa(changeIP) + "; changePort: " + itoa(changePort);
		}
	protected:
		/**
		 * Writes the value part of the attribute to a buffer
		 * as required by the superclass.
		 * @param buf		Buffer where the value part
		 * 			of the attribute will be 
		 * 			written.
		 * @return		The number of bytes written to
		 * 			the buffer.
		*/
		virtual int getValue(unsigned char *buf);

		/**
		 * @return Returns the number of bytes in the value
		 * part of the address attribute.
		*/
		virtual int getValueLength();
	private:
		bool changeIP;
		bool changePort;
};

/**
 * Defines the superclass for the Username and Password attribute classes.
 * @author Erik Eliasson
*/
class LIBMSTUN_API STUNAttributeString: public STUNAttribute{
	public:
		/**
		 * Creates a string attribute of type "type" given a string.
		 * @param type		must be USERNAME or PASSWORD
		 * @param str		Attribute string that sould be 
		 * 			padded to even four bytes. If not
		 * 			whitespace will be added to the
		 * 			end of the string.
		 * @param strlen	Length of the string.
		*/
		STUNAttributeString(int type, char *str, int strlen);

		/**
		 * Parses a string attribute from a raw data buffer.
		*/
		STUNAttributeString(int type, int length, unsigned char *data);
		~STUNAttributeString();
		
	protected:
		/**
		 * Writes the value part of the attribute to a buffer
		 * as required by the superclass.
		 * @param buf		Buffer where the value part
		 * 			of the attribute will be 
		 * 			written.
		 * @return		The number of bytes written to
		 * 			the buffer.
		*/
		virtual int getValue(unsigned char *buf);
		
		/**
		 * @return Returns the number of bytes in the value
		 * part of the address attribute.
		*/
		virtual int getValueLength();
	private:
		char *str;
		int length;
};

/**
 *  Defines the USERNAME attribute
 *  @author Erik Eliasson
*/
class LIBMSTUN_API STUNAttributeUsername: public STUNAttributeString{
	public:
		/**
		 * Creates a USERNAME attribute from a given string.
		*/
		STUNAttributeUsername(char *username, int length);

		/**
		 * Parses a USERNAME attribute from a raw data buffer.
		*/
		STUNAttributeUsername(int length, unsigned char* data);

		/**
		 * Returns a string describing the attribute as required
		 * by the superclass STUNAttribute.
		*/
		virtual std::string getDesc(){
			return "USERNAME";
		}
	
};

/**
 * Defines the STUNAttributePassword attribute
 * @author Erik Eliasson
*/
class LIBMSTUN_API STUNAttributePassword: public STUNAttributeString{
	public:
		/**
		 * Creates a PASSWORD attribute from a string.
		*/
		STUNAttributePassword(char *password, int length);

		/**
		 * Parses a PASSWORD attribute from a raw data buffer.
		*/
		STUNAttributePassword(int length, unsigned char* data);

		/**
		 * Returns a string describing the attribute as required
		 * by the superclass STUNAttribute.
		*/
		virtual std::string getDesc(){
			return "PASSWORD";
		}

};

/**
 * Defines the Error Code attribute
 * @author Erik Eliasson
*/
class LIBMSTUN_API STUNAttributeErrorCode: public STUNAttribute{
	public:
		/**
		 * Creates an Error Code attribute from
		 * an error message string and an error code.
		 * @param msg 		Clear text string describing
		 * 			the error.
		 * @param errorCode	Error code as specified in RFC3489
		 * 			(1XX-6XX)
		*/
		STUNAttributeErrorCode(char *msg, int errorCode);

		/**
		 * Parses a Error Code attribute from a raw data buffer.
		*/
		STUNAttributeErrorCode(int length, unsigned char* data);
		~STUNAttributeErrorCode();

		/**
		 * Writes the value part of the attribute to a buffer
		 * as required by the superclass.
		 * @param buf		Buffer where the value part
		 * 			of the attribute will be 
		 * 			written.
		 * @return		The number of bytes written to
		 * 			the buffer.
		*/
		virtual int getValue(unsigned char *buf);
		/**
		 * @return Returns the number of bytes in the value
		 * part of the address attribute.
		*/
		virtual int getValueLength();

		/**
		 * Returns a string describing the attribute as 
		 * required by the superclass STUNAttribute.
		*/
		virtual std::string getDesc(){
			return std::string("Error code: ")+itoa(errorCode)+" Message: "+message;
		}

	private:
		/// 1xx-6xx
		int errorCode;
		/** Four byte aligned clear text error message. It is not 
		 * zero terminated.
		*/
		char *message;
		/// Length of the message.
		int messageLength;
		
};

/**
 * Defines the Reflected From attribute.
 * @author Erik Eliasson
*/
class LIBMSTUN_API STUNAttributeReflectedFrom: public STUNAttributeAddress{
	public:
		/**
		 * Creates a Reflected From attribute from an 
		 * address/port pair.
		*/
		STUNAttributeReflectedFrom(char *addr, uint16_t port);

		/**
		 * Parses a Reflected From attribute from a raw data buffer.
		*/
		STUNAttributeReflectedFrom(int length, unsigned char *data);
};

/**
 * Defines the Unknown Attributes attribute.
 * @author Erik Eliasson
*/
class LIBMSTUN_API STUNAttributeUnknownAttributes: public STUNAttribute{
	public:
		/**
		 * Creates a UnknownAttributes attribute from an 
		 * address/port pair.
		*/
		STUNAttributeUnknownAttributes(uint16_t *addr, int n_attribs);

		/**
		 * Parses a Unknown Attributes attribute from a raw data buffer.
		*/
		STUNAttributeUnknownAttributes(int length, unsigned char *data);
		
		~STUNAttributeUnknownAttributes();
		
		/**
		 * Writes the value part of the attribute to a buffer
		 * as required by the superclass.
		 * @param buf		Buffer where the value part
		 * 			of the attribute will be 
		 * 			written.
		 * @return		The number of bytes written to
		 * 			the buffer.
		*/
		virtual int getValue(unsigned char *buf);
		
		/**
		 * @return Returns the number of bytes in the value
		 * part of the address attribute.
		*/
		virtual int getValueLength();

		/**
		 * Returns a string describing the attribute as 
		 * required by the superclass STUNAttribute.
		*/
		virtual std::string getDesc(){
			
			return std::string("Unknown Attributes");
			
		}
		
	private:
		uint16_t *attributes;
		int nAttributes;

};



#endif
