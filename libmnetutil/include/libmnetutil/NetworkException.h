/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/* Copyright (C) 2004 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#ifndef NETWORK_EXCEPTION_H
#define NETWORK_EXCEPTION_H

//#include<config.h>

#include<string.h>

#ifndef NO_SECURITY
#include<openssl/ssl.h>
#endif

#include<string>

class NetworkException{
	public:
		virtual ~NetworkException(){}
		virtual std::string errorDescription();
	protected:
		NetworkException();
		NetworkException( int errorNumber );
		int errorNumber;
};

class HostNotFound : public NetworkException{
	public:
		HostNotFound( std::string host ):host(host){};
		virtual std::string errorDescription(){ return
			"Host "+host+" not found.";
		}
	private:
		std::string host;
};

class ResolvError : public NetworkException{
	public:
		ResolvError( int errorNumber );
};

class ConnectFailed : public NetworkException{
	public:
		ConnectFailed( int errorNumber );
};

class SocketFailed : public NetworkException{
	public:
		SocketFailed( int errorNumber );
};

class BindFailed : public NetworkException{
	public:
		BindFailed( int errorNumber );
};

class SendFailed : public NetworkException{
	public:
		SendFailed( int errorNumber );
};

class SetSockOptFailed : public NetworkException{
	public:
		SetSockOptFailed( int errorNumber );
};

class ListenFailed : public NetworkException{
	public:
		ListenFailed( int errorNumber );
};

class GetSockNameFailed : public NetworkException{
	public:
		GetSockNameFailed( int errorNumber );
};

#ifndef NO_SECURITY
class TLSConnectFailed : public ConnectFailed{
	public:
		TLSConnectFailed( int errorNumber, SSL * ssl  );
		virtual std::string errorDescription();

	private:
		SSL * ssl;
};

class TLSInitFailed : public NetworkException{
	public:
		TLSInitFailed();
		virtual std::string errorDescription(){return
			"TLS initialization failed.";
		};
};

class TLSContextInitFailed : public NetworkException{
	public:
		TLSContextInitFailed();
		virtual std::string errorDescription(){return
			"TLS context initialization failed.";
		};
};
#endif //NO_SECURITY


	
#endif
