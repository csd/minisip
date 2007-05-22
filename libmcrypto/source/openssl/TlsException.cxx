/*
  Copyright (C) 2005, 2004 Erik Eliasson, Johan Bilien
  Copyright (C) 2006 Mikael Magnusson
  
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
 *          Mikael Magnusson <mikma@users.sourceforge.net>
*/


#include<config.h>

#include<libmcrypto/openssl/TlsException.h>

using namespace std;


TLSConnectFailed::TLSConnectFailed( int errorNumber_, SSL * ssl_ ):ConnectFailed(errorNumber_),ssl(ssl_){};

const char *TLSConnectFailed::what(){
	
	switch( SSL_get_error( ssl, errorNumber ) ){
		case SSL_ERROR_NONE:
			msg = "SSL Error: No error"; break;
		case SSL_ERROR_ZERO_RETURN:
			msg = "SSL Error: Connection was closed"; break;
		case SSL_ERROR_WANT_READ:
			msg = "SSL Error: Could not perform the read opearation on the underlying TCP connection" ; break;
		case SSL_ERROR_WANT_WRITE:
			msg = "SSL Error: Could not perform the write opearation on the underlying TCP connection"; break;
		case SSL_ERROR_WANT_CONNECT:
			msg = "SSL Error: The underlying TCP connection is not connected" ; break;
#ifdef SSL_ERROR_WANT_ACCEPT
		case SSL_ERROR_WANT_ACCEPT:
			msg = "SSL Error: The underlying TCP connection is not accepted" ; break;
#endif
		case SSL_ERROR_WANT_X509_LOOKUP:
			msg = "SSL Error: Error in the X509 lookup" ; break;
		case SSL_ERROR_SYSCALL:
			msg = "SSL Error: I/O error" ; break;
		case SSL_ERROR_SSL:
			msg = "SSL Error: Error in the SSL protocol" ; break;
		default:
			msg = "SSL Error: Unknown SSL error";
	}
	return msg.c_str();
}
