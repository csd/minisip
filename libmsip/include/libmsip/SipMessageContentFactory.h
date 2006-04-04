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



#ifndef _SIPMESSAGECONTENTFACTORY_H
#define _SIPMESSAGECONTENTFACTORY_H

#include<libmsip/libmsip_config.h>

#include<map>
#include<libmsip/SipMessageContent.h>
#include<string>
#include<libmutil/MemObject.h>

typedef MRef<SipMessageContent*>(*SipMessageContentFactoryFuncPtr)(const std::string & buf, const std::string & ContentType);

class LIBMSIP_API SMCFCollection{
public:
	void addFactory(std::string contentType, SipMessageContentFactoryFuncPtr);
	SipMessageContentFactoryFuncPtr getFactory(const std::string contentType);
	
private:
	std::map<std::string, SipMessageContentFactoryFuncPtr > factories; // map with content type as key
};

#endif
