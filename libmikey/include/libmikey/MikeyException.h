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

/**
 * Base class for all exceptions that may occur in the MIKEY implementation.
 * @author Erik Eliasson, Johan Bilien
 * @version 0.01
 */

#include<libmikey/MikeyMessage.h>



class MikeyException{
	public:
		/**
		 * @param All exceptions MUST have a std::string describing the
		 * 	exception that is suitable to present to the user.
		 */
		MikeyException(std::string message);
		virtual ~MikeyException(){};

		/**
		 * @returns std::string describing the exception.
		 */
		virtual std::string message();
		
	private:
		std::string messageValue;

};


class MikeyExceptionUninitialized: public MikeyException{
	public:
		MikeyExceptionUninitialized(std::string msg);
		virtual ~MikeyExceptionUninitialized();

};



class MikeyExceptionMessageContent: public MikeyException{
	public:
		MikeyExceptionMessageContent(std::string msg);
		MikeyExceptionMessageContent(MikeyMessage * errMsg, std::string msg="");
		virtual ~MikeyExceptionMessageContent();

		MikeyMessage * errorMessage();
	private:
		MikeyMessage * errorMessageValue;

};



class MikeyExceptionMessageLengthException: public MikeyException{
	public:
		MikeyExceptionMessageLengthException(std::string msg);
		virtual ~MikeyExceptionMessageLengthException();

};


class MikeyExceptionNullPointerException : public MikeyException{
	public:
		MikeyExceptionNullPointerException(std::string msg);
		virtual ~MikeyExceptionNullPointerException();

};


class MikeyExceptionAuthentication : public MikeyException{
	public:
		MikeyExceptionAuthentication(std::string msg);
		virtual ~MikeyExceptionAuthentication();

};

class MikeyExceptionUnacceptable : public MikeyException{
	public:
		MikeyExceptionUnacceptable(std::string msg);
		virtual ~MikeyExceptionUnacceptable();

};

class MikeyExceptionUnimplemented : public MikeyException{
	public:
		MikeyExceptionUnimplemented(std::string msg);
		virtual ~MikeyExceptionUnimplemented();

};
#endif
