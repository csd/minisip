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


#include<config.h>

#include<libmutil/mtime.h>

#include<time.h>

#include <sys/timeb.h>
#include <winsock2.h>


LIBMUTIL_API uint64_t mtime(){
#ifdef _WIN32_WCE
	struct _timeb tb;
	_ftime (&tb);
#elif defined(__MINGW32__)
	// struct __timeb64 tb;  // requires v.61 or higher or msvcrt.dll
	// _ftime64 (&tb); 
	struct _timeb tb;
	_ftime (&tb);
#else
	struct __timeb64 tb;
	_ftime64_s (&tb);
#endif

	return ((uint64_t)tb.time) * (int64_t)1000 + ((uint64_t)tb.millitm);
}


#ifndef HAVE_GETTIMEOFDAY
extern "C" {
LIBMUTIL_API
	void gettimeofday (struct timeval *tv, struct timezone *tz){
#ifdef _WIN32_WCE
		struct _timeb tb;
		_ftime (&tb);
#elif defined(__MINGW32__)
		//struct __timeb64 tb;
		//_ftime64 (&tb);
		struct _timeb tb;
		_ftime (&tb);
#else
		struct __timeb64 tb;
		_ftime64_s (&tb);
#endif

		tv->tv_sec = (long)tb.time; // Fix: tv_sec wraps year 2038 (tv.time is ok though)
		tv->tv_usec = tb.millitm * 1000L;
		if( tz ){
			tz->tz_minuteswest = tb.timezone;	/* minutes west of Greenwich  */
			tz->tz_dsttime = tb.dstflag;	/* type of dst correction  */
		}
	}
}
#endif	// HAVE_GETTIMEOFDAY

LIBMUTIL_API int msleep(int32_t ms){
	Sleep(ms); //function returns void
	return 0;
}
