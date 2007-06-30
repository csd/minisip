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

#include <config.h>
#include <libmnetutil/LdapException.h>

/**
 * @author	Mikael Svensson
 * @author	Erik Eliasson <eliasson@it.kth.se>
 * @author	Johan Bilien <jobi@via.ecp.fr>
 */

LdapException::LdapException() {
	msg = "LdapException";
};

LdapException::LdapException(std::string msg) {
	this->msg = "LdapException: " + msg;
};

const char* LdapException::what()const throw(){
	return msg.c_str();
}

LdapAttributeNotFoundException::LdapAttributeNotFoundException(std::string message) {
	msg = "LdapAttributeNotFoundException: Could not find attribute " + message;
}
LdapNotConnectedException::LdapNotConnectedException() {
	msg = "LdapNotConnectedException: Not connected to server";
}
LdapUnsupportedException::LdapUnsupportedException(std::string feature) {
	msg = "LdapUnsupportedException: Feature " + feature + " not supported";
}
