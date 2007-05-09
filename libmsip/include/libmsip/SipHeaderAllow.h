/*
  Copyright (C) 2004-2007 Erik Eliasson, Johan Bilien, Mikael Magnusson
  
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

/* Name
 * 	SipHeaderAllow.h
 * Authors
 * 	Erik Eliasson, eliasson@it.kth.se
*/

#ifndef SIPHEADERALLOW_H
#define SIPHEADERALLOW_H

#include<libmsip/libmsip_config.h>

#include<libmsip/SipHeader.h>
#include<libmsip/SipHeaderString.h>

/**
 * @author Erik Eliasson
*/

extern SipHeaderFactoryFuncPtr sipHeaderAllowFactory;

class LIBMSIP_API SipHeaderValueAllow: public SipHeaderValueString{
	public:
		SipHeaderValueAllow(const std::string &build_from);

                virtual std::string getMemObjectType() const {return "SipHeaderAllow";}
		
		// string getString(); is inherited from
		// SipHeaderValueString that returns the string
		// passed to the constructor
};

#endif
