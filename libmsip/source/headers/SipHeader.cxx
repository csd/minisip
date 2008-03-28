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

#include<config.h>

#include<libmsip/SipHeader.h>
#include<libmsip/SipUtils.h>
#include<libmsip/SipHeaderVia.h>
#include<libmsip/SipHeaderFrom.h>
#include<libmsip/SipHeaderTo.h>
#include<libmsip/SipHeaderCallID.h>
#include<libmsip/SipHeaderCSeq.h>
#include<libmsip/SipHeaderContact.h>
#include<libmsip/SipHeaderUserAgent.h>
#include<libmsip/SipHeaderRoute.h>
#include<libmsip/SipHeaderRecordRoute.h>
#include<libmsip/SipHeaderContentLength.h>
#include<libmsip/SipHeaderEvent.h>
#include<libmsip/SipHeaderAccept.h>
#include<libmsip/SipHeaderAcceptContact.h>
#include<libmsip/SipHeaderContentType.h>
#include<libmsip/SipHeaderMaxForwards.h>
#include<libmsip/SipHeaderUnknown.h>
#include<libmsip/SipHeaderWarning.h>

#include<libmutil/stringutils.h>

#include<vector>

using namespace std;

SipHeaderFactories SipHeader::headerFactories=SipHeaderFactories();


SipHeaderParameter::SipHeaderParameter(string parseFrom){
	vector<string> key_val = split(parseFrom,true,'=');
	key = key_val[0];
	hasEqual = false;
	if (key_val.size()==2){
		value = key_val[1];
		hasEqual=true;
	}
	//cerr<<"key_val "+key_val[0]<<endl;
}

SipHeaderParameter::SipHeaderParameter(string k, string val, bool equalSign):key(k),value(val),hasEqual(equalSign){
	
}

std::string SipHeaderParameter::getKey() const {
	return key;
}

std::string SipHeaderParameter::getValue() const {
	return value;
}

void SipHeaderParameter::setValue(std::string v){
	value=v;
}

string SipHeaderParameter::getString() const{
	if (hasEqual || value.size()>0){
		return key+"="+value;
	}else{
		return key;
	}
}


void SipHeaderFactories::addFactory(string headerType, SipHeaderFactoryFuncPtr f){
	string ht;
	for (unsigned i=0; i< headerType.size();i++){
		ht+=toupper(headerType[i]);
	}
	
	factories[ht] = f;
}

SipHeaderFactoryFuncPtr SipHeaderFactories::getFactory(const string headerType) const{
	string ht;
	for (unsigned i=0; i< headerType.size();i++){
		ht+=toupper(headerType[i]);
	}

	std::map<std::string, SipHeaderFactoryFuncPtr >::const_iterator res;
	res = factories.find(ht);
	if( res != factories.end()) {
		return (*factories.find(ht)).second;
	}else{
		return NULL;
	}
}

SipHeader::SipHeader(MRef<SipHeaderValue*> val): headerName(val->headerName){
//	cerr << "Header name is "<< flush << headerName<< endl;
	type = val->getType();
	headerValues.push_back(val);
}

SipHeader::~SipHeader(){

}

string SipHeader::getString() const{
	string ret = headerName +": ";
	bool first=true;
	for (int i=0; i< headerValues.size(); i++){
		if (!first){
			ret += ",";
		}else{
			first=false;
		}
		ret+=headerValues[i]->getStringWithParameters();
	}
	return ret;
}

int32_t SipHeader::getType() const {
	return type;
}

int SipHeader::getNoValues() const {
	return headerValues.size();
}

MRef<SipHeaderValue *> SipHeader::getHeaderValue(int i) const {
	assert(i < headerValues.size() );
	return headerValues[i];
}

void SipHeader::addHeaderValue(MRef<SipHeaderValue*> v){
	massert(type == v->getType());
	headerValues.push_back(v);
}

void SipHeader::removeHeaderValue(int i){
	assert(i < headerValues.size() );
	headerValues.remove(i);
}

static string getHeader(const string &line,int &endi) {
	string ret;
	int i;
	for (i=0;   (i<(int)line.size()) && line[i]!=' ' && line[i]!='\t' && line[i]!=':'   ;  i++){
		ret+=line[i];
	}
	while (line[i]==' ' || line[i]=='\t' || line[i]==':'){
		i++;
	}
	endi=i;
	return ret;
}

string findHeaderType(string s){
	string ht;
	int ssize = (int)s.size();
	for (int i=0; s[i]!=':' && i<ssize; i++){
		ht+=s[i];
	}
	return trim(ht);
}

MRef<SipHeader *> SipHeader::parseHeader(const string &line){
	int hdrstart=0;
	MRef<SipHeader*> h;
	
	string hdr = getHeader(line,hdrstart);
 	//cerr<< endl << "PARSEHDR: Sip Header: header"+hdr<<endl;
	string valueline = line.substr(hdrstart);
	
 	//cerr << "PARSEHDR: hdr parsed to <"<< hdr <<">"<< endl;
 	//cerr << "PARSEHDR: valueline parsed to "<< valueline<< endl;

	string headerType = findHeaderType(line);
	string ht = upCase(headerType);
	char valueSeparator;
	char paramSeparator;

	if( ht == "WWW-AUTHENTICATE" ||
			ht == "PROXY-AUTHENTICATE" ||
			ht == "AUTHORIZATION" ||
			ht == "PROXY-AUTHORIZATION" ){
		valueSeparator = '\0';
		paramSeparator = ',';
	} else {
		valueSeparator = ',';
		paramSeparator = ';';
	}

	vector<string> values = split(valueline,true, valueSeparator);

	for (unsigned i=0; i< values.size(); i++){
		vector<string> value_params;
		string value_zero; //value zero is the non-parameter part of the header
		if( values[i].size() == 0 ) { //an empty value??? BUG!
			// 				cerr << "BUGBUGBUG: SipHeader::parseHeader : Found Empty header value in Sip message!:: " << valueline << endl;
			continue;
		}
		// 			cerr << "PARSER: First value+params line: "<< values[i]<<""<<endl;

		if( ht == "ACCEPT-CONTACT" ) {
			value_params = split(values[i],true,'\n');
			if (value_params.size()>0)
				value_zero = value_params[0];
			// 				cerr<<"valueline.substr(2): "+valueline.substr(2)<<endl;
		} else if( ht == "CONTACT" ) {
			size_t ltPos = values[i].find( '<' );
			size_t gtPos = values[i].find( '>' );
			if( ltPos!=string::npos && gtPos!=string::npos ) {
				//if the string contains '<' and '>' ...
				//remove them
				value_zero = values[i].substr( ltPos + 1, gtPos - ltPos - 1 );
				//now split the outside parameters ...
				value_params = split( 
						//note that it should be gtPos -1, but we need value_params[0] to be 
						//not a param ...so we take some junk from the uri previous to the first ;
						values[i].substr( gtPos - 1, values[i].size() - (gtPos - 1) ),
						true,
						';' );
			} else { //if there is no < or >, then just split into parameters ...
				value_params = split(values[i],true,';');
				if (value_params.size()>0)
					value_zero = value_params[0];
			}
		} else if( ht == "FROM" ||
				ht == "TO" ||
			   ht == "ROUTE" ||
			   ht == "RECORD-ROUTE" ) {
			size_t ltPos = values[i].find( '<' );
			size_t gtPos = values[i].find( '>' );
			if( ltPos!=string::npos && gtPos!=string::npos ) {
				value_zero = values[i].substr( 0, gtPos + 1 );
				//now split the outside parameters ...
				value_params = split( 
						//note that it should be gtPos -1, but we need value_params[0] to be 
						//not a param ...so we take some junk from the uri previous to the first ;
						values[i].substr( gtPos - 1, values[i].size() - (gtPos - 1) ),
						true,
						';' );
			} else { //if there is no < or >, then just split into parameters ...
				value_params = split(values[i],true,';');
				if (value_params.size()>0)
					value_zero = value_params[0];
			}
		} else {
			value_params = split(values[i],true,paramSeparator);
			if (value_params.size()>0)
				value_zero = value_params[0];
		}

//		cerr << "PARSER: Header type is: "<< headerType << endl;
//		cerr << "PARSER: Creating value from string: "<< value_zero <<endl;
		SipHeaderFactoryFuncPtr factory;
		factory = SipHeader::headerFactories.getFactory(headerType);
		MRef<SipHeaderValue *> hval;
		if (factory){
			hval = factory(value_zero);
		}else{
			hval = new SipHeaderValueUnknown(headerType, value_zero);
		}	

		for(unsigned j=1; j<value_params.size(); j++){
			hval->addParameter(new SipHeaderParameter(value_params[j]));
		}
		if (i==0){
			h= new SipHeader(hval);
		}else{
				h->addHeaderValue(hval);
			}
	}

	if (!h){
		//Special case for headers that are allowed to exist
		//without any header value. This list is compatibel 
		//with: RFC3261, RFC3262(none)
		if ( 		ht=="ACCEPT" || 
				ht=="ACCEPT-ENCODING" ||
				ht=="ACCEPT-LANGUAGE" ||
				ht=="ALLOW" ||
				ht=="ORGANIZATION" ||
				ht=="SUBJECT" ||
				ht=="SUPPORTED") {
			SipHeaderFactoryFuncPtr factory;
			factory = SipHeader::headerFactories.getFactory(headerType);
			MRef<SipHeaderValue *> hval;
			if (factory){
				hval = factory("");
			}else{
				hval = new SipHeaderValueUnknown(headerType, "");
			}	
			h= new SipHeader(hval);
		}
	}
	return h;
}



SipHeaderValue::SipHeaderValue(int t, const string &hname):headerName(hname),type(t){

}


void SipHeaderValue::setParameter(std::string key, std::string val){
	if (val.size()>0){
		MRef<SipHeaderParameter*> param = new SipHeaderParameter(key,val,true);
		addParameter(param);
	}else{
		removeParameter(key);
	}
}

void SipHeaderValue::addParameter(MRef<SipHeaderParameter*> p){
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

bool SipHeaderValue::hasParameter(const std::string &key) const {
	for (int i=0; i< parameters.size();i++){
		if (parameters[i]->getKey()==key){
			return true;
		}
	}
	return false;
}


std::string SipHeaderValue::getParameter(std::string key) const {
	for (int i=0; i< parameters.size();i++){
		if (parameters[i]->getKey()==key){
			return parameters[i]->getValue();
		}
	}
	return "";
}

void SipHeaderValue::removeParameter(std::string key){
	for (int i=0; i< parameters.size(); i++){
		if (parameters[i]->getKey()==key){
			parameters.remove(i);
			i=0;
		}
	}
}


std::string SipHeaderValue::getStringWithParameters() const{
	std::string parameterList;
	int nparam = parameters.size();
	for (int i=0; i< nparam; i++){
		if( i == 0 )
			parameterList+=getFirstParameterSeparator();
		else
			parameterList+=getParameterSeparator();
		parameterList+=parameters[i]->getString();
	}

	return getString()+parameterList;
}


