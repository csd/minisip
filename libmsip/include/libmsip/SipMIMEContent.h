
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
#ifndef _SIPMIMECONTENT_H
#define _SIPMIMECONTENT_H

#include<libmsip/SipMessageContent.h>
#include<libmutil/MemObject.h>
#include<string.h>

MRef<SipMessageContent*> SipMIMEContentFactory(const std::string & buf, const std::string & ContentType);

class SipMimeContent : public SipMessageContent{
	public:
		SipMimeContent(std::string ContentType);
		SipMimeContent(std::string ContentType, std::string Message, std::string boundry);
		SipMimeContent(std::string content, std::string ContentTyp);
		virtual std::string getString();
		virtual std::string getContentType();
		virtual std::string getMemObjectType(){return "MSipMimeContent";}
		void addPart(MRef<SipMessageContent*> part);
		MRef<SipMessageContent*> popFirstPart();
		void setBoundry(std::string boundry);
		std::string getBoundry();
	private:
		std::string Message;
		std::string ContentType;
		std::string boundry;
		std::string uniqueboundry;
		std::list <MRef<SipMessageContent*> > parts;
};

#endif


