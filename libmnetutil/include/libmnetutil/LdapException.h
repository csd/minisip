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

#ifndef LDAPEXCEPTION_H_
#define LDAPEXCEPTION_H_

#include<libmnetutil/libmnetutil_config.h>

#include <libmutil/Exception.h>
#include <libmnetutil/libmnetutil_config.h>
#include <string>

/**
 * @author	Mikael Svensson
 * @author	Erik Eliasson <eliasson@it.kth.se>
 * @author	Johan Bilien <jobi@via.ecp.fr>
 */
class LIBMNETUTIL_API LdapException : public Exception {
	public:
		LdapException();
		LdapException(std::string message);
		virtual ~LdapException() throw() {};
		virtual const char *what() const throw();
	protected:
		std::string msg;
};

class LIBMNETUTIL_API LdapAttributeNotFoundException : public LdapException {
	public:
		LdapAttributeNotFoundException(std::string message);
};

class LIBMNETUTIL_API LdapNotConnectedException : public LdapException {
	public:
		LdapNotConnectedException();
};

class LIBMNETUTIL_API LdapUnsupportedException : public LdapException {
	public:
		LdapUnsupportedException(std::string feature);
};

#endif
