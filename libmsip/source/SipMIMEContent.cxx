
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

#include<libmsip/SipMessageContent.h>


SipMimeContent::SipMimeContent(std::string ContentType, std::string Message, std::string boundry) {
	this->Message = Message; 
	this->ContentType = ContentType;
	this->boundry = boundry;
}

std::string SipMimeContent::getString() {
	if(ContentType.substr(0,8) == "multipart"){
		std::list <MRef<SipMessageContent*> >::iterator iter;
		std::string mes = Message + "\r\n";
		if(parts.empty())
			mes ="--" + boundry + "\r\n";
		else
			for( iter = parts.begin(); iter != parts.end()  ; iter++ ){
				mes = mes + "--" + boundry + "\r\n";
				mes = mes + "Content-type: " + (*iter)->getContentType() + "\r\n";
				mes = mes + (*iter)->getString() + "\r\n";
			}
		mes = mes + "--" + boundry + "--" + "\r\n";
		return mes;
	}
	else
		return Message;
}

std::string SipMimeContent::getContentType() {
	if(ContentType.substr(0,8) == "multipart")
		return ContentType +"; boundary=" + boundry;
	else
		return ContentType;
}
		
void SipMimeContent::addPart(MRef<SipMessageContent*> part){
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
	




