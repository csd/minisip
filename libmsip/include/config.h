/*
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
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

#ifndef LIBMSIP_EXPORTS
# ifdef DLL_EXPORT
#  define LIBMUTIL_IMPORTS
#  define LIBMNETUTIL_IMPORTS
#  define LIBMSIP_EXPORTS
# endif	 // DLL_EXPORT
#endif	// LIBMSIP_EXPORTS


#include<libmutil/mtypes.h>

#ifdef _MSC_VER
	#define WIN32
	#pragma warning (disable: 4251)
	#pragma warning (disable: 4290)

	#ifndef LIBMSIP_EXPORTS
		#error Visual Studio is not set up correctly to compile libmutil to a .dll (LIBMSIP_EXPORTS not defined).
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

#endif

