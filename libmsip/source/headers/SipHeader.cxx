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
#include<libmsip/SipHeaderUnsupported.h>
#include<libmsip/SipHeaderWarning.h>

#include<libmutil/split_in_lines.h>
#include<libmutil/trim.h>

#include<vector>

SipHeaderFactories SipHeader::headerFactories=SipHeaderFactories();


SipHeaderParameter::SipHeaderParameter(string parseFrom){
	vector<string> key_val = split(parseFrom,true,'=');
	key = key_val[0];
	hasEqual = false;
	if (key_val.size()==2){
		value = key_val[1];
		hasEqual=true;
	}
	
}

SipHeaderParameter::SipHeaderParameter(string k, string val, bool equalSign):key(k),value(val),hasEqual(equalSign){
	
}



string SipHeaderParameter::getString(){
	if (hasEqual || value.size()>0){
		return key+"="+value;
	}else{
		return key;
	}
}


void SipHeaderFactories::addFactory(string headerType, SipHeaderFactoryFuncPtr f){
	string ht;
	for (int i=0; i< headerType.size();i++){
		ht+=toupper(headerType[i]);
	}
	
	factories[ht] = f;
}

SipHeaderFactoryFuncPtr SipHeaderFactories::getFactory(const string headerType){
	string ht;
	for (int i=0; i< headerType.size();i++){
		ht+=toupper(headerType[i]);
	}
	return factories[ht];
}


SipHeader::SipHeader(MRef<SipHeaderValue*> val): headerName(val->headerName){
//	cerr << "Header name is "<< flush << headerName<< endl;
	type = val->getType();
	headerValues.push_back(val);
}

SipHeader::~SipHeader(){

}

string SipHeader::getString(){
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

void SipHeader::addHeaderValue(MRef<SipHeaderValue*> v){
	headerValues.push_back(v);
}

static string getHeader(const string &line,int &endi){
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
	int ssize = s.size();
	for (int i=0; s[i]!=':' && i<ssize; i++){
		ht+=s[i];
	}
	return trim(ht);
}

MRef<SipHeader *> SipHeader::parseHeader(const string &line){
	int hdrstart=0;
	string hdr = getHeader(line,hdrstart);
	string valueline = line.substr(hdrstart);
	
//	cerr << "hdr parsed to <"<< hdr << ">"<< endl;
//	cerr << "valueline parsed to <"<< valueline<<">"<< endl;
	
	vector<string> values = split(valueline,true, ',');

	string headerType = findHeaderType(line);
	
	MRef<SipHeader*> h;

	for (int i=0; i< values.size(); i++){
		vector<string> value_params = split(valueline,true,';');
		//cerr << "Header type is <"<< headerType << ">"<< endl;
		//cerr << "Creating value from string <"<< value_params[0]<<">"<<endl;
		SipHeaderFactoryFuncPtr factory;
		factory = SipHeader::headerFactories.getFactory(headerType);
		MRef<SipHeaderValue *> hval;
		if (factory){
			hval = factory(value_params[0]);
		}else{
			hval = new SipHeaderValueUnsupported(value_params[0]);
		}	
		
		for(int j=1; j<value_params.size(); j++){
			//cerr << "Adding parameter <"<< value_params[j]<< ">"<< endl;
			hval->addParameter(new SipHeaderParameter(value_params[j]));
		}
		if (i==0){
			h= new SipHeader(hval);
		}else{
			h->addHeaderValue(hval);
		}
	}
	return h;

}



SipHeaderValue::SipHeaderValue(int t, const string &hname):headerName(hname),type(t){

}

