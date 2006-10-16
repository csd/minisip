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
 * 	SipHeaderSubject.h
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * 	Mikael Magnusson, mikma@users.sourceforge.net
 * Purpose
 * 
*/

#ifndef SIPHEADERUNKNOWN_H
#define SIPHEADERUNKNOWN_H

#include<libmsip/libmsip_config.h>

#include<libmsip/SipHeaderString.h>

/**
 * libmsip handles SIP headers that are not known to it by placing
 * the entire header in a SipHeaderValueUnknown. This is
 * the only header where the "header value" also will contain
 * the header "name" (the identifier starting the header line before 
 * the colon). The "getString" method (inherited from 
 * SipHeaderValueString) will therefore return the entire header.
 * 
 * @author Erik Eliasson
*/
class LIBMSIP_API SipHeaderValueUnknown: public SipHeaderValueString{
	public:
		SipHeaderValueUnknown(const std::string &headerName, const std::string &build_from);

                virtual std::string getMemObjectType() const {return "SipHeaderUnknown";}
};

#endif
