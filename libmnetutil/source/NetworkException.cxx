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

#ifdef HAVE_CONFIG_H
#include<config.h>
#endif


#include<libmnetutil/NetworkException.h>

using namespace std;

ConnectFailed::ConnectFailed( int errorNumber ):NetworkException(errorNumber){};
SendFailed::SendFailed( int errorNumber ):NetworkException(errorNumber){};
ResolvError::ResolvError( int errorNumber ):NetworkException(errorNumber){};
SocketFailed::SocketFailed( int errorNumber ):NetworkException(errorNumber){};
ListenFailed::ListenFailed( int errorNumber ):NetworkException(errorNumber){};
BindFailed::BindFailed( int errorNumber ):NetworkException(errorNumber){};
GetSockNameFailed::GetSockNameFailed( int errorNumber ):NetworkException(errorNumber){};
SetSockOptFailed::SetSockOptFailed( int errorNumber ):NetworkException(errorNumber){};
NetworkException::NetworkException( int errorNumber ):errorNumber(errorNumber){};
NetworkException::NetworkException():errorNumber(0){};

string NetworkException::errorDescription(){
	char buf[256];

#ifdef LINUX
	strerror_r( errorNumber, buf, 256 );
	return string( (const char *)buf );
#endif
#ifdef WIN32
	return string( strerror( errorNumber ));
#endif
}

#ifndef NO_SECURITY

TLSInitFailed::TLSInitFailed():NetworkException(){
}

TLSContextInitFailed::TLSContextInitFailed():NetworkException(){
}

TLSConnectFailed::TLSConnectFailed( int errorNumber, SSL * ssl ):ConnectFailed(errorNumber),ssl(ssl){};

string TLSConnectFailed::errorDescription(){
	switch( SSL_get_error( ssl, errorNumber ) ){
		case SSL_ERROR_NONE:
			return string( "SSL Error: No error" );
		case SSL_ERROR_ZERO_RETURN:
			return string( "SSL Error: Connection was closed" );
		case SSL_ERROR_WANT_READ:
			return string( "SSL Error: Could not perform the read opearation on the underlying TCP connection" );
		case SSL_ERROR_WANT_WRITE:
			return string( "SSL Error: Could not perform the write opearation on the underlying TCP connection" );
		case SSL_ERROR_WANT_CONNECT:
			return string( "SSL Error: The underlying TCP connection is not connected" );
		case SSL_ERROR_WANT_ACCEPT:
			return string( "SSL Error: The underlying TCP connection is not accepted" );
		case SSL_ERROR_WANT_X509_LOOKUP:
			return string( "SSL Error: Error in the X509 lookup" );
		case SSL_ERROR_SYSCALL:
			return string( "SSL Error: I/O error" );
		case SSL_ERROR_SSL:
			return string( "SSL Error: Error in the SSL protocol" );
	}
	return "";
}
#endif




