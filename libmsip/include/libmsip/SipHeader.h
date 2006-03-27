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

using namespace std;

class SipHeaderValue;

typedef MRef<SipHeaderValue*>(*SipHeaderFactoryFuncPtr)(const string & buf);

class LIBMSIP_API SipHeaderFactories{
	public:
		void addFactory(string contentType, SipHeaderFactoryFuncPtr);
		SipHeaderFactoryFuncPtr getFactory(const string contentType);

	private:
		map<string, SipHeaderFactoryFuncPtr > factories;
};

class LIBMSIP_API SipHeaderParameter:public MObject{
	public:
		SipHeaderParameter(string parseFrom);
		SipHeaderParameter(string key, string value, bool hasEqual);	//hasEqual is there to support ;lr
		string getMemObjectType(){return "SipHeaderParameter";}
		string getKey(){return key;}
		string getValue(){return value;}
		void setValue(string v){value=v;}
		string getString();
		
	private:
		string key;
		string value;
		bool hasEqual;

};

class LIBMSIP_API SipHeaderValue : public MObject{
	public:
		SipHeaderValue(int type, const string &hName);
		virtual string getString()=0;	
		int getType(){return type;}

		void setParameter(string key, string val){
			if (val.size()>0){
				MRef<SipHeaderParameter*> param = new SipHeaderParameter(key,val,true);
				addParameter(param);
			}else{
				removeParameter(key);
			}
		}

		void addParameter(MRef<SipHeaderParameter*> p){
			//If key already exist, change the existing value
			//(a key can only exist once)
			for (int i=0; i< parameters.size();i++){
				if (parameters[i]->getKey()==p->getKey()){
					parameters[i]->setValue(p->getValue());
					//cerr<<"p->getValue() "+p->getValue()<<endl;
					return;
				}
			}
			parameters.push_back(p);
		}

		bool hasParameter(const string &key){
			for (int i=0; i< parameters.size();i++){
				if (parameters[i]->getKey()==key){
					return true;
				}
			}
			return false;
		}

		string getParameter(string key){
			for (int i=0; i< parameters.size();i++){
				if (parameters[i]->getKey()==key){
					return parameters[i]->getValue();
				}
			}
			return "";
		}

		void removeParameter(string key){
			for (int i=0; i< parameters.size(); i++){
				if (parameters[i]->getKey()==key){
					parameters.remove(i);
					i=0;
				}
			}
		
		}

		string getStringWithParameters(){
			string parameterList;
			int nparam = parameters.size();
			for (int i=0; i< nparam; i++){
				parameterList+=";"+parameters[i]->getString();
			}
			return getString()+parameterList;
		}

		const string &headerName;
	protected:
		int type;
		minilist<MRef<SipHeaderParameter*> > parameters;
};



class LIBMSIP_API SipHeader : public MObject{
	public:
		static SipHeaderFactories headerFactories;
		
		
                SipHeader(MRef<SipHeaderValue*> value);
		virtual ~SipHeader();

		string getString();
		void addHeaderValue(MRef<SipHeaderValue*> v);

                virtual std::string getMemObjectType(){return "SipHeader";}

		int32_t getType(){return type;}
		int getNoValues(){return headerValues.size();}
		MRef<SipHeaderValue *> getHeaderValue(int i){
			assert(i < headerValues.size() );
			return headerValues[i];
		}

		static MRef<SipHeader *> parseHeader(const string &buildFrom);

	private:
		int32_t type;
		string headerName;

		minilist<MRef<SipHeaderValue*> > headerValues;
};


#endif
