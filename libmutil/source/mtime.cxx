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


#include<stdint.h>

#ifdef WIN32
#include <sys/timeb.h>
#else
#include<sys/time.h>
#include<time.h>
#endif

uint64_t mtime(){
#ifdef WIN32
	struct _timeb tb;
	_ftime (&tb);

	return ((uint64_t)tb.time) * 1000LL + ((uint64_t)tb.millitm);
#else // FIXME: do a proper check for gettimeofday
	struct timeval tv;

	gettimeofday( &tv, NULL );

	return ((uint64_t)tv.tv_sec) * (uint64_t)1000 + ((uint64_t)tv.tv_usec) / (uint64_t)1000;
#endif
}
