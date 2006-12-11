/*
  Copyright (C) 2006 Zachary T Welch

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
 * Authors: Zachary T Welch <zach-minisip@splitstring.com>
 */

#ifndef MLIBMCRYPTO_HMAC_H
#define MLIBMCRYPTO_HMAC_H

// XXX: replace this forward compatibility layer with a Bridge interface
#include<libmcrypto/uninst_config.h>

#include<config.h>
#ifdef HAVE_OPENSSL
#include<libmcrypto/openssl/hmac.h>
#elif defined( HAVE_GNUTLS )
#error "gnutls hmac support is not complete"
#endif // HAVE_GNUTLS

#endif	// MLIBMCRYPTO_HMAC_H

