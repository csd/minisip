/*
  Copyright (C) 2005, 2004 Erik Eliasson, Johan Bilien
  Copyright (C) 2006 Mikael Magnusson
  
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
 *          Mikael Magnusson <mikma@users.sourceforge.net>
*/


/* Name
 * 	SipHeader.h
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#ifndef SIPHEADER_H
#define SIPHEADER_H

#include<libmsip/libmsip_config.h>

#include<libmutil/MemObject.h>
#include<libmutil/minilist.h>
#include<libmutil/mtypes.h>
#include<map>

#include<sys/types.h>

/**
 * @author Erik Eliasson
*/

#define SIP_HEADER_TYPE_ACCEPT			0
#define SIP_HEADER_TYPE_AUTHORIZATION		1
#define SIP_HEADER_TYPE_CALLID			2
#define SIP_HEADER_TYPE_CONTACT			3
#define SIP_HEADER_TYPE_CONTENTLENGTH		4
#define SIP_HEADER_TYPE_CONTENTTYPE		5
#define SIP_HEADER_TYPE_CSEQ			6
#define SIP_HEADER_TYPE_EVENT			7
#define SIP_HEADER_TYPE_EXPIRES			8
#define SIP_HEADER_TYPE_FROM			9
#define SIP_HEADER_TYPE_MAXFORWARDS		10
#define SIP_HEADER_TYPE_PROXYAUTHENTICATE	11
#define SIP_HEADER_TYPE_PROXYAUTHORIZATION	12
#define SIP_HEADER_TYPE_RECORDROUTE		13
#define SIP_HEADER_TYPE_ROUTE			14
#define SIP_HEADER_TYPE_SUBJECT			15
#define SIP_HEADER_TYPE_TO			16
#define SIP_HEADER_TYPE_USERAGENT		17
#define SIP_HEADER_TYPE_VIA			18
#define SIP_HEADER_TYPE_UNKNOWN                 19
#define SIP_HEADER_TYPE_ACCEPTCONTACT		20
#define SIP_HEADER_TYPE_WARNING			21
#define SIP_HEADER_TYPE_REFERTO			22
#define SIP_HEADER_TYPE_WWWAUTHENTICATE		25
#define SIP_HEADER_TYPE_SUPPORTED		26
#define SIP_HEADER_TYPE_UNSUPPORTED		27
#define SIP_HEADER_TYPE_REQUIRE			28
#define SIP_HEADER_TYPE_RACK			29
#define SIP_HEADER_TYPE_RSEQ			30
#define SIP_HEADER_TYPE_ALLOWEVENTS		31
#define SIP_HEADER_TYPE_SUBSCRIPTIONSTATE	32
#define SIP_HEADER_TYPE_ALLOW			33
#define SIP_HEADER_TYPE_SNAKESM			34

class SipHeaderValue;

typedef MRef<SipHeaderValue*>(*SipHeaderFactoryFuncPtr)(const std::string & buf);

class LIBMSIP_API SipHeaderFactories{
	public:
		void addFactory(std::string contentType, SipHeaderFactoryFuncPtr);
		SipHeaderFactoryFuncPtr getFactory(const std::string contentType) const;

	private:
		std::map<std::string, SipHeaderFactoryFuncPtr > factories;
};

class LIBMSIP_API SipHeaderParameter:public MObject{
	public:
		SipHeaderParameter(std::string parseFrom);
		SipHeaderParameter(std::string key, std::string value, bool hasEqual);	//hasEqual is there to support ;lr
		std::string getMemObjectType() const {return "SipHeaderParameter";}
		std::string getKey() const;
		std::string getValue() const;
		void setValue(std::string v);
		std::string getString() const;
		
	private:
		std::string key;
		std::string value;
		bool hasEqual;

};

class LIBMSIP_API SipHeaderValue : public MObject{
	public:
		SipHeaderValue(int type, const std::string &hName);
		virtual std::string getString() const =0;	
		int getType(){return type;}

		void setParameter(std::string key, std::string val);

		void addParameter(MRef<SipHeaderParameter*> p);

		bool hasParameter(const std::string &key) const;

		std::string getParameter(std::string key) const;

		void removeParameter(std::string key);

		std::string getStringWithParameters() const ;

		const std::string &headerName;
	protected:

		virtual char getFirstParameterSeparator() const {return ';';}
		virtual char getParameterSeparator() const {return ';';}

		int type;
		minilist<MRef<SipHeaderParameter*> > parameters;
};



class LIBMSIP_API SipHeader : public MObject{
	public:
		static SipHeaderFactories headerFactories;

		/**
		 * @param value	Initial/first header value. The type
		 * 		if the header is set to the type of the 
		 * 		header value.
		 */
                SipHeader(MRef<SipHeaderValue*> value);
		virtual ~SipHeader();

		std::string getString() const ;
		void addHeaderValue(MRef<SipHeaderValue*> v);

                virtual std::string getMemObjectType() const {return "SipHeader";}

		int32_t getType() const;
		int getNoValues() const;
		MRef<SipHeaderValue *> getHeaderValue(int i) const;
		void removeHeaderValue(int i);

		static MRef<SipHeader *> parseHeader(const std::string &buildFrom);

	private:
		int32_t type;
		std::string headerName;

		minilist<MRef<SipHeaderValue*> > headerValues;
};


#endif
