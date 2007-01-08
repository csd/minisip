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
#include<libmikey/MikeyDefs.h>
#include<libmikey/MikeyException.h>

MikeyException::MikeyException(const char *message): Exception(message) {
}


MikeyExceptionUninitialized::MikeyExceptionUninitialized(const char* msg): MikeyException(msg){

}

MikeyExceptionUninitialized::~MikeyExceptionUninitialized()throw(){

}


MikeyExceptionNullPointerException::MikeyExceptionNullPointerException(const char *msg): MikeyException(msg){

}


MikeyExceptionNullPointerException::~MikeyExceptionNullPointerException()throw(){

}



MikeyExceptionMessageContent::MikeyExceptionMessageContent(const char* msg): MikeyException(msg){

}

MikeyExceptionMessageContent::MikeyExceptionMessageContent(MRef<MikeyMessage *> errMsg, const char* msg):MikeyException(msg),errorMessageValue(errMsg){

}

MikeyExceptionMessageContent::~MikeyExceptionMessageContent() throw(){

}


MRef<MikeyMessage *> MikeyExceptionMessageContent::errorMessage(){
        return errorMessageValue;
}


MikeyExceptionMessageLengthException::MikeyExceptionMessageLengthException(const char* msg): MikeyException(msg){

}

MikeyExceptionMessageLengthException::~MikeyExceptionMessageLengthException() throw() {

}

MikeyExceptionAuthentication::MikeyExceptionAuthentication(const char* msg): MikeyException(msg){

}

MikeyExceptionAuthentication::~MikeyExceptionAuthentication() throw() {

}

MikeyExceptionUnacceptable::MikeyExceptionUnacceptable(const char* msg): MikeyException(msg){

}

MikeyExceptionUnacceptable::~MikeyExceptionUnacceptable() throw (){

}

MikeyExceptionUnimplemented::MikeyExceptionUnimplemented(const char* msg): MikeyException(msg){

}

MikeyExceptionUnimplemented::~MikeyExceptionUnimplemented() throw(){

}

