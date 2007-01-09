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
 *	    Pan Xuan <xuan@kth.se>
*/

#include<config.h>

#include <libmcrypto/sha1.h>

#include <stdlib.h>
#include <string.h>

#include <openssl/crypto.h>
#include <openssl/sha.h>

void sha1(unsigned char * data, unsigned int data_length,
	  unsigned char * digest ){
	SHA1(data, (unsigned long)data_length, digest);
}
