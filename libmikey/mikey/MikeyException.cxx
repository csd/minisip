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

MikeyException::MikeyException(string message){
	this->messageValue=message;
}

string MikeyException::message(){
	return messageValue;
}


MikeyExceptionUninitialized::MikeyExceptionUninitialized(string msg): MikeyException(msg){

}

MikeyExceptionUninitialized::~MikeyExceptionUninitialized(){

}


MikeyExceptionNullPointerException::MikeyExceptionNullPointerException(string msg): MikeyException(msg){

}


MikeyExceptionNullPointerException::~MikeyExceptionNullPointerException(){

}



MikeyExceptionMessageContent::MikeyExceptionMessageContent(string msg): MikeyException(msg){

}

MikeyExceptionMessageContent::MikeyExceptionMessageContent(MikeyMessage * errMsg, string msg):MikeyException(msg),errorMessageValue(errMsg){

}

MikeyExceptionMessageContent::~MikeyExceptionMessageContent(){

}

MikeyMessage * MikeyExceptionMessageContent::errorMessage(){
	return errorMessageValue;
}


MikeyExceptionMessageLengthException::MikeyExceptionMessageLengthException(string msg): MikeyException(msg){

}

MikeyExceptionMessageLengthException::~MikeyExceptionMessageLengthException(){

}

MikeyExceptionAuthentication::MikeyExceptionAuthentication(string msg): MikeyException(msg){

}

MikeyExceptionAuthentication::~MikeyExceptionAuthentication(){

}

MikeyExceptionUnacceptable::MikeyExceptionUnacceptable(string msg): MikeyException(msg){

}

MikeyExceptionUnacceptable::~MikeyExceptionUnacceptable(){

}

MikeyExceptionUnimplemented::MikeyExceptionUnimplemented(string msg): MikeyException(msg){

}

MikeyExceptionUnimplemented::~MikeyExceptionUnimplemented(){

}

