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

#ifndef HMAC_SHA256_H
#define HMAC_SHA256_H

#include <libmcrypto/config.h>

LIBMCRYPTO_API void hmac_sha256( unsigned char * key, unsigned int key_length,
                unsigned char * data, unsigned int data_length,
                unsigned char * mac, unsigned int * mac_length );

LIBMCRYPTO_API void hmac_sha256( unsigned char * key, unsigned int key_length,
                unsigned char * data[], unsigned int data_length[],
                unsigned char * mac, unsigned int * mac_length );
#endif
