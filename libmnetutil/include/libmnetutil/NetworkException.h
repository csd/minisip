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

#ifdef _MSC_VER
#ifdef LIBMNETUTIL_EXPORTS
#define LIBMNETUTIL_API __declspec(dllexport)
#else
#define LIBMNETUTIL_API __declspec(dllimport)
#endif
#else
#define LIBMNETUTIL_API
#endif

//#include<config.h>

#include<string.h>

#ifndef NO_SECURITY
#include<openssl/ssl.h>
#endif

#include<string>

class LIBMNETUTIL_API NetworkException{
	public:
		virtual ~NetworkException(){}
		virtual std::string errorDescription();
	protected:
		NetworkException();
		NetworkException( int errorNumber );
		int errorNumber;
};

class LIBMNETUTIL_API HostNotFound : public NetworkException{
	public:
		HostNotFound( std::string host ):host(host){};
		virtual std::string errorDescription(){ return
			"Host "+host+" not found.";
		}
	private:
		std::string host;
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

#ifndef NO_SECURITY
class LIBMNETUTIL_API TLSConnectFailed : public ConnectFailed{
	public:
		TLSConnectFailed( int errorNumber, SSL * ssl  );
		virtual std::string errorDescription();

	private:
		SSL * ssl;
};

class LIBMNETUTIL_API TLSInitFailed : public NetworkException{
	public:
		TLSInitFailed();
		virtual std::string errorDescription(){return
			"TLS initialization failed.";
		};
};

class LIBMNETUTIL_API TLSContextInitFailed : public NetworkException{
	public:
		TLSContextInitFailed();
		virtual std::string errorDescription(){return
			"TLS context initialization failed.";
		};
};
#endif //NO_SECURITY


	
#endif
