
/*
  Copyright (C) 2005, 2004 Erik Eliasson, Johan Bilien, Joachim Orrblad
  
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
 * Authors: Joachim Orrblad <joachim[at]orrblad.com>
 *          
*/

#include<config.h>

#include<libmsip/SipMessageContentMime.h>
#include<libmsip/SipMessage.h>
#include<libmsip/SipMessageContentFactory.h>
#include<libmutil/massert.h>
#include <iostream>

using namespace std;

MRef<SipMessageContent*> SipMIMEContentFactory(const std::string & buf, const std::string & ContentType) {
	return new SipMessageContentMime(buf, ContentType);
}

SipMessageContentMime::SipMessageContentMime(std::string t){
	this->ContentType = t;
	this->boundry = "boun=_dry";
	this->Message = "";
	this->uniqueboundry = "_Minisip";
}

SipMessageContentMime::SipMessageContentMime(std::string t, std::string m, std::string b) {
	this->Message = m; 
	this->ContentType = t;
	this->boundry = b;
	this->uniqueboundry = "_Minisip";
}

SipMessageContentMime::SipMessageContentMime(std::string content, std::string t) {
	size_t index2;
	std::string cont;
	this->uniqueboundry = "_Minisip";
	if(t.substr(0,9) == "multipart"){
		this->ContentType = t.substr(0 , ContentType.find("; ",0) );
		index2 = t.find("; boundary=",0);
		massert(index2 != string::npos);
		this->boundry = t.substr(index2 + 11 , t.find(";",index2 + 11));
		// Find first bodypart
		index2 = content.find("--"+this->boundry, 0);
		massert(index2 != string::npos);
		// Extract preamble if any
		if(index2 > 0)
			this->Message = content.substr(0, index2 - 5);
		else
			this->Message = "";
		// Extract the bodyparts
		size_t boundrysize = 2 + this->boundry.length();
		// Find end of body
		size_t endindex = content.rfind("--"+this->boundry+"--", content.length());
		size_t index1 = index2;
		while (endindex != index1){
			index1 = index1 + boundrysize + 2;
			if (content.substr(index1,2) == "\r\n"){
				cont = "text/plain; charset=us-ascii";
				index1 = index1 + 2;
			}
			else {
				if (content.substr(index1,14) == "Content-type: ")
					cont = content.substr(index1+14, content.find("\r\n\r\n", index1 + 14) - index1 - 14);
				else{
					cont = "";
					cerr <<  "Absence of Content-type in MIMEContent.cxx" << endl;
				}
			}
			// Find the end of the bodypart
			
			index2 = content.find("--"+this->boundry, index1) - 5;
			index1 = content.find("\r\n\r\n", index1 + 14) + 4;		
			SipMessageContentFactoryFuncPtr contentFactory = SipMessage::contentFactories.getFactory( cont);
			if (contentFactory)
				addPart(contentFactory(content.substr(index1,index2-index1+1), cont));
			else //TODO: Better error handling
				merr << "WARNING: No SipMessageContentFactory found for content type "<<cont <<endl;
			//End of one bodypart becomes beginning of the next
			index1 = index2 + 5;	
		}
	}
	else{
		this->ContentType = t;
		this->Message = content;
		this->boundry = "";
	}
}

std::string SipMessageContentMime::getString() const{
	if(ContentType.substr(0,9) == "multipart"){
		std::list <MRef<SipMessageContent*> >::const_iterator iter;
		std::string mes;
		if(Message != "")
			mes = Message + "\r\n\r\n";
		if(parts.empty())
			mes ="--" + boundry + "\r\n\r\n";
		else
			for( iter = parts.begin(); iter != parts.end()  ; iter++ ){
				mes = mes + "--" + boundry + "\r\n";
				mes = mes + "Content-type: " + (*iter)->getContentType() + "\r\n\r\n";
				mes = mes + (*iter)->getString() + "\r\n\r\n";
			}
		mes = mes + "--" + boundry + "--" + "\r\n";
		return mes;
	}
	else{
		return Message;
	}
}

std::string SipMessageContentMime::getContentType() const{
	if(ContentType.substr(0,9) == "multipart")
		return ContentType +"; boundary=" + boundry;
	else
		return ContentType;
}

void SipMessageContentMime::setBoundry(std::string b){
	this->boundry = b;
}

std::string SipMessageContentMime::getBoundry(){
	return boundry;
}
	
void SipMessageContentMime::addPart(MRef<SipMessageContent*> part){
	if( (part->getContentType()).substr(0,9) == "multipart")
		if(((SipMessageContentMime*)*part)->getBoundry() == boundry){
			((SipMessageContentMime*)*part)->setBoundry(boundry + uniqueboundry);
			uniqueboundry = uniqueboundry + "_Rules";
		}
	parts.push_back(part);
}

MRef<SipMessageContent*> SipMessageContentMime::popFirstPart() {
	if(!parts.empty()){
		MRef<SipMessageContent*> part = parts.front(); 
		parts.pop_front();
		return part;
	}
	else 
		return NULL;
}
	




