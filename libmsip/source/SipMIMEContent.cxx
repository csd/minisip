
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

#include<libmsip/SipMIMEContent.h>
#include<libmsip/SipMessage.h>
#include<libmsip/SipMessageContentFactory.h>
#include <iostream>

MRef<SipMessageContent*> SipMIMEContentFactory(const std::string & buf, const std::string & ContentType) {
	return new SipMimeContent(buf, ContentType);
}

SipMimeContent::SipMimeContent(std::string ContentType){
	this->ContentType = ContentType;
	this->boundry = "boun=_dry";
	this->Message = "";
	this->uniqueboundry = "_Minisip";
}

SipMimeContent::SipMimeContent(std::string ContentType, std::string Message, std::string boundry) {
	this->Message = Message; 
	this->ContentType = ContentType;
	this->boundry = boundry;
	this->uniqueboundry = "_Minisip";
}

SipMimeContent::SipMimeContent(std::string content, std::string ContentType) {
	int index2;
	std::string cont;
	this->uniqueboundry = "_Minisip";
	if(ContentType.substr(0,9) == "multipart"){
		this->ContentType = ContentType.substr(0 , ContentType.find("; ",0) );
		index2 = ContentType.find("; boundary=",0);
		assert(index2 != string::npos);
		this->boundry = ContentType.substr(index2 + 11 , ContentType.find(";",index2 + 11));
		// Find first bodypart
		index2 = content.find("--"+this->boundry, 0);
		assert(index2 != string::npos);
		// Extract preamble if any
		if(index2 > 0)
			this->Message = content.substr(0, index2 - 5);
		else
			this->Message = "";
		// Extract the bodyparts
		int boundrysize = 2 + this->boundry.length();
		// Find end of body
		int endindex = content.rfind("--"+this->boundry+"--", content.length());
		int index1 = index2;
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
				merr << "WARNING: No SipMessageContentFactory found for content type "<<cont <<end;
			//End of one bodypart becomes beginning of the next
			index1 = index2 + 5;	
		}
	}
	else{
		this->ContentType = ContentType;
		this->Message = content;
		this->boundry = "";
	}
}

std::string SipMimeContent::getString() {
	if(ContentType.substr(0,9) == "multipart"){
		std::list <MRef<SipMessageContent*> >::iterator iter;
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

std::string SipMimeContent::getContentType() {
	if(ContentType.substr(0,9) == "multipart")
		return ContentType +"; boundary=" + boundry;
	else
		return ContentType;
}

void SipMimeContent::setBoundry(std::string boundry){
	this->boundry = boundry;
}

std::string SipMimeContent::getBoundry(){
	return boundry;
}
	
void SipMimeContent::addPart(MRef<SipMessageContent*> part){
	if( (part->getContentType()).substr(0,9) == "multipart")
		if(((SipMimeContent*)*part)->getBoundry() == boundry){
			((SipMimeContent*)*part)->setBoundry(boundry + uniqueboundry);
			uniqueboundry = uniqueboundry + "_Rules";
		}
	parts.push_back(part);
}

MRef<SipMessageContent*> SipMimeContent::popFirstPart() {
	if(!parts.empty()){
		MRef<SipMessageContent*> part = parts.front(); 
		parts.pop_front();
		return part;
	}
	else 
		return NULL;
}
	




