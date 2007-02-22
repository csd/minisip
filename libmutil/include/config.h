/*
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* Copyright (C) 2004 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#ifndef CONFIG_H
#define CONFIG_H

/* Compilation time configuration */
#ifndef _MSC_VER
#	include"compilation_config.h"
#else
#	include"compilation_config_w32_wce.h"
#endif

#ifndef LIBMUTIL_EXPORTS
# ifdef DLL_EXPORT
#  define LIBMUTIL_EXPORTS
#  define OPENSSL_OPT_WINDLL	// import Windows dll
# endif	 // DLL_EXPORT
#endif	// LIBMUTIL_EXPORTS

#include<libmutil/mtypes.h>

#ifdef _MSC_VER
	#define WIN32
	#pragma warning (disable: 4251)

	#ifndef LIBMUTIL_EXPORTS
		#error Visual Studio is not set up correctly to compile libmutil to a .dll (LIBMUTIL_EXPORTS not defined).
	#endif
#endif

//Temporary ... STLPort does not allow addition of errno.h ... but WCEcompat does ... 
//So we don't have to repeat this everytime, include errno.h for all files ... 
//	anyway, it is just an int :)
//Anyway, while compiling for EVC, it will still trigger some warnings ... ignore them, errno exhists for sure.
#ifdef _WIN32_WCE
#	ifndef _STLP_NATIVE_ERRNO_H_INCLUDED
#		include<wcecompat/errno.h>
#		define _STLP_NATIVE_ERRNO_H_INCLUDED
#	endif
#else
#	include <errno.h>
#endif
/*big/little endian conversion*/

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
#if defined WORDS_BIGENDIAN
#   define hton16(i)   ( i )
#   define hton32(i)   ( i )
#   define hton64(i)   ( i )
#   define ntoh16(i)   ( i ) 
#   define ntoh32(i)   ( i )
#   define ntoh64(i)   ( i )
#else
#   define hton16(i)   U16_AT(&i)
#   define hton32(i)   U32_AT(&i)
#   define hton64(i)   U64_AT(&i)
#   define ntoh16(i)   U16_AT(&i)
#   define ntoh32(i)   U32_AT(&i)
#   define ntoh64(i)   U64_AT(&i)
#endif


#ifdef DEBUG_OUTPUT
#define MSM_DEBUG
#define MSM_DEBUG_COMMAND
#endif

#endif
