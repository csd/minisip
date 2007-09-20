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


#include<config.h>

#include<libmnetutil/NetworkException.h>

#ifdef _WIN32_WCE
#include<wcecompat/string.h>
#endif

using namespace std;

ConnectFailed::ConnectFailed( int errorNr ):NetworkException(errorNr){};

UnknownAddressFamily::UnknownAddressFamily( int errorNr ) : NetworkException(errorNr) { }

SendFailed::SendFailed( int errorNr ):NetworkException(errorNr){};

ResolvError::ResolvError( int errorNr ):NetworkException(errorNr){};

SocketFailed::SocketFailed( int errorNr ):NetworkException(errorNr){};

ListenFailed::ListenFailed( int errorNr ):NetworkException(errorNr){};

BindFailed::BindFailed( int errorNr ):NetworkException(errorNr){};

GetSockNameFailed::GetSockNameFailed( int errorNr ):NetworkException(errorNr){};

SetSockOptFailed::SetSockOptFailed( int errorNr ):NetworkException(errorNr){};

NetworkException::NetworkException( int errorNr ):errorNumber(errorNr){
	#ifdef WIN32
		msg = string( strerror( errorNumber ));
	#else
		char buf[256];
		buf[0]=0;
		#if defined(GNU)
		int ret = strerror_r( errorNumber, buf, sizeof( buf ) );
		msg = string( buf );

		#else
		if (strerror_r( errorNumber, buf, sizeof( buf ) ))
			msg="Unknown error";
		else
			msg = string( buf );
		#endif
	#endif
};

NetworkException::NetworkException():errorNumber(0){
	msg="NetworkException";
};

const char* NetworkException::what()const throw(){
	return msg.c_str();
}


const char* HostNotFound::what() {
	msg = "Host "+host+" not found.";
	return msg.c_str();
}


const char* UnknownAddressFamily::what() {
	msg = "Unknown address family: " + errorNumber;
	return msg.c_str();
}
