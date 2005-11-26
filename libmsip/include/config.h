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


#ifdef _MSC_VER
#define W32			//FIXME: not correct for WCE env?

#pragma warning (disable: 4251)

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


/* Compilation time configuration */
#include"compilation_config.h"

#include<stdint.h>

#endif

// FIXME!!
                                                                                                                             
#ifndef WIN32
#define LINUX
#endif

#ifdef DEBUG_OUTPUT
#define MSM_DEBUG
#define MSM_DEBUG_COMMAND
#endif

#ifndef DEBUG_OUTPUT
#define NDEBUG
#endif

typedef uint8_t byte_t;

#endif

