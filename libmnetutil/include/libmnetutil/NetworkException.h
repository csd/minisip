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

#ifndef NETWORK_EXCEPTION_H
#define NETWORK_EXCEPTION_H

#include<libmnetutil/libmnetutil_config.h>

#include<string.h>

#include<string>
#include<libmutil/Exception.h>

class LIBMNETUTIL_API NetworkException : public Exception{
	public:
		NetworkException( );
		NetworkException( int errorNumber );
		virtual ~NetworkException() throw() {};
		virtual const char *what() const throw();
	protected:
		int errorNumber;
		std::string msg;
};

class LIBMNETUTIL_API HostNotFound : public NetworkException{
	public:
		HostNotFound( std::string host_ ):NetworkException(-1),host(host_){};
		virtual ~HostNotFound()throw() {}
		virtual const char*what();
	private:
		std::string host;
		std::string msg;
};
class LIBMNETUTIL_API UnknownAddressFamily : public NetworkException{
public:
	UnknownAddressFamily( int errorNumber );
	virtual const char*what();
};

class LIBMNETUTIL_API ResolvError : public NetworkException{
	public:
		ResolvError( int errorNumber );
};

class LIBMNETUTIL_API ConnectFailed : public NetworkException{
	public:
		ConnectFailed( int errorNumber );
};

class LIBMNETUTIL_API SocketFailed : public NetworkException{
	public:
		SocketFailed( int errorNumber );
};

class LIBMNETUTIL_API BindFailed : public NetworkException{
	public:
		BindFailed( int errorNumber );
};

class LIBMNETUTIL_API SendFailed : public NetworkException{
	public:
		SendFailed( int errorNumber );
};

class LIBMNETUTIL_API SetSockOptFailed : public NetworkException{
	public:
		SetSockOptFailed( int errorNumber );
};

class LIBMNETUTIL_API ListenFailed : public NetworkException{
	public:
		ListenFailed( int errorNumber );
};

class LIBMNETUTIL_API GetSockNameFailed : public NetworkException{
	public:
		GetSockNameFailed( int errorNumber );
};
#endif

