/*
  Copyright (C) 2006 Erik Eliasson
  
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
*/


#include<config.h>

#include<libmsip/SipHeaderSupported.h>

using namespace std;

MRef<SipHeaderValue *> supportedFactory(const string &build_from){
	                return new SipHeaderValueSupported(build_from);
}

SipHeaderFactoryFuncPtr sipHeaderSupportedFactory=supportedFactory;

const string SipHeaderValueSupportedTypeStr = "Supported";

SipHeaderValueSupported::SipHeaderValueSupported(const string &build_from)
		: SipHeaderValueString(SIP_HEADER_TYPE_SUPPORTED,SipHeaderValueSupportedTypeStr, build_from)
{
}

