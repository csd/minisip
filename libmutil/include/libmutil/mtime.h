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



#ifndef MINISIP_TIME_H
#define MINISIP_TIME_H

#include <libmutil/libmutil_config.h>

#include<libmutil/mtypes.h>
#include<string>

#if defined WIN32 || defined _MSC_VER
	#include<time.h>
	#include <sys/timeb.h>
	#ifdef __MINGW32__
	#	include<sys/time.h>
	#endif

	#ifndef HAVE_STRUCT_TIMEZONE
	/* Emulate gettimeofday (Ulrich Leodolter, 1/11/95).  */
	struct timezone{
			int tz_minuteswest;     /* Minutes west of GMT.  */
			int tz_dsttime;     /* Nonzero if DST is ever in effect.  */
	};
	#endif	// HAVE_STRUCT_TIMEZONE

	#ifdef _WIN32_WCE
	//MS EVC++ 4.0 defines struct timeval in winsock.h and winsock2.h ... 
	//it is sad, but we include winsock2.h just to obtain this structure ... 
		#include<winsock2.h>
	#endif

	#ifndef HAVE_GETTIMEOFDAY
extern "C" {
	LIBMUTIL_API void gettimeofday (struct timeval *tv, struct timezone *tz);
}
	#endif	// HAVE_GETTIMEOFDAY
#endif

LIBMUTIL_API uint64_t mtime();

/**
 * Purpose: Implement a platform independent way of delaying 
 *		execution of a thread.
 * @arg msec	The thread will suspend sleep for at least the 
 *		requested number of milliseconds.
 * @return -1 if error, else otherwise
 */
LIBMUTIL_API int msleep(int32_t msec);

LIBMUTIL_API std::string nowStr();

#endif
