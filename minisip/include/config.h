/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/* Copyright (C) 2004 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#ifndef CONFIG_H
#define CONFIG_H

/* Compilation time configuration */
#if defined(_WIN32_WCE) || defined(_MSC_VER)
#	include"compilation_config_w32_wce.h"
#else
#	include"compilation_config.h"
#endif

#include<libmutil/mtypes.h>

#define LIBMUTIL_IMPORTS
#define LIBMNETUTIL_IMPORTS
#define LIBMSIP_IMPORTS
#define LIBMIKEY_IMPORTS
#define LIBMINISIP_IMPORTS

#define SOUND_CARD_FREQ 48000

#if defined _MSC_VER || defined __MINGW32__
#	ifndef WIN32
#		define WIN32
#	endif	
#endif

#ifdef WIN32
	//In windows, use direct sound for all, except in windows ce (use then Wave in/out)
#	ifdef _WIN32_WCE
#		define WAVE_SOUND
#	else
#		define DSOUND
#	endif

#	ifndef _WIN32_WINNT
#		define _WIN32_WINNT 0x0500
#	endif
	
	//warning message: class member needs to have dll-interface to be used by clients of class 'XXX'
#	ifndef __MINGW32__
#		pragma warning (disable: 4251)
#	endif

#	ifdef __MINGW32__
#		define _WIN32_WINNT 0x0500
#		define DSOUND
#	endif	// !__MINGW32__

#else
#	define LINUX
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
#	include<openssl/err.h>
#else
#	include <errno.h>
#endif

/*
#ifdef USE_STL
#	undef __NO_ISOCEXT
	using namespace std;
	#include<string>
#endif
*/

#ifdef DEBUG_OUTPUT
#	define MSM_DEBUG
#	define MSM_DEBUG_COMMAND
#endif


#if defined(WIN32) && !defined(_WIN32_WCE) && !defined(__MINGW32__)
#	define TEXT_UI
//#define DEBUG_OUTPUT
#	include<iostream>
	using namespace std;
#	pragma message ("USING NAMESPACE STD ... in config.h")
#endif

#ifndef HAVE_UINT
	typedef unsigned int uint;
#endif

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


#endif




