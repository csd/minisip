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
#include<libmikey/MikeyMessage.h>
#include<libmikey/MikeyException.h>
#include<libmikey/KeyAgreementDHHMAC.h>

#include<string>

using namespace std;

KeyAgreementDHHMAC::KeyAgreementDHHMAC( const unsigned char * pskPtr,
					int pskLengthValue )
		:KeyAgreementPSK(pskPtr, pskLengthValue),
		 KeyAgreementDHBase(NULL){
}

KeyAgreementDHHMAC::~KeyAgreementDHHMAC(){
}

int32_t KeyAgreementDHHMAC::type(){
	return KEY_AGREEMENT_TYPE_DHHMAC;
}

MikeyMessage* KeyAgreementDHHMAC::createMessage(){
	return MikeyMessage::create( this );
}
