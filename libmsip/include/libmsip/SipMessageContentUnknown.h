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


#ifndef _SIPMESSAGECONTENTUNKNOWN_H
#define _SIPMESSAGECONTENTUNKNOWN_H

#include<libmsip/libmsip_config.h>

#include<libmsip/SipMessageContent.h>
#include<iostream>

MRef<SipMessageContent*> LIBMSIP_API sipUnknownMessageContentFactory(const std::string &, const std::string &ContentType);

class LIBMSIP_API SipMessageContentUnknown : public SipMessageContent{
public:
	SipMessageContentUnknown(std::string m, std::string contentType);
	
	virtual std::string getMemObjectType() const {return "SipMessageContentUnknown";}
	
	virtual std::string getString() const;

	virtual std::string getContentType() const;
private:
	std::string msg;
	std::string contentType;
};

#endif
