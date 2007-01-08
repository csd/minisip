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


#ifndef MIKEYEXCEPTION_H
#define MIKEYEXCEPTION_H

#include<libmikey/libmikey_config.h>
#include<libmikey/MikeyMessage.h>
#include<libmutil/MemObject.h>


/**
 * Base class for all exceptions that may occur in the MIKEY implementation.
 * @author Erik Eliasson, Johan Bilien
 * @version 0.01
 */

#include<libmutil/Exception.h>

class MikeyMessage;


class LIBMIKEY_API MikeyException : public Exception{
	public:
		/**
		 * @param All exceptions MUST have a std::string describing the
		 * 	exception that is suitable to present to the user.
		 */
		MikeyException(const char* message);
		virtual ~MikeyException()throw (){};
};


class LIBMIKEY_API MikeyExceptionUninitialized: public MikeyException{
	public:
		MikeyExceptionUninitialized(const char* msg);
		virtual ~MikeyExceptionUninitialized() throw();

};



class LIBMIKEY_API MikeyExceptionMessageContent: public MikeyException{
	public:
		MikeyExceptionMessageContent(const char* msg);
		MikeyExceptionMessageContent(MRef<MikeyMessage *> errMsg, const char* msg="");
		virtual ~MikeyExceptionMessageContent()throw();

		MRef<MikeyMessage *> errorMessage();
	private:
		MRef<MikeyMessage *> errorMessageValue;

};



class LIBMIKEY_API MikeyExceptionMessageLengthException: public MikeyException{
	public:
		MikeyExceptionMessageLengthException(const char* msg);
		virtual ~MikeyExceptionMessageLengthException() throw();

};


class LIBMIKEY_API MikeyExceptionNullPointerException : public MikeyException{
	public:
		MikeyExceptionNullPointerException(const char* msg);
		virtual ~MikeyExceptionNullPointerException() throw();

};


class LIBMIKEY_API MikeyExceptionAuthentication : public MikeyException{
	public:
		MikeyExceptionAuthentication(const char* msg);
		virtual ~MikeyExceptionAuthentication() throw();

};

class LIBMIKEY_API MikeyExceptionUnacceptable : public MikeyException{
	public:
		MikeyExceptionUnacceptable(const char* msg);
		virtual ~MikeyExceptionUnacceptable() throw();

};

class LIBMIKEY_API MikeyExceptionUnimplemented : public MikeyException{
	public:
		MikeyExceptionUnimplemented(const char* msg);
		virtual ~MikeyExceptionUnimplemented() throw();

};
#endif
