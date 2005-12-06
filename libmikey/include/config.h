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


#ifndef CONFIG_H
#define CONFIG_H

#ifndef LIBMIKEY_EXPORTS
# ifdef DLL_EXPORT
#  define LIMMUTIL_IMPORTS
#  define LIBMIKEY_EXPORTS
#  define OPENSSL_OPT_WINDLL	// import Windows dll
# endif	 // DLL_EXPORT
#endif	// LIBMIKEY_EXPORTS


#ifdef _MSC_VER 

#pragma warning (disable: 4251)

#ifndef WIN32
#define WIN32
#endif

#ifndef uint8_t
typedef unsigned char  uint8_t;
#endif

#ifndef byte_t
typedef unsigned char  byte_t;
#endif

#ifndef int16_t
typedef __int16  int16_t;
#endif

#ifndef uint16_t
typedef unsigned short  uint16_t;
#endif

#ifndef int32_t
typedef __int32  int32_t;
#endif

#ifndef uint32_t
typedef unsigned int  uint32_t;
#endif

#ifndef int64_t
typedef __int64  int64_t;
#endif

#ifndef uint64_t
typedef unsigned long long  uint64_t;
#endif

#else
 
#include<stdint.h>

#ifdef __CYGWIN__
#define WIN32
#endif

/* Compilation time configuration */
#include"compilation_config.h"

#endif

/* STL replacement */


#include<string>
using namespace std;

typedef uint8_t byte_t;

/* big/little endian conversion */

static inline uint16_t U16_AT( void const * _p )
{
    const uint8_t * p = (const uint8_t *)_p;
    return ( ((uint16_t)p[0] << 8) | p[1] );
}
static inline uint32_t U32_AT( void const * _p )
{
    const uint8_t * p = (const uint8_t *)_p;
    return ( ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16)
              | ((uint32_t)p[2] << 8) | p[3] );
}
static inline uint64_t U64_AT( void const * _p )
{
    const uint8_t * p = (const uint8_t *)_p;
    return ( ((uint64_t)p[0] << 56) | ((uint64_t)p[1] << 48)
              | ((uint64_t)p[2] << 40) | ((uint64_t)p[3] << 32)
              | ((uint64_t)p[4] << 24) | ((uint64_t)p[5] << 16)
              | ((uint64_t)p[6] << 8) | p[7] );
}


#endif
